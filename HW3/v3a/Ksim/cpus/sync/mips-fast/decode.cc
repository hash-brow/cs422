#include "decode.h"

Decode::Decode (Mipc *mc)
{
   _mc = mc;
}

Decode::~Decode (void) {}

void
Decode::MainLoop (void)
{
   /*
    * +ve half:
    *    - make a local copy of _mc->_fd
    *    - do a "partial" decode to check if we are decoding a sys call
    *      or an instr. with a data hazard; if so, stall
    * -ve half:
    *    - if no stall, decode the instruction completely. doing this in the -ve
    *      half allows us to read values written by WB in the +ve half.
    *    - update _mc->_de with the results
    */

   // TODO: ensure delete for all new
   // TODO: this stuff is big endian right?
   while (1) {
      AWAIT_P_PHI0; // @posedge
      pipe_reg_t* fd = new pipe_reg_t;
      *fd = *_mc->_fd;
      unsigned int ins_copy = fd->_ins;

      pipe_reg_t* de = new pipe_reg_t;
      _mc->Dec(fd, de); // "partial" decode

      Bool stall = FALSE;
      Bool isNewSyscall = FALSE;
      _mc->_stallFetch = FALSE;

      // for _src_regs[i], i = 0, 1
      Bool ex_ex_bypass[2];
      ex_ex_bypass[0] = FALSE;
      ex_ex_bypass[1] = FALSE;

      Bool ex_ex_bypass_hi_lo[2];
      ex_ex_bypass_hi_lo[0] = FALSE;
      ex_ex_bypass_hi_lo[1] = FALSE;
      
      Bool ex_ex_bypass_subreg = FALSE;

      if (_mc->_isSyscall == TRUE) {
         // if there's an existing syscall in the pipeline, stall
         stall = TRUE;
      } else if (de->_isIllegalOp) {
         // current instr. is illegal, update _mc->_de and move on
         AWAIT_P_PHI1;
         _mc->_de->_isIllegalOp = TRUE;
         continue;
      } else if (de->_isSyscall) {
         // the current instr. is a syscall
         stall = TRUE;
         isNewSyscall = TRUE;
         _mc->_isSyscall = TRUE;
      } else {
         for (int i = 0; i < 2; i++) {
            unsigned int v = de->_src_reg[i];
            if (v) {
               if (v == HI) {
                  if (_mc->_hi_lo_wait[0] == 2) {
                     ex_ex_bypass_hi_lo[0] = TRUE;
                  } else if (_mc->_hi_lo_wait[0] > 0) stall = TRUE;
               } else if (v == LO) {
                  if (_mc->_hi_lo_wait[1] == 2) {
                     ex_ex_bypass_hi_lo[1] = TRUE;
                  } else if (_mc->_hi_lo_wait[1] > 0) stall = TRUE;
               } else {
                  if (_mc->_gpr_wait[v] > 0) {
                     if (_mc->_gpr_wait[v] == 2) {
                        // ex-ex bypass, do not bypass stores, do not bypass when there is a load in EX
                        ex_ex_bypass[i] = TRUE;
                     } else stall |= TRUE;
                  }
               }
            }
         }

         if (de->_src_subreg && _mc->_gpr_wait[de->_src_subreg] > 0) {
            if (_mc->_gpr_wait[de->_src_subreg] == 2) {
               ex_ex_bypass_subreg = TRUE;
            } else stall |= TRUE;
         }

         if (de->_has_float_src)
            stall |= (_mc->_fpr_wait[de->_src_freg] > 0);
      }

      /*
       * no ex-ex bypass if:
       * (1) current instruction is a store
       * (2) when there is a load in EX
       */
      if ((ex_ex_bypass_subreg | ex_ex_bypass[0] | ex_ex_bypass[1] |
               ex_ex_bypass_hi_lo[0] | ex_ex_bypass_hi_lo[1]) == TRUE) {
         // store
         if (de->_memControl && !de->_writeREG) stall = TRUE;
         // load in EX
         else if (_mc->_de->_memControl && _mc->_de->_writeREG) stall = TRUE;
      }

      for (int i = 0; i < 32; i++) {
         if (_mc->_gpr_wait[i]) _mc->_gpr_wait[i]--;
      }

      for (int i = 0; i < 16; i++) {
         if (_mc->_fpr_wait[i]) _mc->_fpr_wait[i]--;
      }

      for (int i = 0; i < 2; i++) {
         if (_mc->_hi_lo_wait[i]) _mc->_hi_lo_wait[i]--;
      }

      if (stall) {
         _mc->_stallFetch = TRUE;
      }

      delete de;

      AWAIT_P_PHI1; // @negedge
      if (!stall) {
         // the proper decode
         _mc->Dec(fd, _mc->_de);

         if (ex_ex_bypass[0]) {
            _mc->_de->_decodedSRC1 = _mc->_ex_ex_bypass_lo;
            // btgt = _decodedSRC1 
            if (_mc->_de->_btgt_bypass) _mc->_de->_btgt = _mc->_ex_ex_bypass_lo;
         }
         if (ex_ex_bypass[1]) _mc->_de->_decodedSRC2 = _mc->_ex_ex_bypass_lo;
         if (ex_ex_bypass_subreg) _mc->_de->_subregOperand = _mc->_ex_ex_bypass_lo;
         if (ex_ex_bypass_hi_lo[0]) _mc->_de->_decodedSRC1 = _mc->_ex_ex_bypass_hi;
         if (ex_ex_bypass_hi_lo[1]) _mc->_de->_decodedSRC1 = _mc->_ex_ex_bypass_lo;
         
#define SET_MAX(dest, src) if (src > dest) dest = src
         if (_mc->_de->_writeREG && _mc->_de->_decodedDST) { 
            SET_MAX(_mc->_gpr_wait[_mc->_de->_decodedDST], _mc->_de->_dstall);
         }
         else if (_mc->_de->_hiWPort || _mc->_de->_loWPort) {
            if (_mc->_de->_hiWPort) 
               SET_MAX(_mc->_hi_lo_wait[0], _mc->_de->_dstall);
            if (_mc->_de->_loWPort)
               SET_MAX(_mc->_hi_lo_wait[1], _mc->_de->_dstall);
         }
         else if (_mc->_de->_writeFREG)
            SET_MAX(_mc->_fpr_wait[_mc->_de->_decodedDST >> 1], _mc->_de->_dstall);
      } else if (isNewSyscall) {
         _mc->Dec(fd, _mc->_de);
         fd->_ins = 0;
         _mc->_de->_is_bubble = TRUE;

        _mc->_fd->_ins = 0;
      } else if (_mc->_isSyscall) {
         fd->_ins = 0;
         _mc->Dec(fd, _mc->_de);
         _mc->_de->_is_bubble = TRUE;

        _mc->_fd->_ins = 0;
      } else {
         fd->_ins = 0;
         _mc->Dec(fd, _mc->_de);
         _mc->_de->_is_bubble = TRUE;
      }

      delete fd;
#ifdef MIPC_DEBUG
      fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x\n", SIM_TIME, ins_copy);
#endif
   }
}
