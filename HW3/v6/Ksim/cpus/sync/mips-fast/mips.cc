#include "mips.h"
#include <assert.h>
#include "mips-irix5.h"

Mipc::Mipc (Mem *m) : _l('M')
{
   _mem = m;
   _sys = new MipcSysCall (this);	// Allocate syscall layer
   _fd = new pipe_reg_t;
   _de = new pipe_reg_t;
   _em = new pipe_reg_t;
   _mw = new pipe_reg_t;

   for (int i = 0; i < 32; ++i) _gpr_wait[i] = 0;
   for (int i = 0; i < 16; ++i) _fpr_wait[i] = 0;
   for (int i = 0; i < 2; ++i) _hi_lo_wait[i] = 0;


#ifdef MIPC_DEBUG
   _debugLog = fopen("mipc.debug", "w");
   assert(_debugLog != NULL);
#endif
   
   Reboot (ParamGetString ("Mipc.BootROM"));
}

Mipc::~Mipc (void)
{

}

void 
Mipc::MainLoop (void)
{
   LL addr;
   unsigned int ins;	// Local instruction register

   Assert (_boot, "Mipc::MainLoop() called without boot?");

   _nfetched = 0;
   _stallFetch = FALSE;

   while (!_sim_exit) {
      /*
       * Imagine in EX we calculate the branch target and update _pc
       * accordingly in the positive half. 
       * We must hence do everything in the negative half
       */

      AWAIT_P_PHI0; // @posedge
      AWAIT_P_PHI1; // @negedge
      if (!_stallFetch) {
         _fd->_ins = _mem->BEGetWord (_fetch_pc, _mem->Read(_fetch_pc & ~(LL)0x7));
         _fd->_pc = _fetch_pc;
#ifdef MIPC_DEBUG
        fprintf(_debugLog, "<%llu> Fetched ins %#x from PC %#x\n", SIM_TIME, _fd->_ins, _fetch_pc);
#endif
        _nfetched++;
      }
   }

   MipcDumpstats();
   Log::CloseLog();
   
#ifdef MIPC_DEBUG
   assert(_debugLog != NULL);
   fclose(_debugLog);
#endif

   exit(0);
}

void
Mipc::MipcDumpstats()
{
  Log l('*');
  l.startLogging = 0;

  l.print ("");
  l.print ("************************************************************");
  l.print ("");
  l.print ("Number of instructions: %llu", _nfetched);
  l.print ("Number of simulated cycles: %llu", SIM_TIME);
  l.print ("CPI: %.2f", ((double)SIM_TIME)/_nfetched);
  l.print ("Int Conditional Branches: %llu", _num_cond_br);
  l.print ("Jump and Link: %llu", _num_jal);
  l.print ("Jump Register: %llu", _num_jr);
  l.print ("Number of fp instructions: %llu", _fpinst);
  l.print ("Number of loads: %llu", _num_load);
  l.print ("Number of syscall emulated loads: %llu", _sys->_num_load);
  l.print ("Number of stores: %llu", _num_store);
  l.print ("Number of syscall emulated stores: %llu", _sys->_num_store);
  l.print ("");

  l.print("Additional counters");
  l.print("Number of ex-ex bypasses: %llu", _num_ex_ex);
  l.print("Number of mem-ex bypasses: %llu", _num_mem_ex);
  l.print("Number of mem-mem bypasses: %llu", _num_mem_mem);
  l.print("Number of syscalls: %llu", _num_syscall);
  l.print("Number of stalls due to load interlock: %llu", _num_stalls);
}

void 
Mipc::fake_syscall (unsigned int ins)
{
   _sys->pc = _fetch_pc;
   _sys->quit = 0;
   _sys->EmulateSysCall ();
   if (_sys->quit)
      _sim_exit = 1;
}

/*------------------------------------------------------------------------
 *
 *  Mipc::Reboot --
 *
 *   Reset processor state
 *
 *------------------------------------------------------------------------
 */
void 
Mipc::Reboot (char *image)
{
   FILE *fp;
   Log l('*');

   _boot = 0;

   if (image) {
      _boot = 1;
      printf ("Executing %s\n", image);
      fp = fopen (image, "r");
      if (!fp) {
	 fatal_error ("Could not open `%s' for booting host!", image);
      }
      _mem->ReadImage(fp);
      fclose (fp);

      _num_load = 0;
      _num_store = 0;
      _fpinst = 0;
      _num_cond_br = 0;
      _num_jal = 0;
      _num_jr = 0;

      _num_ex_ex = 0;
      _num_mem_ex = 0;
      _num_mem_mem = 0;
      _num_syscall = 0;
      _num_stalls = 0;

      _sim_exit = 0;
      _fetch_pc = ParamGetInt ("Mipc.BootPC");	// Boom! GO
   }
}

LL
MipcSysCall::GetDWord(LL addr)
{
   _num_load++;      
   return m->Read (addr);
}

void
MipcSysCall::SetDWord(LL addr, LL data)
{
  
   m->Write (addr, data);
   _num_store++;
}

Word 
MipcSysCall::GetWord (LL addr) 
{ 
  
   _num_load++;   
   return m->BEGetWord (addr, m->Read (addr & ~(LL)0x7)); 
}

void 
MipcSysCall::SetWord (LL addr, Word data) 
{ 
  
   m->Write (addr & ~(LL)0x7, m->BESetWord (addr, m->Read(addr & ~(LL)0x7), data)); 
   _num_store++;
}
  
void 
MipcSysCall::SetReg (int reg, LL val) 
{ 
   _ms->_gpr[reg] = val; 
}

LL 
MipcSysCall::GetReg (int reg) 
{
   return _ms->_gpr[reg]; 
}

LL
MipcSysCall::GetTime (void)
{
  return SIM_TIME;
}
