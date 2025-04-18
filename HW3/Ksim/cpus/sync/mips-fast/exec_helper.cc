#include <math.h>
#include "mips.h"
#include "opcodes.h"
#include <assert.h>
#include "app_syscall.h"

#define MAGIC_EXEC 0xdeadbeef

/*------------------------------------------------------------------------
 *
 *  Instruction exec 
 *
 *------------------------------------------------------------------------
 */
void
Mipc::Dec (pipe_reg_t* fd, pipe_reg_t* de)
{
   MipsInsn i;
   signed int a1, a2;
   unsigned int ar1, ar2, s1, s2, r1, r2, t1, t2;
   LL addr;
   unsigned int val;
   LL value, mask;
   int sa,j;
   Word dummy;

   de->_isIllegalOp = FALSE;
   de->_ins = 0;
   de->_isSyscall = FALSE;
   de->_dstall = 0;
   de->_has_float_src = 0;
   de->_is_bubble = FALSE;

   /*
    * Data stall is 2 cycles.
    * Reg. will be updated in the +ve half of WB,
    * and decode will read it in the -ve half.
    */
   // TODO: ensure that fd propagates to de
   de->_dstall = 2;
   de->_src_reg[0] = 0;
   de->_src_reg[1] = 0;
   de->_src_freg = 0;
   de->_has_float_src = 0;
   de->_ins = fd->_ins;

   i.data = fd->_ins;
  
#define SIGN_EXTEND_BYTE(x)  do { x <<= 24; x >>= 24; } while (0)
#define SIGN_EXTEND_IMM(x)   do { x <<= 16; x >>= 16; } while (0)

   switch (i.reg.op) {
   case 0:
      // SPECIAL (ALU format)
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = _gpr[i.reg.rt];
      de->_decodedDST = i.reg.rd;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;

      // See definition of MipsInsn
      // Appropriately changed within the switch cases for the minority instructions
      // https://www.kth.se/social/files/563c63c9f276547044e8695f/mips-ref-sheet.pdf
      de->_src_reg[0] = i.reg.rt;
      de->_src_reg[1] = i.reg.rs;

      switch (i.reg.func) {
      case 0x20:			// add
      case 0x21:			// addu
         de->_opControl = func_add_addu;
	 break;

      case 0x24:			// and
         de->_opControl = func_and;
	 break;

      case 0x27:			// nor
         de->_opControl = func_nor;
	 break;

      case 0x25:			// or
         de->_opControl = func_or;
	 break;

      case 0:			// sll
         de->_opControl = func_sll;
         de->_decodedShiftAmt = i.reg.sa;
         de->_src_reg[1] = 0;
	 break;

      case 4:			// sllv
         de->_opControl = func_sllv;
	 break;

      case 0x2a:			// slt
         de->_opControl = func_slt;
	 break;

      case 0x2b:			// sltu
         de->_opControl = func_sltu;
	 break;

      case 0x3:			// sra
         de->_opControl = func_sra;
         de->_decodedShiftAmt = i.reg.sa;
         de->_src_reg[1] = 0;
	 break;

      case 0x7:			// srav
         de->_opControl = func_srav;
	 break;

      case 0x2:			// srl
         de->_opControl = func_srl;
         de->_decodedShiftAmt = i.reg.sa;
         de->_src_reg[1] = 0;
	 break;

      case 0x6:			// srlv
         de->_opControl = func_srlv;
	 break;

      case 0x22:			// sub
      case 0x23:			// subu
	 // no overflow check
         de->_opControl = func_sub_subu;
	 break;

      case 0x26:			// xor
         de->_opControl = func_xor;
	 break;

      case 0x1a:			// div
         de->_opControl = func_div;
         de->_hiWPort = TRUE;
         de->_loWPort = TRUE;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
	 break;

      case 0x1b:			// divu
         de->_opControl = func_divu;
         de->_hiWPort = TRUE;
         de->_loWPort = TRUE;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
	 break;

      case 0x10:			// mfhi
         de->_opControl = func_mfhi;
         de->_src_reg[0] = HI;
         de->_src_reg[1] = 0;
	 break;

      case 0x12:			// mflo
         de->_opControl = func_mflo;
         de->_src_reg[0] = LO;
	 break;

      case 0x11:			// mthi
         de->_opControl = func_mthi;
         de->_hiWPort = TRUE;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
         de->_src_reg[0] = i.reg.rs;
         de->_src_reg[1] = 0;
	 break;

      case 0x13:			// mtlo
         de->_opControl = func_mtlo;
         de->_loWPort = TRUE;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
         de->_src_reg[0] = i.reg.rs;
         de->_src_reg[1] = 0;
	 break;

      case 0x18:			// mult
         de->_opControl = func_mult;
         de->_hiWPort = TRUE;
         de->_loWPort = TRUE;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
	 break;

      case 0x19:			// multu
         de->_opControl = func_multu;
         de->_hiWPort = TRUE;
         de->_loWPort = TRUE;
         de->_writeREG = FALSE;
          de->_writeFREG = FALSE;
	 break;

      case 9:			// jalr
         de->_opControl = func_jalr;
         de->_btgt = de->_decodedSRC1;
         _de->_bdslot = 1;
         break;

      case 8:			// jr
         de->_opControl = func_jr;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
         de->_btgt = de->_decodedSRC1;
         _de->_bdslot = 1;
	 break;

      case 0xd:			// await/break
         de->_opControl = func_await_break;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
	 break;

      case 0xc:			// syscall
         de->_opControl = func_syscall;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
         de->_isSyscall = TRUE;
         de->_dstall = 3;
	 break;

      default:
	 de->_isIllegalOp = TRUE;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
	 break;
      }
      break;	// ALU format

   case 8:			// addi
   case 9:			// addiu
      // ignore overflow: no exceptions
      de->_opControl = func_addi_addiu;
      de->_decodedSRC1 = _gpr[i.imm.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
       de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_src_reg[0] = i.imm.rs;
      break;

   case 0xc:			// andi
      de->_opControl = func_andi;
      de->_decodedSRC1 = _gpr[i.imm.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_src_reg[0] = i.imm.rs;
      break;

   case 0xf:			// lui
      de->_opControl = func_lui;
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      break;

   case 0xd:			// ori
      de->_opControl = func_ori;
      de->_decodedSRC1 = _gpr[i.imm.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_src_reg[0] = i.imm.rs;
      break;

   case 0xa:			// slti
      de->_opControl = func_slti;
      de->_decodedSRC1 = _gpr[i.imm.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_src_reg[0] = i.imm.rs;
      break;

   case 0xb:			// sltiu
      de->_opControl = func_sltiu;
      de->_decodedSRC1 = _gpr[i.imm.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_src_reg[0] = i.imm.rs;
      break;

   case 0xe:			// xori
      de->_opControl = func_xori;
      de->_decodedSRC1 = _gpr[i.imm.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_src_reg[0] = i.imm.rs;
      break;

   case 4:			// beq
      de->_opControl = func_beq;
      de->_decodedSRC1 = _gpr[i.imm.rs];
      de->_decodedSRC2 = _gpr[i.imm.rt];
      de->_branchOffset = i.imm.imm;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_branchOffset <<= 16; de->_branchOffset >>= 14; _de->_bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
      de->_src_reg[0] = i.imm.rs;
      de->_src_reg[1] = i.imm.rt;
      break;

   case 1:
      // REGIMM
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_branchOffset = i.imm.imm;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;

      switch (i.reg.rt) {
      case 1:			// bgez
         de->_opControl = func_bgez;
         de->_branchOffset <<= 16; de->_branchOffset >>= 14; _de->_bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
         de->_src_reg[0] = i.imm.rs;
	 break;

      case 0x11:			// bgezal
         de->_opControl = func_bgezal;
         de->_decodedDST = 31;
         de->_writeREG = TRUE;
         de->_branchOffset <<= 16; de->_branchOffset >>= 14; _de->_bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
         de->_src_reg[0] = i.imm.rs;
	 break;

      case 0x10:			// bltzal
         de->_opControl = func_bltzal;
         de->_decodedDST = 31;
         de->_writeREG = TRUE;
         de->_branchOffset <<= 16; de->_branchOffset >>= 14; _de->_bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
         de->_src_reg[0] = i.imm.rs;
	 break;

      case 0x0:			// bltz
         de->_opControl = func_bltz;
         de->_branchOffset <<= 16; de->_branchOffset >>= 14; _de->_bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
         de->_src_reg[0] = i.imm.rs;
	 break;

      default:
	 de->_isIllegalOp = TRUE;
	 break;
      }
      break;

   case 7:			// bgtz
      de->_opControl = func_bgtz;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_branchOffset = i.imm.imm;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_branchOffset <<= 16; de->_branchOffset >>= 14; _de->_bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
      de->_src_reg[0] = i.imm.rs;
      break;

      case 6:			// blez
      de->_opControl = func_blez;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_branchOffset = i.imm.imm;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_branchOffset <<= 16; de->_branchOffset >>= 14; _de->_bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
      de->_src_reg[0] = i.imm.rs;
      break;

   case 5:			// bne
      de->_opControl = func_bne;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = _gpr[i.reg.rt];
      de->_branchOffset = i.imm.imm;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_branchOffset <<= 16; de->_branchOffset >>= 14; _de->_bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
      de->_src_reg[0] = i.imm.rs;
      de->_src_reg[1] = i.imm.rt;
      break;

   case 2:			// j
      de->_opControl = func_j;
      de->_branchOffset = i.tgt.tgt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_btgt = ((_pc+4) & 0xf0000000) | (de->_branchOffset<<2); _de->_bdslot = 1;
      break;

   case 3:			// jal
      de->_opControl = func_jal;
      de->_branchOffset = i.tgt.tgt;
      de->_decodedDST = 31;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_btgt = ((_pc+4) & 0xf0000000) | (de->_branchOffset<<2); _de->_bdslot = 1;
      break;

   case 0x20:			// lb  
      de->_opControl = func_lb;
      de->_memOp = mem_lb;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.reg.rs;
      break;

   case 0x24:			// lbu
      de->_opControl = func_lbu;
      de->_memOp = mem_lbu;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.reg.rs;
      break;

   case 0x21:			// lh
      de->_opControl = func_lh;
      de->_memOp = mem_lh;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.reg.rs;
      break;

   case 0x25:			// lhu
      de->_opControl = func_lhu;
      de->_memOp = mem_lhu;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.reg.rs;
      break;

   case 0x22:			// lwl
      de->_opControl = func_lwl;
      de->_memOp = mem_lwl;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_subregOperand = _gpr[i.reg.rt];
      de->_decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.imm.rt;
      de->_src_reg[1] = i.imm.rs;
      break;

   case 0x23:			// lw
      de->_opControl = func_lw;
      de->_memOp = mem_lw;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.imm.rs;
      break;

   case 0x26:			// lwr
      de->_opControl = func_lwr;
      de->_memOp = mem_lwr;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_subregOperand = _gpr[i.reg.rt];
      de->_decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.imm.rt;
      de->_src_reg[1] = i.imm.rs;
      break;

   case 0x31:			// lwc1
      de->_opControl = func_lwc1;
      de->_memOp = mem_lwc1;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = TRUE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.reg.rs;
      break;

   case 0x39:			// swc1
      de->_opControl = func_swc1;
      de->_memOp = mem_swc1;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_freg = i.freg.ft >> 1;
      de->_has_float_src = 1;
      de->_src_reg[0] = i.imm.rs;
      break;

   case 0x28:			// sb
      de->_opControl = func_sb;
      de->_memOp = mem_sb;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.imm.rt;
      de->_src_reg[1] = i.imm.rs;
      break;

   case 0x29:			// sh  store half word
      de->_opControl = func_sh;
      de->_memOp = mem_sh;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.imm.rt;
      de->_src_reg[1] = i.imm.rs;
      break;

   case 0x2a:			// swl
      de->_opControl = func_swl;
      de->_memOp = mem_swl;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.imm.rt;
      de->_src_reg[1] = i.imm.rs;
      break;

   case 0x2b:			// sw
      de->_opControl = func_sw;
      de->_memOp = mem_sw;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.imm.rt;
      de->_src_reg[1] = i.imm.rs;
      break;

   case 0x2e:			// swr
      de->_opControl = func_swr;
      de->_memOp = mem_swr;
      de->_decodedSRC1 = _gpr[i.reg.rs];
      de->_decodedSRC2 = i.imm.imm;
      de->_decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      de->_src_reg[0] = i.imm.rt;
      de->_src_reg[1] = i.imm.rs;
      break;

   case 0x11:			// floating-point
      _fpinst++;
      switch (i.freg.fmt) {
      case 4:			// mtc1
         de->_opControl = func_mtc1;
         de->_decodedSRC1 = _gpr[i.freg.ft];
         de->_decodedDST = i.freg.fs;
         de->_writeREG = FALSE;
         de->_writeFREG = TRUE;
         de->_hiWPort = FALSE;
         de->_loWPort = FALSE;
         de->_memControl = FALSE;
         de->_src_reg[0] = i.freg.ft;
	 break;

      case 0:			// mfc1
         de->_opControl = func_mfc1;
         de->_decodedSRC1 = _fpr[(i.freg.fs)>>1].l[FP_TWIDDLE^((i.freg.fs)&1)];
         de->_decodedDST = i.freg.ft;
         de->_writeREG = TRUE;
         de->_writeFREG = FALSE;
         de->_hiWPort = FALSE;
         de->_loWPort = FALSE;
         de->_memControl = FALSE;
         de->_src_freg = i.freg.fs >> 1;
         de->_has_float_src = 1;
	 break;
      default:
         de->_isIllegalOp = TRUE;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
         de->_hiWPort = FALSE;
         de->_loWPort = FALSE;
         de->_memControl = FALSE;
	 break;
      }
      break;
   default:
      de->_isIllegalOp = TRUE;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      break;
   }
}

/*
 *
 * Debugging: print registers
 *
 */
void 
Mipc::dumpregs (void)
{
   int i;

   printf ("\n--- PC = %08x ---\n", _pc);
   for (i=0; i < 32; i++) {
      if (i < 10)
	 printf (" r%d: %08x (%ld)\n", i, _gpr[i], _gpr[i]);
      else
	 printf ("r%d: %08x (%ld)\n", i, _gpr[i], _gpr[i]);
   }
   //printf ("taken: %d, bd: %d\n", _btaken, _de->_bdslot);
   //printf ("target: %08x\n", _btgt);
}

void
Mipc::func_add_addu (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   //printf("Encountered unimplemented instruction: add or addu.\n");
   //printf("You need to fill in func_add_addu in exec_helper.cc to proceed forward.\n");
   //exit(0);
	em->_opResultLo = (unsigned)(de->_decodedSRC1 + de->_decodedSRC2);
}

void
Mipc::func_and (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = de->_decodedSRC1 & de->_decodedSRC2;
}

void
Mipc::func_nor (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = ~(de->_decodedSRC1 | de->_decodedSRC2);
}

void
Mipc::func_or (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = de->_decodedSRC1 | de->_decodedSRC2;
}

void
Mipc::func_sll (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = de->_decodedSRC2 << de->_decodedShiftAmt;
}

void
Mipc::func_sllv (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   //printf("Encountered unimplemented instruction: sllv.\n");
   //printf("You need to fill in func_sllv in exec_helper.cc to proceed forward.\n");
   //exit(0);
	em->_opResultLo = de->_decodedSRC2 << (de->_decodedSRC1 & 0x1F);
}

void
Mipc::func_slt (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   if (de->_decodedSRC1 < de->_decodedSRC2) {
      em->_opResultLo = 1;
   }
   else {
      em->_opResultLo = 0;
   }
}

void
Mipc::func_sltu (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   if ((unsigned)de->_decodedSRC1 < (unsigned)de->_decodedSRC2) {
      em->_opResultLo = 1;
   }
   else {
      em->_opResultLo = 0;
   }
}

void
Mipc::func_sra (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = de->_decodedSRC2 >> de->_decodedShiftAmt;
}

void
Mipc::func_srav (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = de->_decodedSRC2 >> (de->_decodedSRC1 & 0x1f);
}

void
Mipc::func_srl (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = (unsigned)de->_decodedSRC2 >> de->_decodedShiftAmt;
}

void
Mipc::func_srlv (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = (unsigned)de->_decodedSRC2 >> (de->_decodedSRC1 & 0x1f);
}

void
Mipc::func_sub_subu (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = (unsigned)de->_decodedSRC1 - (unsigned)de->_decodedSRC2;
}

void
Mipc::func_xor (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = de->_decodedSRC1 ^ de->_decodedSRC2;
}

void
Mipc::func_div (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   if (de->_decodedSRC2 != 0) {
      em->_opResultHi = (unsigned)(de->_decodedSRC1 % de->_decodedSRC2);
      em->_opResultLo = (unsigned)(de->_decodedSRC1 / de->_decodedSRC2);
   }
   else {
      em->_opResultHi = 0x7fffffff;
      em->_opResultLo = 0x7fffffff;
   }
}

void
Mipc::func_divu (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   if ((unsigned)de->_decodedSRC2 != 0) {
      em->_opResultHi = (unsigned)(de->_decodedSRC1) % (unsigned)(de->_decodedSRC2);
      em->_opResultLo = (unsigned)(de->_decodedSRC1) / (unsigned)(de->_decodedSRC2);
   }
   else {
      em->_opResultHi = 0x7fffffff;
      em->_opResultLo = 0x7fffffff;
   }
}

void
Mipc::func_mfhi (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = mc->_hi;
}

void
Mipc::func_mflo (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = mc->_lo;
}

void
Mipc::func_mthi (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultHi = de->_decodedSRC1;
}

void
Mipc::func_mtlo (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = de->_decodedSRC1;
}

void
Mipc::func_mult (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   unsigned int ar1, ar2, s1, s2, r1, r2, t1, t2;
                                                                                
   ar1 = de->_decodedSRC1;
   ar2 = de->_decodedSRC2;
   s1 = ar1 >> 31; if (s1) ar1 = 0x7fffffff & (~ar1 + 1);
   s2 = ar2 >> 31; if (s2) ar2 = 0x7fffffff & (~ar2 + 1);
                                                                                
   t1 = (ar1 & 0xffff) * (ar2 & 0xffff);
   r1 = t1 & 0xffff;              // bottom 16 bits
                                                                                
   // compute next set of 16 bits
   t1 = (ar1 & 0xffff) * (ar2 >> 16) + (t1 >> 16);
   t2 = (ar2 & 0xffff) * (ar1 >> 16);
                                                                                
   r1 = r1 | (((t1+t2) & 0xffff) << 16); // bottom 32 bits
   r2 = (ar1 >> 16) * (ar2 >> 16) + (t1 >> 16) + (t2 >> 16) +
            (((t1 & 0xffff) + (t2 & 0xffff)) >> 16);
                                                                                
   if (s1 ^ s2) {
      r1 = ~r1;
      r2 = ~r2;
      r1++;
      if (r1 == 0)
         r2++;
   }
   em->_opResultHi = r2;
   em->_opResultLo = r1;
}

void
Mipc::func_multu (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   unsigned int ar1, ar2, s1, s2, r1, r2, t1, t2;
                                                                                
   ar1 = de->_decodedSRC1;
   ar2 = de->_decodedSRC2;
                                                                                
   t1 = (ar1 & 0xffff) * (ar2 & 0xffff);
   r1 = t1 & 0xffff;              // bottom 16 bits
                                                                                
   // compute next set of 16 bits
   t1 = (ar1 & 0xffff) * (ar2 >> 16) + (t1 >> 16);
   t2 = (ar2 & 0xffff) * (ar1 >> 16);
                                                                                
   r1 = r1 | (((t1+t2) & 0xffff) << 16); // bottom 32 bits
   r2 = (ar1 >> 16) * (ar2 >> 16) + (t1 >> 16) + (t2 >> 16) +
            (((t1 & 0xffff) + (t2 & 0xffff)) >> 16);
                            
   em->_opResultHi = r2;
   em->_opResultLo = r1;                                                    
}

void
Mipc::func_jalr (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_btaken = 1;
   mc->_num_jal++;
   em->_opResultLo = de->_pc + 8;
}

void
Mipc::func_jr (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_btaken = 1;
   mc->_num_jr++;
}

void
Mipc::func_await_break (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
}

void
Mipc::func_syscall (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->fake_syscall (ins);
}

void
Mipc::func_addi_addiu (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   //printf("Encountered unimplemented instruction: addi or addiu.\n");
   //printf("You need to fill in func_addi_addiu in exec_helper.cc to proceed forward.\n");
   //exit(0);
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_opResultLo = (unsigned)(de->_decodedSRC1 + de->_decodedSRC2);
}

void
Mipc::func_andi (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = de->_decodedSRC1 & de->_decodedSRC2;
}

void
Mipc::func_lui (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   //printf("Encountered unimplemented instruction: lui.\n");
   //printf("You need to fill in func_lui in exec_helper.cc to proceed forward.\n");
   //exit(0);
   em->_opResultLo = (de->_decodedSRC2 << 16);
}

void
Mipc::func_ori (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   //printf("Encountered unimplemented instruction: ori.\n");
   //printf("You need to fill in func_ori in exec_helper.cc to proceed forward.\n");
   //exit(0);
   em->_opResultLo = (de->_decodedSRC1 | de->_decodedSRC2);
}

void
Mipc::func_slti (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   if (de->_decodedSRC1 < de->_decodedSRC2) {
      em->_opResultLo = 1;
   }
   else {
      em->_opResultLo = 0;
   }
}

void
Mipc::func_sltiu (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   if ((unsigned)de->_decodedSRC1 < (unsigned)de->_decodedSRC2) {
      em->_opResultLo = 1;
   }
   else {
      em->_opResultLo = 0;
   }
}

void
Mipc::func_xori (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = de->_decodedSRC1 ^ de->_decodedSRC2;
}

void
Mipc::func_beq (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_cond_br++;
   //printf("Encountered unimplemented instruction: beq.\n");
   //printf("You need to fill in func_beq in exec_helper.cc to proceed forward.\n");
   //exit(0);
   em->_btaken = (de->_decodedSRC1 == de->_decodedSRC2);
}

void
Mipc::func_bgez (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_cond_br++;
   em->_btaken = !(de->_decodedSRC1 >> 31);
}

void
Mipc::func_bgezal (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_cond_br++;
   em->_btaken = !(de->_decodedSRC1 >> 31);
   em->_opResultLo = de->_pc + 8;
}

void
Mipc::func_bltzal (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_cond_br++;
   em->_btaken = (de->_decodedSRC1 >> 31);
   em->_opResultLo = de->_pc + 8;
}

void
Mipc::func_bltz (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_cond_br++;
   em->_btaken = (de->_decodedSRC1 >> 31);
}

void
Mipc::func_bgtz (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_cond_br++;
   em->_btaken = (de->_decodedSRC1 > 0);
}

void
Mipc::func_blez (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_cond_br++;
   em->_btaken = (de->_decodedSRC1 <= 0);
}

void
Mipc::func_bne (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_cond_br++;
   em->_btaken = (de->_decodedSRC1 != de->_decodedSRC2);
}

void
Mipc::func_j (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_btaken = 1;
}

void
Mipc::func_jal (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_jal++;
   //printf("Encountered unimplemented instruction: jal.\n");
   //printf("You need to fill in func_jal in exec_helper.cc to proceed forward.\n");
   //exit(0);
   em->_btaken = 1;
   em->_opResultLo = de->_pc + 8;
}

void
Mipc::func_lb (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   signed int a1;

   mc->_num_load++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_lbu (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_load++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_lh (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   signed int a1;
                                                                                
   mc->_num_load++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_lhu (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_load++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_lwl (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   signed int a1;
   unsigned s1;
                                                                                
   mc->_num_load++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_lw (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_load++;
   //printf("Encountered unimplemented instruction: lw.\n");
   //printf("You need to fill in func_lw in exec_helper.cc to proceed forward.\n");
   //exit(0);
   
   signed int a1;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1 + de->_decodedSRC2);
}

void
Mipc::func_lwr (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   unsigned ar1, s1;
                                                                                
   mc->_num_load++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_lwc1 (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_load++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_swc1 (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_store++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_sb (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_store++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_sh (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_store++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_swl (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   unsigned ar1, s1;
                                                                                
   mc->_num_store++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_sw (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   mc->_num_store++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_swr (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   unsigned ar1, s1;
                                                                                
   mc->_num_store++;
   SIGN_EXTEND_IMM(de->_decodedSRC2);
   em->_memory_addr_reg = (unsigned)(de->_decodedSRC1+de->_decodedSRC2);
}

void
Mipc::func_mtc1 (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = de->_decodedSRC1;
}

void
Mipc::func_mfc1 (Mipc *mc, unsigned ins, pipe_reg_t* de, pipe_reg_t* em)
{
   em->_opResultLo = de->_decodedSRC1;
}

void
Mipc::mem_lb (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   signed int a1;

   a1 = mc->_mem->BEGetByte(em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7));
   SIGN_EXTEND_BYTE(a1);
   mw->_opResultLo = a1;
}

void
Mipc::mem_lbu (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   mw->_opResultLo = mc->_mem->BEGetByte(em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7));
}

void
Mipc::mem_lh (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   signed int a1;

   a1 = mc->_mem->BEGetHalfWord(em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7));
   SIGN_EXTEND_IMM(a1);
   mw->_opResultLo = a1;
}

void
Mipc::mem_lhu (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   mw->_opResultLo = mc->_mem->BEGetHalfWord (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7));
}

void
Mipc::mem_lwl (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   signed int a1;
   unsigned s1;

   a1 = mc->_mem->BEGetWord (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7));
   s1 = (em->_memory_addr_reg & 3) << 3;
   mw->_opResultLo = (a1 << s1) | (em->_subregOperand & ~(~0UL << s1));
}

void
Mipc::mem_lw (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   mw->_opResultLo = mc->_mem->BEGetWord (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7));
}

void
Mipc::mem_lwr (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   unsigned ar1, s1;

   ar1 = mc->_mem->BEGetWord (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7));
   s1 = (~em->_memory_addr_reg & 3) << 3;
   mw->_opResultLo = (ar1 >> s1) | (em->_subregOperand & ~(~(unsigned)0 >> s1));
}

void
Mipc::mem_lwc1 (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   mw->_opResultLo = mc->_mem->BEGetWord (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7));
}

void
Mipc::mem_swc1 (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   mc->_mem->Write(em->_memory_addr_reg & ~(LL)0x7, mc->_mem->BESetWord (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7), mc->_fpr[em->_decodedDST>>1].l[FP_TWIDDLE^(em->_decodedDST&1)]));
}

void
Mipc::mem_sb (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   mc->_mem->Write(em->_memory_addr_reg & ~(LL)0x7, mc->_mem->BESetByte (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7), mc->_gpr[em->_decodedDST] & 0xff));
}

void
Mipc::mem_sh (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   mc->_mem->Write(em->_memory_addr_reg & ~(LL)0x7, mc->_mem->BESetHalfWord (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7), mc->_gpr[em->_decodedDST] & 0xffff));
}

void
Mipc::mem_swl (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   unsigned ar1, s1;

   ar1 = mc->_mem->BEGetWord (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7));
   s1 = (em->_memory_addr_reg & 3) << 3;
   ar1 = (mc->_gpr[em->_decodedDST] >> s1) | (ar1 & ~(~(unsigned)0 >> s1));
   mc->_mem->Write(em->_memory_addr_reg & ~(LL)0x7, mc->_mem->BESetWord (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7), ar1));
}

void
Mipc::mem_sw (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   mc->_mem->Write(em->_memory_addr_reg & ~(LL)0x7, mc->_mem->BESetWord (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7), mc->_gpr[em->_decodedDST]));
}

void
Mipc::mem_swr (Mipc *mc, pipe_reg_t* em, pipe_reg_t* mw)
{
   unsigned ar1, s1;

   ar1 = mc->_mem->BEGetWord (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7));
   s1 = (~em->_memory_addr_reg & 3) << 3;
   ar1 = (mc->_gpr[em->_decodedDST] << s1) | (ar1 & ~(~0UL << s1));
   mc->_mem->Write(em->_memory_addr_reg & ~(LL)0x7, mc->_mem->BESetWord (em->_memory_addr_reg, mc->_mem->Read(em->_memory_addr_reg & ~(LL)0x7), ar1));
}


