#ifndef _HASHBTB_H
#define _HASHBTB_H

#include "pin.H"
#include <vector>
#include <iostream>
#include <cstdint>

struct BTB_ENTRY {
	BOOL valid;
	ADDRINT tag;
	UINT64 lru_state;
	ADDRINT target;
};

class HASHBTB {
	private:
		static const int BTB_SETS = 128;
		static const int BTB_WAYS = 4;
		std::vector<std::vector<BTB_ENTRY>> cache;
		UINT32 preds, misses, fails;
		UINT32 ghr;

		void update_lru(int setIndex, int wayIndex);
	public:
		HASHBTB();
		void btb_fill(ADDRINT insAddr, ADDRINT branchAddr, BOOL taken, UINT32 insSize);
};

#endif
