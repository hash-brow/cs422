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

      pipe_reg_t* em = new pipe_reg_t;
      *em = *_mc->_em;

      Bool stall = FALSE;
      Bool isNewSyscall = FALSE;
      _mc->_stallFetch = FALSE;

#ifdef MIPC_DEBUG
      if (fd->_ins == 0) {
         fprintf(_mc->_debugLog, "<%llu> Decoding bubble ins\n", SIM_TIME);
      } else {
         fprintf(_mc->_debugLog, "<%llu> Decoding ins %#x\n", SIM_TIME, fd->_ins);
      }
#endif

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
                  stall |= (_mc->_hi_lo_wait[0] == 1);
               } else if (v == LO) {
                  stall |= (_mc->_hi_lo_wait[1] == 1);  
               } else {
                  stall |= (_mc->_gpr_wait[v] == 1);
               }
            }
         }

         if (de->_has_float_src){
            stall |= (_mc->_fpr_wait[de->_src_freg] > 0);
         }
      }
      
      if (stall) {
         _mc->_stallFetch = TRUE;
      }

      AWAIT_P_PHI1; // @negedge
      if (!stall) {
         // copy values to de register from mc register
         for(int i = 0; i < 32; i++){
            _mc->_de->_gpr[i] = _mc->_gpr[i];
         }
         // bypass from ex
         for (int i = 0; i < 2; i++) {
            unsigned int v = de->_src_reg[i];
            if (v) {
               if (v == HI) {
                  if(_mc->_hi_lo_wait[0] == 2){
                     _mc->_de->_hi = em->_opResultHi;
                     _mc->_hi_lo_wait[0] = 0;   
                  }
               } else if (v == LO) {
                  if(_mc->_hi_lo_wait[1] == 2){
                     _mc->_de->_lo = em->_opResultLo;
                     _mc->_hi_lo_wait[1] = 0;
                  }
               } else {
                  if(_mc->_gpr_wait[v] == 2){   
                     _mc->_de->_gpr[v] = em->_opResultLo;
                     _mc->_gpr_wait[v] = 0;
                  }
               }
            }
         }
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


     if (!stall) {
         // the proper decode 
        _mc->Dec(fd, _mc->_de);
         
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
      delete de;
      delete em;
#ifdef MIPC_DEBUG
      fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x\n", SIM_TIME, ins_copy);
#endif
   }
}
