#include "wb.h"

Writeback::Writeback (Mipc *mc)
{
   _mc = mc;
}

Writeback::~Writeback (void) {}

void
Writeback::MainLoop (void)
{
   /*
    * +ve half:
    *    - samples _mc->_mw into local registers
    *    - updates registers so that they can be decoded in the
    *      -ve half of DE
    * -ve half:
    *    - executes system calls
    */ 

   while (1) {
      AWAIT_P_PHI0; // @posedge

      if (!_mc->_mw->_valid) {
        AWAIT_P_PHI1;
        continue;
      }

      pipe_reg_t* mw = new pipe_reg_t;
      *mw = *_mc->_mw;
      
      if (mw->_isIllegalOp) {
         printf("Illegal ins %#x at PC %#x. Terminating simulation!\n", mw->_ins, mw->_pc);
#ifdef MIPC_DEBUG
         fclose(_mc->_debugLog);
#endif
         printf("Register state on termination:\n\n");
         _mc->dumpregs();
         exit(0);
      }

      if (!mw->_isSyscall) {
         if (mw->_writeREG) {
            _mc->_gpr[mw->_decodedDST] = mw->_opResultLo;
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Writing to reg %u, value: %#x\n", SIM_TIME, mw->_decodedDST, mw->_opResultLo);
#endif
         } else if (mw->_writeFREG) {
            _mc->_fpr[(mw->_decodedDST)>>1].l[FP_TWIDDLE^((mw->_decodedDST)&1)] = mw->_opResultLo;
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Writing to freg %u, value: %#x\n", SIM_TIME, (mw->_decodedDST)>>1, mw->_opResultLo);
#endif
         } else {
            if (mw->_hiWPort) {
               _mc->_hi = mw->_opResultHi;
#ifdef MIPC_DEBUG
               fprintf(_mc->_debugLog, "<%llu> Writing to Hi, value: %#x\n", SIM_TIME, mw->_opResultHi);
#endif
            } 
            if (mw->_loWPort) {
               _mc->_lo = mw->_opResultLo;
#ifdef MIPC_DEBUG
               fprintf(_mc->_debugLog, "<%llu> Writing to Lo, value: %#x\n", SIM_TIME, mw->_opResultLo);
#endif
            }
         }

         // $0 is always 0
         _mc->_gpr[0] = 0;

         AWAIT_P_PHI1; // @negedge
      } else {
         AWAIT_P_PHI1; // @negedge
         // TODO: can system calls clobber registers??
         if (mw->_isSyscall) {
            // TODO: can the branch delay slot have syscalls?
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> SYSCALL! Trapping to emulation layer at PC %#x\n", SIM_TIME, mw->_pc);
#endif      
            mw->_opControl(_mc, mw->_ins, NULL, NULL);
            _mc->_fetch_pc += 4;

            _mc->_isSyscall = FALSE;
         }
      }
      delete mw;
   }
}
