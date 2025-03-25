#include "../include/hashbtb.h"

HASHBTB::HASHBTB(){
	cache.resize(BTB_SETS, std::vector<HASHBTB_ENTRY>(BTB_WAYS, {false, 0, 0, 0}));
	preds = misses = fails = 0;

	ghr = 0;
	lru_timer = 0;
}

void HASHBTB::update_lru(int setIndex, int wayIndex){
	// for(int i = 0; i < BTB_WAYS; i++) {
	// 	if(cache[setIndex][i].valid && cache[setIndex][i].lru_state < cache[setIndex][wayIndex].lru_state){
	// 		cache[setIndex][i].lru_state++;
	// 	}
	// }
	// cache[setIndex][wayIndex].lru_state = 0;

	cache[setIndex][wayIndex].lru_state = ++lru_timer;
}

void HASHBTB::btb_fill(HASHBTB* self, ADDRINT insAddr, ADDRINT branchAddr, BOOL taken, UINT32 insSize){
	// std::cout << "hashbtb" << std::endl;

	UINT32 btbIdx = (insAddr & 127) ^ self->ghr;
	ADDRINT tag = insAddr;
	ADDRINT nextIns = insAddr + insSize;
	BOOL found = false;

	ADDRINT target = nextIns;

	int colIdx;

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
			int lruIdx = -1;
			for(int i = 0; i < BTB_WAYS; i++){
				if(!self->cache[btbIdx][i].valid){
					lruIdx = i;
					break;
				}
			}
			if (lruIdx == -1) {
				lruIdx = 0;
				for(int i = 1; i < BTB_WAYS; i++){
					if(self->cache[btbIdx][i].lru_state < self->cache[btbIdx][lruIdx].lru_state){
						lruIdx = i;
					}
				}
			}

			self->cache[btbIdx][lruIdx] = {true, insAddr, 0LL, branchAddr};
			self->update_lru(btbIdx, lruIdx);
		}
	} else {
		self->fails += (target != nextIns);
		if(found) self->cache[btbIdx][colIdx].valid = false;
	}

	if(found) self->update_lru(btbIdx, colIdx);
	else self->misses++;
}

void HASHBTB::print(std::ostream& OutFile){
	OutFile << std::fixed << std::setprecision(12) << "Accesses " << preds;
	OutFile << ", Misses " << misses << " (" << (double) misses / preds << ")";
	OutFile << ", Mispredictions " << fails << " (" << (double) fails / preds << ")\n";
}

void HASHBTB::update_ghr(BOOL taken){
	if(taken) ghr = ((ghr << 1) | 1) & 127;
	else ghr = ((ghr << 1) | 0) & 127;
}
