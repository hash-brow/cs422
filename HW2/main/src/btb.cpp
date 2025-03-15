#include "btb.h"

BTB::BTB(){
	cache.resize(BTB_SETS, std::vector<BTB_ENTRY>(BTB_WAYS, {false, 0, 0, 0}));
	preds = misses = fails = 0;
}

void BTB::update_lru(int setIndex, int wayIndex){
	for(int i = 0; i < BTB_WAYS; i++) {
		if(cache[setIndex][i].valid && cache[setIndex][i].lru_state < cache[setIndex][wayIndex].lru_state){
			cache[setIndex][i].lru_state++;
		}
	}
	cache[setIndex][wayIndex].lru_state = 0;
}

void BTB::btb_fill(ADDRINT insAddr, ADDRINT branchAddr, BOOL taken, UINT32 insSize){
	UINT32 btbIdx = insAddr & MASK_128;
	ADDRINT tag = insAddr >> 7;
	ADDRINT nextIns = insAddr + insSize;
	BOOL found = false;

	ADDRINT target = nextIns;

	int colIdx = 0;

	for(int i = 0; i < BTB_WAYS; i++){
		if(cache[btbIdx][i].valid && cache[btbIdx][i].tag == tag){
			found = true;
			target = cache[btbIdx][i].target;
			colIdx = i;
			break;
		}
	}

	preds++;

	if(taken){
		fails += (target != branchAddr);
		
		if(found && (branchAddr != target))
			cache[btbIdx][colIdx].target = branchAddr;

		if(!found && (branchAddr != target)){
			int lruIdx = 0;

			for(int i = 0; i < BTB_WAYS; i++){
				if(!cache[btbIdx][i].valid){
					lruIdx = i;
					break;
				}

				if(cache[btbIdx][i].lru_state > cache[btbIdx][lru_index].lru_state){
					lruIdx = i;
				}
			}

			update_lru(btbIdx, lruIdx)
			cache[btbIdx][lruIdx] = {true, insAddr, 0, branchAddr};
		}
	} else {
		fails += (target != nextIns);
		if(found)
			cache[btbIdx][colIdx].valid = false;
	}

	if(found) update_lru(btbIdx, colIdx);
	else misses++;
}
