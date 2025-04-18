#include "executor.h"

Exe::Exe (Mipc *mc)
{
   _mc = mc;
}

Exe::~Exe (void) {}

void
Exe::MainLoop (void)
{
   /*
    * +ve half:
    *    - reads data from _mc->_de and performs the operation, result stored in local registers
    *    - updates _mc->_pc
    * -ve half:
    *    - stores results in _mc->_em
    */

   while (1) {
      AWAIT_P_PHI0; // @posedge
      pipe_reg_t *em = new pipe_reg_t(_mc->_de);

      if (!_mc->_isSyscall && !_mc->_de->_isIllegalOp) {
         _mc->_de->_opControl(_mc, _mc->_de->_ins, _mc->_de, em);
#ifdef MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Executed ins %#x\n", SIM_TIME, em->_ins);
#endif
         // if it is a branch instr., then _bdslot = 1
         if (em->_bdslot && em->_btaken) 
            _mc->_pc = em->_btgt;
         else if (!_stallFetch)
            _mc->_pc = _mc->_pc + 4;
      } else if (_mc->_de->_isIllegalOp) {
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Illegal ins %#x in execution stage at PC %#x\n", SIM_TIME, em->_ins, em->_pc);
#endif
      } else { 
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Deferring execution of syscall ins %#x\n", SIM_TIME, em->_ins);
#endif
      }

      AWAIT_P_PHI1; // @negedge
      *_mc->_em = *em;
      delete em;
   }
}
