#include "executor.h"

Exe::Exe (Mipc *mc)
{
   _mc = mc;
}

Exe::~Exe (void) {}

void
Exe::MainLoop (void)
{
   unsigned int ins;
   Bool isSyscall, isIllegalOp;

   while (1) {
      AWAIT_P_PHI1; // @negedge
      pipe_reg_t *de = new pipe_reg_t(_mc->de);
      if (de->_valid) {
         AWAIT_P_PHI0; // @posedge
         if (!de->isSyscall && !de->isIllegalOp) {
            _mc->opControl(_mc, de->_ins, de, _mc->_em);
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Executed ins %#x\n", SIM_TIME, de->_ins);
#endif
         } else if (de->isSyscall) {
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Deferring execution of syscall ins %#x\n", SIM_TIME, de->_ins);
#endif
         } else {
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Illegal ins %#x in execution stage at PC %#x\n", SIM_TIME, de->_ins, de->_pc);
#endif
         }
         _mc->_em->_valid = TRUE;

         /*
         if (!isIllegalOp && !isSyscall) {
            if (_mc->_lastbdslot && _mc->_btaken)
            {
               _mc->_pc = _mc->_btgt;
            }
            else
            {
               _mc->_pc = _mc->_pc + 4;
            }
            _mc->_lastbdslot = _mc->_bdslot;
         }
         */
      }
      else {
         AWAIT_P_PHI0; // @posedge
         _mc->_em->_valid = FALSE;
      }
   }
}
