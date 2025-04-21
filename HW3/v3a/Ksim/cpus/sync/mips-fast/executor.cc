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

      if (!_mc->_de->_valid) {
        AWAIT_P_PHI1;
        continue;
      }

      pipe_reg_t *em = new pipe_reg_t;
      *em = *_mc->_de;

      if (em->_isIllegalOp) {
         // if we have an illegal op, propagate that msg and move on
         AWAIT_P_PHI1;
         _mc->_em->_isIllegalOp = TRUE;
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Illegal ins %#x in execution stage at PC %#x\n", SIM_TIME, em->_ins, em->_pc);
#endif
         continue;
      }

      if (!_mc->_de->_isSyscall) {
         _mc->_de->_opControl(_mc, _mc->_de->_ins, _mc->_de, em);
         _mc->_ex_ex_bypass_lo = em->_opResultLo;
         _mc->_ex_ex_bypass_hi = em->_opResultHi;
#ifdef MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Executed ins %#x\n", SIM_TIME, em->_ins);
#endif
         // if it is a branch instr., then _bdslot = 1
         if (em->_bdslot && em->_btaken) 
            _mc->_fetch_pc = em->_btgt;
         else if (!em->_is_bubble) {
            _mc->_fetch_pc += 4;
         }
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
