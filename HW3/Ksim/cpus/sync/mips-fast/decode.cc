#include "decode.h"

Decode::Decode (Mipc *mc)
{
   _mc = mc;
}

Decode::~Decode (void) {}

void
Decode::MainLoop (void)
{
   unsigned int ins;
   while (1) {
      AWAIT_P_PHI1; // @negedge
      pipe_reg_t* fd = new pipe_reg_t(_mc->fd);

      if (fd->_valid) {
         AWAIT_P_PHI0; // @posedge
         _mc->Dec(fd->_ins, fd, _mc->_de);
#ifdef MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x\n", SIM_TIME, fd->_ins);
#endif
         _mc->_de->_valid = TRUE;
      } else {
         AWAIT_P_PHI0; // @posedge
         _mc->_de->_valid = FALSE;
      }

      delete fd;
   }
}
