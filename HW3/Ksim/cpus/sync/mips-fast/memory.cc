#include "memory.h"

Memory::Memory (Mipc *mc)
{
   _mc = mc;
}

Memory::~Memory (void) {}

void
Memory::MainLoop (void)
{
   /*
    * +ve half:
    *    - reads _mc->_em into local registers
    * -ve half:
    *    - performs memory op, if one exists
    *    - copies results to _mc->_mw
    */

   while (1) {
      AWAIT_P_PHI0; // @posedge
      pip_reg_t* em = new pipe_register_t(_mc->_em);

      if (em->_memControl) {
         AWAIT_P_PHI1; // @negedge
         
         *_mc->_mw = *em;
         em->_memOp(_mc, em, _mc->_mw);

#ifdef MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Accessing memory at address %#x for ins %#x\n", SIM_TIME, em->_memory_addr_reg, em->_ins);
#endif
      } else {
         AWAIT_P_PHI1; // @negedge
         *_mc->_mw = *em;
#ifdef MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Memory has nothing to do for ins %#x\n", SIM_TIME, em->_ins);
#endif
      }
      delete em;
   }
}
