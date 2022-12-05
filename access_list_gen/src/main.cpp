#include <fstream>
#include <iostream>
#include <optional>
#include <charconv>

#include "moe/arg_parser.hpp"
#include "moe/rt_check.hpp"
#include "moe/fmt_string.hpp"
#include "moe/moe_typedef.hpp"

void data_from_stride(int stride, std::string_view targetMemTraceFilePath) {
	auto ofs = std::ofstream(targetMemTraceFilePath);
	for (int i = 0; i < 10000; ++i) {
		for (int j = 0; j < 256; j += stride) {
			ofs << j * 4 << "\n";
		}
	}
}

void
transfer_from_lackey(std::string_view lackeyMemTraceFilePath, std::string_view targetMemTraceFilePath) {
	auto ifs = std::ifstream(lackeyMemTraceFilePath);
	moe_assert(ifs.is_open());
	auto ofs = std::ofstream(targetMemTraceFilePath);
	auto buf = std::string();
	auto ma = std::vector<u64>();
	while (std::getline(ifs, buf)) {
		if (buf.substr(0, 2) == "==") {
			continue;
		}
		buf = std::string("0x") + buf.substr(3);
		buf = std::string(buf.begin(), std::find(buf.begin(), buf.end(), ','));
		auto addr = std::stoull(buf, nullptr, 16);
		ofs << addr << '\n';
	}
}

int main(int argc, char ** argv) {
	struct Info {
		std::optional<int> stride = std::nullopt;
		std::optional<std::string> lackeyFilePath = std::nullopt;
		std::optional<std::string> targetMemTraceFilePath = std::nullopt;
	} info;
	auto argParser = moe::ArgParser("access_list_gen");
	argParser.add_option(
		moe::ArgOption(
			std::nullopt, "stride", true, [&info](std::string_view s) {
				int st = 0;
				auto res = std::from_chars(s.begin(), s.end(), st);
				moe_assert(res.ec == std::errc());
				info.stride = st;
			}, std::nullopt
		)
	);
	argParser.add_option(
		moe::ArgOption(
			std::nullopt, "lackey_mtrace_file_path", true, [&info](std::string_view s) {
				info.lackeyFilePath = s;
			}, std::nullopt
		)
	);
	argParser.add_option(
		moe::ArgOption(
			'o', std::nullopt, true, [&info](std::string_view s) {
				info.targetMemTraceFilePath = s;
			}, std::nullopt
		)
	);
	argParser.parse(argc, argv);
	
	moe_assert(info.targetMemTraceFilePath.has_value());
	if (info.stride.has_value()) {
		data_from_stride(info.stride.value(), info.targetMemTraceFilePath.value());
	} else if (info.lackeyFilePath.has_value()) {
		transfer_from_lackey(info.lackeyFilePath.value(), info.targetMemTraceFilePath.value());
	} else {
		moe_panic("");
	}
	return 0;
}