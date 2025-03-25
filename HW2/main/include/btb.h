#ifndef _BTB_H
#define _BTB_H

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

class BTB {
	private:
		static const int BTB_SETS = 128;
		static const int BTB_WAYS = 4;
		std::vector<std::vector<BTB_ENTRY>> cache;
		UINT32 preds, misses, fails;

		void update_lru(int setIndex, int wayIndex);
	public:
		BTB();
		static void btb_fill(BTB* self, ADDRINT insAddr, ADDRINT branchAddr, BOOL taken, UINT32 insSize);
		void print(std::ostream& OutFile);
};

#endif
