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
    *      or an instr. with a data hazard; if so, stall/bypass
    * -ve half:
    *    - if no stall, decode the instruction completely. doing this in the -ve
    *      half allows us to read values written by WB in the +ve half.
    *    - update _mc->_de with the results
    */

   // TODO: this stuff is big endian right?
   
   struct bypass {
      Bool src[2]; // for gpr[_src_regs[i]], i = 0, 1
      Bool hi_lo[2]; // for _hi, _lo
      Bool subreg; // for _subregOperand
      Bool fpr; // for fprs
      Bool src_dst; // for _decodedDST
      Bool src_fdst; // for _decodedDST, but when the src is a fpr

      bypass() :
         subreg(FALSE),
         fpr(FALSE),
         src_dst(FALSE),
         src_fdst(FALSE)
      {
         src[0] = src[1] = hi_lo[0] = hi_lo[1] = FALSE;
      }

      Bool is_bypass() {
         return src[0] | src[1] | hi_lo[0] | hi_lo[1] | subreg | fpr;
      }
   };
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

      struct bypass ex_ex;
      struct bypass mem_ex;

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
                  int u = _mc->_hi_lo_wait[0];
                  if (u == 2) ex_ex.hi_lo[0] = TRUE;
                  else if (u == 1) mem_ex.hi_lo[0] = TRUE;
               } else if (v == LO) {
                  int u = _mc->_hi_lo_wait[1];
                  if (u == 2) ex_ex.hi_lo[1] = TRUE;
                  else if (u == 1) mem_ex.hi_lo[1] = TRUE;
               } else {
                  int u = _mc->_gpr_wait[v];
                  if (u == 2) ex_ex.src[i] = TRUE;
                  else if (u == 1) mem_ex.src[i] = TRUE;
               }
            }
         }

         if (de->_src_dst) {
            int v = _mc->_gpr_wait[de->_src_dst];
            if (v == 2) ex_ex.src_dst = TRUE;
            else if (v == 1) mem_ex.src_dst = TRUE;
         }

         if (de->_src_subreg && _mc->_gpr_wait[de->_src_subreg] > 0) {
            int v = _mc->_gpr_wait[de->_src_subreg];
            if (v == 2) ex_ex.subreg = TRUE;
            else if (v == 1) mem_ex.subreg = TRUE;
         }

         // 0 is a valid fpr, hence we require separate flags unlike gpr
         if (de->_has_float_src) {
            int v = _mc->_fpr_wait[de->_src_freg >> 1];
            if (v == 2) ex_ex.fpr = TRUE;
            else if (v == 1) mem_ex.fpr = TRUE;
         }

         if (de->_has_fdst) {
            int v = _mc->_fpr_wait[de->_src_fdst >> 1];
            if (v == 2) ex_ex.src_fdst = TRUE;
            else if (v == 1) mem_ex.src_fdst = TRUE;
         }
      }

      Bool mem_mem_bypass = FALSE;

      /*
       * no ex-ex bypass if there is a load in EX, and the current instruction is not a store
       */
      if (ex_ex.is_bypass() == TRUE && _mc->_de->_memControl && _mc->_de->_writeREG) {
         if (de->_memControl && ((!de->_writeREG && ex_ex.src_dst) || (!de->_writeFREG && ex_ex.src_fdst))) {
            // store with dst coming from load, we need to trigger the mem-mem bypass
            mem_mem_bypass = TRUE;
         } else stall = TRUE;
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

         // EX-EX bypass
         if (ex_ex.src[0]) {
            _mc->_de->_decodedSRC1 = _mc->_ex_ex.lo;
            // btgt = _decodedSRC1 
            if (_mc->_de->_btgt_bypass) _mc->_de->_btgt = _mc->_ex_ex.lo;
         }
         if (ex_ex.src[1]) _mc->_de->_decodedSRC2 = _mc->_ex_ex.lo;
         if (ex_ex.subreg) _mc->_de->_subregOperand = _mc->_ex_ex.lo;
         if (ex_ex.hi_lo[0]) _mc->_de->_decodedSRC1 = _mc->_ex_ex.hi;
         if (ex_ex.hi_lo[1]) _mc->_de->_decodedSRC1 = _mc->_ex_ex.lo;
         if (ex_ex.fpr) _mc->_de->_decodedSRC1 = _mc->_ex_ex.lo;
         if (ex_ex.src_dst) _mc->_de->_decodedDST = _mc->_ex_ex.lo;
         if (ex_ex.src_fdst) _mc->_de->_decodedDST = _mc->_ex_ex.lo;

         if (mem_mem_bypass) _mc->_de->_mem_mem_bypass = TRUE;

         // MEM-EX bypass
         if (mem_ex.src[0]) {
            _mc->_de->_decodedSRC1 = _mc->_mem_ex.lo;
            if (_mc->_de->_btgt_bypass) _mc->_de->_btgt = _mc->_mem_ex.lo;
         }
         if (mem_ex.src[1]) _mc->_de->_decodedSRC2 = _mc->_mem_ex.lo;
         if (mem_ex.subreg) _mc->_de->_subregOperand = _mc->_mem_ex.lo;
         if (mem_ex.hi_lo[0]) _mc->_de->_decodedSRC1 = _mc->_mem_ex.hi;
         if (mem_ex.hi_lo[1]) _mc->_de->_decodedSRC1 = _mc->_mem_ex.lo;
         if (mem_ex.fpr) _mc->_de->_decodedSRC1 = _mc->_mem_ex.lo;
         if (mem_ex.src_dst) _mc->_de->_decodedDST = _mc->_mem_ex.lo;
         if (mem_ex.src_fdst) _mc->_de->_decodedDST = _mc->_mem_ex.lo;
         
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
         _mc->_fetch_pc -= 4;
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

