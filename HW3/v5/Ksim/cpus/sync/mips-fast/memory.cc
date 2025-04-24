#include "memory.h"

Memory::Memory (Mipc *mc)
{
   _mc = mc;
}

Memory::~Memory (void) {}

void
Memory::MainLoop (void)
{
   while (1) {
      AWAIT_P_PHI0; // @posedge
      
      if (!_mc->_em->_valid) {
        AWAIT_P_PHI1;
        continue;
      }

      pipe_reg_t* mw = new pipe_reg_t;
      *mw = *_mc->_em;

      _mc->_mem_ex.lo = mw->_opResultLo;
      _mc->_mem_ex.hi = mw->_opResultHi;

      if (mw->_isIllegalOp) {
         AWAIT_P_PHI1;
         _mc->_mw->_isIllegalOp = TRUE;
         delete mw;
         continue;
      }

      if (mw->_memControl) {
         mw->_memOp(_mc, _mc->_em, mw);

         if (mw->_writeREG || mw->_writeFREG) {
            _mc->_mem_ex.lo = mw->_opResultLo;
            _mc->_mem_mem.lo = mw->_opResultLo;
         }
         
         AWAIT_P_PHI1; // @negedge
         *_mc->_mw = *mw;

#ifdef MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Accessing memory at address %#x for ins %#x\n", SIM_TIME, mw->_memory_addr_reg, mw->_ins);
#endif
      } else {
         AWAIT_P_PHI1; // @negedge
         *_mc->_mw = *mw;
#ifdef MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Memory has nothing to do for ins %#x\n", SIM_TIME, mw->_ins);
#endif
      }
      delete mw;
   }
}
