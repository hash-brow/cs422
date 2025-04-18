#include "decode.h"

Decode::Decode (Mipc *mc)
{
   _mc = mc;
}

Decode::~Decode (void) {}

void
Decode::MainLoop (void)
{
   // TODO: ensure delete for all new
   unsigned int ins;
   while (1) {
      AWAIT_P_PHI0; // @posedge
      pipe_reg_t* fd = new pipe_reg_t(_mc->_fd);
      unsigned int ins_copy = fd->_ins;

      Bool gpr_stall[32];
      Bool fpr_stall[16];
      Bool hi_lo_stall[2];

      for (int i = 0; i < 32; i++) {
         gpr_stall[i] = _mc->_gpr_wait[i] > 0;
         if (_mc->_gpr_wait[i]) _mc->_gpr_wait[i]--;
      }

      for (int i = 0; i < 16; i++) {
         fpr_stall[i] = _mc->_fpr_wait[i] > 0;
         if (_mc->_fpr_wait[i]) _mc->_fpr_wait[i]--;
      }

      for (int i = 0; i < 2; i++) {
         hi_lo_stall[i] = _mc->_hi_lo_wait[i] > 0;
         if (_mc->_hi_lo_wait[i]) _mc->_hi_lo_wait[i]--;
      }

      if (!_mc->_isSyscall) {
         AWAIT_P_PHI1; // @negedge
         _mc->Dec(fd, _mc->_de);

         Bool stall = FALSE;
         _stallFetch = FALSE;

         if (_mc->_de->_isSyscall) {
            _isSyscall = TRUE;
            stall = TRUE;
         } else if (!_mc->_de->_isIllegalOp) {


            for (int i = 0; i < 2; i++) {
               unsigned int v = _mc->_de->_src_reg[i];
               if (v) {
                  if (v == HI) {
                     stall |= hi_lo_stall[0];
                  } else if (v == LO) {
                     stall |= hi_lo_stall[1];
                  } else {
                     stall |= gpr_stall[v];
                  }
               }
            }

            if (_mc->_de->_src_freg) stall |= fpr_stall[_mc->_de->_src_freg];

#define SET_MAX(dest, src) if (src > dest) dest = src

            if (!stall) {
               if (_mc->_de->_writeREG && _mc->_de->_decodedDST)
                  SET_MAX(_mc->_gpr_wait[_mc->_de->_decodedDST], _mc->_de->_dstall);
               else if (_mc->_de->_hiWPort)
                  SET_MAX(_mc->_hi_lo_wait[0], _mc->_de->_dstall);
               else if (_mc->_de->loWPort)
                  SET_MAX(_mc->_hi_lo_wait[1], _mc->_de->_dstall);
               else if (_mc->_de->_writeFREG)
                  // as fpr are paired
                  SET_MAX(_mc->_fpr_wait[_mc->_de->decodedDST / 2], _mc->_de->_dstall);
            } else {
               _stallFetch = TRUE;

               // set _ins to 0 and decode again, to essentially get a NOP
               fd->_ins = 0;
               _mc->Dec(fd, _mc->_de);
            }
         } else {
            fd->_ins = 0;
            _mc->Dec(fd, _mc->_de);
         }
#ifdef MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x\n", SIM_TIME, ins_copy);
#endif
      } else {
         PAUSE(1);
      }

      delete fd;
   }
}
