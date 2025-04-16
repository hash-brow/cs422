#include "memory.h"

Memory::Memory (Mipc *mc)
{
   _mc = mc;
}

Memory::~Memory (void) {}

void
Memory::MainLoop (void)
{
   Bool memControl;

   while (1) {
      AWAIT_P_PHI1; // @negedge
      pipe_reg_t* em = new pipe_register_t(_mc->_em);

      if (em->_valid) {
         AWAIT_P_PHI0; // @posedge
         if (em->_memControl) {
            _mc->memOp(_mc, em, _mc->_mw);
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Accessing memory at address %#x for ins %#x\n", SIM_TIME, em->_memory_addr_reg, em->_ins);
#endif
         } else {
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Memory has nothing to do for ins %#x\n", SIM_TIME, em->_ins);
#endif
         }

         _mc->_mw->_valid = TRUE;
      } else {
         AWAIT_P_PHI0; // @posedge
         _mc->_mw->_valid = FALSE;
      }

      delete em;
   }
}
