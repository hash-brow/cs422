#include "../include/btb.h"

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

void BTB::btb_fill(BTB* self, ADDRINT insAddr, ADDRINT branchAddr, BOOL taken, UINT32 insSize){
	UINT32 btbIdx = insAddr & 127;
	ADDRINT tag = insAddr >> 7;
	ADDRINT nextIns = insAddr + insSize;
	BOOL found = false;

	ADDRINT target = nextIns;

	int colIdx = 0;

	for(int i = 0; i < BTB_WAYS; i++){
		if(self->cache[btbIdx][i].valid && self->cache[btbIdx][i].tag == tag){
			found = true;
			target = self->cache[btbIdx][i].target;
			colIdx = i;
			break;
		}
	}

	self->preds++;

	if(taken){
		self->fails += (target != branchAddr);
		
		if(found && (branchAddr != target))
			self->cache[btbIdx][colIdx].target = branchAddr;

		if(!found && (branchAddr != target)){
			int lruIdx = 0;

			for(int i = 0; i < BTB_WAYS; i++){
				if(!self->cache[btbIdx][i].valid){
					lruIdx = i;
					break;
				}

				if(self->cache[btbIdx][i].lru_state > self->cache[btbIdx][lruIdx].lru_state){
					lruIdx = i;
				}
			}

			self->update_lru(btbIdx, lruIdx);
			self->cache[btbIdx][lruIdx] = {true, insAddr, 0, branchAddr};
		}
	} else {
		self->fails += (target != nextIns);
		if(found)
			self->cache[btbIdx][colIdx].valid = false;
	}

	if(found) self->update_lru(btbIdx, colIdx);
	else self->misses++;
}
