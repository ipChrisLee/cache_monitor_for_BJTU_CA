#include <algorithm>

#include "moe/log.hpp"
#include "moe/fmt_string.hpp"
#include "cache_monitor.hpp"

namespace cache_monitor {

CacheMonitor::CacheMonitor(
	std::string name, u64 blockSize, u64 blockCntPerSet, u64 setCntPerCache,
	u64 blockCntPerCache
) : name(std::move(name)),
    BLOCK_SIZE(blockSize),
    BLOCK_CNT_PER_SET(blockCntPerSet),
    SET_CNT_PER_CACHE(setCntPerCache),
    BLOCK_CNT_PER_CACHE(blockCntPerCache),
    cache({SET_CNT_PER_CACHE, {BLOCK_CNT_PER_SET, CacheSetLine{ }}}) {
	moe_assert(BLOCK_CNT_PER_CACHE == BLOCK_CNT_PER_SET * SET_CNT_PER_CACHE);
}

void CacheMonitor::access(u64 addr) {
	++timNow;
	++accessCnt;
	auto blockIndex = (addr / BLOCK_SIZE);
	auto setIndex = blockIndex % SET_CNT_PER_CACHE;
//	moe_slog_info("", moe::format_str("blockIndex=0x%LLX", blockIndex),
//	              moe::format_str("setIndex=0x%LLX", setIndex));
	/*    |- setIndex -| |- offset -|
	 *  |- blockIndex -| |- offset -|
	 */
	auto hit = u64(0);
	auto invalidLineCnt = u64(0);
	auto lruTim = u64(-1);
	for (auto & cacheSetLine: cache[setIndex]) {
		if (cacheSetLine.valid && cacheSetLine.blockIndex == blockIndex) {
			//  hit
			++hit;
			cacheSetLine.tim = timNow;
		}
		invalidLineCnt += !cacheSetLine.valid;
		lruTim = std::min(lruTim, cacheSetLine.tim);
	}
	if (hit == 1) {
		++hitCnt;
	} else if (hit == 0) {
		++missCnt;
		if (invalidLineCnt) { //  there is invalid line, replace it.
			for (auto & cacheSetLine: cache[setIndex]) {
				if (!cacheSetLine.valid) {
					cacheSetLine = {
						.valid=true,
						.blockIndex=blockIndex,
						.tim=timNow
					};
					break;
				}
			}
		} else {  //  replace least recently used line.
			for (auto & cacheSetLine: cache[setIndex]) {
				if (cacheSetLine.tim == lruTim) {
					cacheSetLine = {
						.valid=true,
						.blockIndex=blockIndex,
						.tim=timNow
					};
					break;
				}
			}
		}
	} else {
		moe_panic("hit twice.");
	}
}

std::string CacheMonitor::cache_to_string() const {
	/*
	 * |-1-| |-1-| |- 2 -| |-    8    -| |-2-| |-   8   -|
	 *  0/1          0x     block index            tim
	 */
	auto res = std::string();
	for (auto iSet = u64(0); iSet < SET_CNT_PER_CACHE; ++iSet) {
		res += "=========" + moe::format_str("0x%02LLX", iSet) + "=========\n";
		for (auto & cacheSetLine: cache[iSet]) {
			res += std::to_string(i32(cacheSetLine.valid)) + " ";
			res += moe::format_str("0x%8LLX", cacheSetLine.blockIndex);
			res += moe::format_str("  %8lld", cacheSetLine.tim);
			res += "\n";
		}
	}
	res += "======================";
	return res;
}

std::string CacheMonitor::res_to_string() const {
	auto res = std::string();
	res += moe::format_str("timNow      =%lld\n", timNow);
	res += moe::format_str("accessCnt   =%lld\n", accessCnt);
	res += moe::format_str("missCnt     =%lld\n", missCnt);
	res += moe::format_str("hitCnt      =%lld\n", hitCnt);
	return res;
}

std::string CacheMonitor::res_csv_header() {
	auto res = std::string();
	res += "name,cache size,block size,block cnt per set,set cnt per cache,access cnt,hit cnt,miss cnt\n";
	return res;
}

std::string CacheMonitor::res_to_csv_line() const {
	auto res = std::string();
	res += name;
	//  cache size
	res += moe::format_str(",%lld", BLOCK_CNT_PER_CACHE * BLOCK_SIZE);
	//  block size
	res += moe::format_str(",%lld", BLOCK_SIZE);
	//  block cnt per set
	res += moe::format_str(",%lld", BLOCK_CNT_PER_SET);
	//  set cnt per cache
	res += moe::format_str(",%lld", SET_CNT_PER_CACHE);
	//  access cnt
	res += moe::format_str(",%lld", accessCnt);
	//  hit cnt
	res += moe::format_str(",%lld", hitCnt);
	//  miss cnt
	res += moe::format_str(",%lld", missCnt);
	res += "\n";
	return res;
}

}
