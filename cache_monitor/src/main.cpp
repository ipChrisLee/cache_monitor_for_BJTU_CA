#include <iostream>
#include <fstream>
#include <vector>

#include "moe/arg_parser.hpp"
#include "moe/log.hpp"
#include "moe/fmt_string.hpp"

#include "cache_monitor.hpp"


int main(int argc, char ** argv) {
	moe::register_std_log("demo/cache_monitor_slog.txt", "\n");
	
	struct Info {
		std::string accessListFilePath;
		std::string csvResFilePath;
	} info;
	auto argParser = moe::ArgParser("CacheMonitor");
	argParser.add_option(
		moe::ArgOption(
			std::nullopt, "in", true, [&info](std::string_view s) {
				info.accessListFilePath = s;
			}, std::nullopt
		)
	);
	argParser.add_option(
		moe::ArgOption(
			std::nullopt, "csv_out", true, [&info](std::string_view s) {
				info.csvResFilePath = s;
			}, std::nullopt
		)
	);
	argParser.parse(argc, argv);
	
	auto accessList = [&info]() {
		auto accessListInStream = std::ifstream(info.accessListFilePath);
		auto list = std::vector<u64>();
		auto tmp = u64(0);
		while (accessListInStream >> tmp) {
			list.emplace_back(tmp);
		}
		return list;
	}();
	auto csvResFileOStream = std::ofstream(info.csvResFilePath);
	
	auto cacheMonitors = std::vector<cache_monitor::CacheMonitor>();
	for (auto blockCntPerCache: std::vector<u64>{16, 32, 64}) {
		for (auto blockCntPerSet: std::vector<u64>{1, 2, 4, blockCntPerCache}) {
			auto setCntPerCache = blockCntPerCache / blockCntPerSet;
			auto name = blockCntPerSet == 1 ? "direct mapping cache" : (
				setCntPerCache == 1 ? "fully associative cache" : "set-associative cache"
			);
			cacheMonitors.emplace_back(
				name, 16, blockCntPerSet, setCntPerCache, blockCntPerCache
			);
		}
	}
	csvResFileOStream << cache_monitor::CacheMonitor::res_csv_header();
	for (auto & cacheMonitor: cacheMonitors) {
		for (auto & addr: accessList) {
//		moe_slog_disable_here();
//		moe_slog_info(moe::format_str("addr=0x%LLX", addr));
			cacheMonitor.access(addr);
//		moe_slog_info("cache", cacheMonitor.cache_to_string());
		}
		csvResFileOStream << cacheMonitor.res_to_csv_line();
	}
	return 0;
}
