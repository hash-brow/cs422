#ifndef __MIPS_H__
#define __MIPS_H__

#include "sim.h"

class Mipc;
class MipcSysCall;
class SysCall;

typedef unsigned Bool;
#define TRUE 1
#define FALSE 0

#if BYTE_ORDER == LITTLE_ENDIAN

#define FP_TWIDDLE 0

#else

#define FP_TWIDDLE 1

#endif

#include "mem.h"
#include "../../common/syscall.h"
#include "queue.h"

//#define MIPC_DEBUG 1

typedef struct _pipe_reg {
   _pipe_reg() :
      _isIllegalOp(FALSE),
      _ins(0),
      _isSyscall(FALSE),
      _dstall(0),
      _has_float_src(0),
      _is_bubble(FALSE),
      _btgt(0), 
      _bdslot(0),
      _btaken(0),
      _pc(0),
      _decodedSRC1(0),
      _decodedSRC2(0),
      _decodedDST(0),
      _subregOperand(0),
      _memory_addr_reg(0),
      _opResultHi(0),
      _opResultLo(0),
      _memControl(0),
      _writeREG(0),
      _writeFREG(0),
      _branchOffset(0),
      _hiWPort(0),
      _loWPort(0),
      _decodedShiftAmt(0),
      _lastbdslot(0),
      _valid(FALSE),
      _btgt_bypass(FALSE)
   {
      _src_reg[0] = 0;
      _src_reg[1] = 0;
      _src_subreg = 0; 
      _src_freg = 0;
   }

   // all zeros = NOP
   unsigned int _ins;

   Bool _valid;

   // no. of cycles before which the dest. 
   // register will be updated
   unsigned int _dstall;

// magic numbers for _hi, _lo registers
// make sure in any if/else using _src_reg, you check for HI, LO first so
// as to avoid out-of-bounds checks
#define HI 333
#define LO 1729
   unsigned int _src_reg[2];
   unsigned int _src_subreg;
   unsigned int _src_freg;
   unsigned int _has_float_src;

   Bool _is_bubble;

   Bool _btgt_bypass;

   signed int	_decodedSRC1, _decodedSRC2;	// Reg fetch output (source values)
   unsigned	_decodedDST;			// Decoder output (dest reg no)
   unsigned 	_subregOperand;			// Needed for lwl and lwr
   unsigned	_memory_addr_reg;				// Memory address register
   unsigned	_opResultHi, _opResultLo;	// Result of operation
   Bool 	_memControl;			// Memory instruction?
   Bool		_writeREG, _writeFREG;		// WB control
   signed int	_branchOffset;
   Bool 	_hiWPort, _loWPort;		// WB control
   unsigned	_decodedShiftAmt;		// Shift amount

   unsigned int	_pc;				// Program counter
   unsigned int _lastbdslot;			// branch delay state

   int 		_btaken; 			// taken branch (1 if taken, 0 if fall-through)
   int 		_bdslot;				// 1 if the next ins is delay slot
   unsigned int	_btgt;				// branch target

   Bool		_isSyscall;			// 1 if system call
   Bool		_isIllegalOp;			// 1 if illegal opcode 

   void (*_opControl)(Mipc*, unsigned, struct _pipe_reg*, struct _pipe_reg*);
   void (*_memOp)(Mipc*, struct _pipe_reg*, struct _pipe_reg*);
} pipe_reg_t;


class Mipc : public SimObject {
public:
   Mipc (Mem *m);
   ~Mipc ();
  
   FAKE_SIM_TEMPLATE;

   MipcSysCall *_sys;		// Emulated system call layer

   void dumpregs (void);	// Dumps current register state

   void Reboot (char *image = NULL);
				// Restart processor.
				// "image" = file name for new memory
				// image if any.

   void MipcDumpstats();			// Prints simulation statistics
   void Dec (pipe_reg_t*, pipe_reg_t*);			// Decoder function
   void fake_syscall (unsigned int ins);	// System call interface

   /* processor state */

   // fetch/decode, decode/execute, execute/memory, memory/write back
   pipe_reg_t *_fd, *_de, *_em, *_mw;
   Bool		_isSyscall;			// 1 if system call

   unsigned int _hi, _lo; 			// mult, div destination
   unsigned int _hi_lo_wait[2];

   unsigned int 	_gpr[32];		// general-purpose integer registers

   /*
    * If you are decoding an instr. whose source operand $x has
    * _gprWait[x] > 0, stall
    */
   unsigned int _gpr_wait[32]; 

   // bypasses for _opResultLo, _opResultHi
   unsigned _ex_ex_bypass_lo;
   unsigned _ex_ex_bypass_hi;
   unsigned _ex_ex_bypass_subreg;

   union {
      unsigned int l[2];
      float f[2];
      double d;
   } _fpr[16];					// floating-point registers (paired)
   unsigned int _fpr_wait[16];


   // Program counter from which instructions are fetched
   // Is updated by EX during the +ve half
   unsigned int _fetch_pc;		

   Bool _stallFetch;
                                 
   unsigned int _boot;				// boot code loaded?

   // Simulation statistics counters

   LL	_nfetched;
   LL	_num_cond_br;
   LL	_num_jal;
   LL	_num_jr;
   LL   _num_load;
   LL   _num_store;
   LL   _fpinst;

   Mem	*_mem;	// attached memory (not a cache)

   Log	_l;
   int  _sim_exit;		// 1 on normal termination

   FILE *_debugLog;

   // EXE stage definitions

   static void func_add_addu (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_and (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_nor (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_or (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_sll (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_sllv (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_slt (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_sltu (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_sra (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_srav (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_srl (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_srlv (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_sub_subu (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_xor (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_div (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_divu (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_mfhi (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_mflo (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_mthi (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_mtlo (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_mult (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_multu (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_jalr (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_jr (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_await_break (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_syscall (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_addi_addiu (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_andi (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_lui (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_ori (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_slti (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_sltiu (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_xori (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_beq (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_bgez (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_bgezal (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_bltzal (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_bltz (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_bgtz (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_blez (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_bne (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_j (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_jal (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_lb (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_lbu (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_lh (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_lhu (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_lwl (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_lw (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_lwr (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_lwc1 (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_swc1 (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_sb (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_sh (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_swl (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_sw (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_swr (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_mtc1 (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);
   static void func_mfc1 (Mipc*, unsigned, pipe_reg_t*, pipe_reg_t*);

   // MEM stage definitions

   static void mem_lb (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_lbu (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_lh (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_lhu (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_lwl (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_lw (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_lwr (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_lwc1 (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_swc1 (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_sb (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_sh (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_swl (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_sw (Mipc*, pipe_reg_t*, pipe_reg_t*);
   static void mem_swr (Mipc*, pipe_reg_t*, pipe_reg_t*);
};


// Emulated system call interface

class MipcSysCall : public SysCall {
public:

   MipcSysCall (Mipc *ms) {

      char buf[1024];
      m = ms->_mem;
      _ms = ms;
      _num_load = 0;
      _num_store = 0;
   };

   ~MipcSysCall () { };

   LL GetDWord (LL addr);
   void SetDWord (LL addr, LL data);

   Word GetWord (LL addr);
   void SetWord (LL addr, Word data);
  
   void SetReg (int reg, LL val);
   LL GetReg (int reg);
   LL GetTime (void);

private:

   Mipc *_ms;
};
#endif /* __MIPS_H__ */

