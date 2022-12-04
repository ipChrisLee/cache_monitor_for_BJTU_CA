#pragma once


#include <vector>

#include "moe/moe_typedef.hpp"
#include "moe/rt_check.hpp"

namespace cache_monitor {

class CacheMonitor {
  public:
	const std::string name;
	const u64 BLOCK_SIZE;               //  how many bytes in a cache block?
	const u64 BLOCK_CNT_PER_SET;        //  how many blocks in a set?
	const u64 SET_CNT_PER_CACHE;        //  how many set in cache?
	const u64 BLOCK_CNT_PER_CACHE;      //  how many block in cache?
	/*
	 *  BLOCK_CNT_PER_CACHE = BLOCK_CNT_PER_SET * SET_CNT_PER_CACHE
	 *  BLOCK_SIZE * BLOCK_CNT_PER_CACHE = CACHE_SIZE
	 *
	 *  Direct mapping cache:
	 *      BLOCK_CNT_PER_SET = 1
	 *      SET_SIZE = BLOCK_CNT_PER_CACHE
	 *  Fully associative cache:
	 *      BLOCK_CNT_PER_SET = BLOCK_CNT_PER_CACHE
	 *      SET_CNT_PER_CACHE = 1
	 *  Set-associative cache:
	 *      BLOCK_CNT_PER_SET != 1
	 *      SET_CNT_PER_CACHE != 1
	 */
	
	//  one line in a set
	class CacheSetLine {
	  public:
		bool valid = false;
		u64 blockIndex = 0;
		u64 tim = 0;
	};
	
	using CacheSet = std::vector<CacheSetLine>;
	std::vector<CacheSet> cache;
	
	CacheMonitor(
		std::string name, u64 blockSize, u64 blockCntPerSet, u64 setCntPerCache,
		u64 blockCntPerCache
	);
	
	u64 timNow = 0;
	u64 accessCnt = 0;
	u64 compulsoryMissCnt = 0;
	u64 capacityMissCnt = 0;
	u64 conflictMissCnt = 0;
	u64 missCnt = 0;
	u64 hitCnt = 0;
	u64 blockOcrCnt=0;  //  how many blocks in cache is occupied.
	void access(u64 addr);
	
	[[nodiscard]] std::string cache_to_string() const;
	
	[[nodiscard]] std::string res_to_string() const;
	
	[[nodiscard]] std::string res_to_csv_line() const;
	static std::string res_csv_header();
	
	
};

}
