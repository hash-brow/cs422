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
   de->_isSyscall = FALSE;

   i.data = fd->_ins;
  
#define SIGN_EXTEND_BYTE(x)  do { x <<= 24; x >>= 24; } while (0)
#define SIGN_EXTEND_IMM(x)   do { x <<= 16; x >>= 16; } while (0)

   switch (i.reg.op) {
   case 0:
      // SPECIAL (ALU format)
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = _gpr[i.reg.rt];
      de->decodedDST = i.reg.rd;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;

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
         de->decodedShiftAmt = i.reg.sa;
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
         de->decodedShiftAmt = i.reg.sa;
	 break;

      case 0x7:			// srav
         de->_opControl = func_srav;
	 break;

      case 0x2:			// srl
         de->_opControl = func_srl;
         de->decodedShiftAmt = i.reg.sa;
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
	 break;

      case 0x12:			// mflo
         de->_opControl = func_mflo;
	 break;

      case 0x11:			// mthi
         de->_opControl = func_mthi;
         de->_hiWPort = TRUE;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
	 break;

      case 0x13:			// mtlo
         de->_opControl = func_mtlo;
         de->_loWPort = TRUE;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
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
         de->_btgt = de->decodedSRC1;
         _bdslot = 1;
         break;

      case 8:			// jr
         de->_opControl = func_jr;
         de->_writeREG = FALSE;
         de->_writeFREG = FALSE;
         de->_btgt = de->decodedSRC1;
         _bdslot = 1;
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
      de->decodedSRC1 = _gpr[i.imm.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
       de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      break;

   case 0xc:			// andi
      de->_opControl = func_andi;
      de->decodedSRC1 = _gpr[i.imm.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      break;

   case 0xf:			// lui
      de->_opControl = func_lui;
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      break;

   case 0xd:			// ori
      de->_opControl = func_ori;
      de->decodedSRC1 = _gpr[i.imm.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      break;

   case 0xa:			// slti
      de->_opControl = func_slti;
      de->decodedSRC1 = _gpr[i.imm.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      break;

   case 0xb:			// sltiu
      de->_opControl = func_sltiu;
      de->decodedSRC1 = _gpr[i.imm.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      break;

   case 0xe:			// xori
      de->_opControl = func_xori;
      de->decodedSRC1 = _gpr[i.imm.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.imm.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      break;

   case 4:			// beq
      de->_opControl = func_beq;
      de->decodedSRC1 = _gpr[i.imm.rs];
      de->decodedSRC2 = _gpr[i.imm.rt];
      de->_branchOffset = i.imm.imm;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_branchOffset <<= 16; de->_branchOffset >>= 14; _bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
      break;

   case 1:
      // REGIMM
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->_branchOffset = i.imm.imm;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;

      switch (i.reg.rt) {
      case 1:			// bgez
         de->_opControl = func_bgez;
         de->_branchOffset <<= 16; de->_branchOffset >>= 14; _bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
	 break;

      case 0x11:			// bgezal
         de->_opControl = func_bgezal;
         de->decodedDST = 31;
         de->_writeREG = TRUE;
         de->_branchOffset <<= 16; de->_branchOffset >>= 14; _bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
	 break;

      case 0x10:			// bltzal
         de->_opControl = func_bltzal;
         de->decodedDST = 31;
         de->_writeREG = TRUE;
         de->_branchOffset <<= 16; de->_branchOffset >>= 14; _bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
	 break;

      case 0x0:			// bltz
         de->_opControl = func_bltz;
         de->_branchOffset <<= 16; de->_branchOffset >>= 14; _bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
	 break;

      default:
	 de->_isIllegalOp = TRUE;
	 break;
      }
      break;

   case 7:			// bgtz
      de->_opControl = func_bgtz;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->_branchOffset = i.imm.imm;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_branchOffset <<= 16; de->_branchOffset >>= 14; _bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
      break;

      case 6:			// blez
      de->_opControl = func_blez;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->_branchOffset = i.imm.imm;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_branchOffset <<= 16; de->_branchOffset >>= 14; _bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
      break;

   case 5:			// bne
      de->_opControl = func_bne;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = _gpr[i.reg.rt];
      de->_branchOffset = i.imm.imm;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_branchOffset <<= 16; de->_branchOffset >>= 14; _bdslot = 1; de->_btgt = (unsigned)((signed)_pc+de->_branchOffset+4);
      break;

   case 2:			// j
      de->_opControl = func_j;
      de->_branchOffset = i.tgt.tgt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_btgt = ((_pc+4) & 0xf0000000) | (de->_branchOffset<<2); _bdslot = 1;
      break;

   case 3:			// jal
      de->_opControl = func_jal;
      de->_branchOffset = i.tgt.tgt;
      de->decodedDST = 31;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = FALSE;
      de->_btgt = ((_pc+4) & 0xf0000000) | (de->_branchOffset<<2); _bdslot = 1;
      break;

   case 0x20:			// lb  
      de->_opControl = func_lb;
      de->_memOp = mem_lb;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x24:			// lbu
      de->_opControl = func_lbu;
      de->_memOp = mem_lbu;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x21:			// lh
      de->_opControl = func_lh;
      de->_memOp = mem_lh;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x25:			// lhu
      de->_opControl = func_lhu;
      de->_memOp = mem_lhu;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x22:			// lwl
      de->_opControl = func_lwl;
      de->_memOp = mem_lwl;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->_subregOperand = _gpr[i.reg.rt];
      de->decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x23:			// lw
      de->_opControl = func_lw;
      de->_memOp = mem_lw;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x26:			// lwr
      de->_opControl = func_lwr;
      de->_memOp = mem_lwr;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->_subregOperand = _gpr[i.reg.rt];
      de->decodedDST = i.reg.rt;
      de->_writeREG = TRUE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x31:			// lwc1
      de->_opControl = func_lwc1;
      de->_memOp = mem_lwc1;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = TRUE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x39:			// swc1
      de->_opControl = func_swc1;
      de->_memOp = mem_swc1;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x28:			// sb
      de->_opControl = func_sb;
      de->_memOp = mem_sb;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x29:			// sh  store half word
      de->_opControl = func_sh;
      de->_memOp = mem_sh;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x2a:			// swl
      de->_opControl = func_swl;
      de->_memOp = mem_swl;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x2b:			// sw
      de->_opControl = func_sw;
      de->_memOp = mem_sw;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x2e:			// swr
      de->_opControl = func_swr;
      de->_memOp = mem_swr;
      de->decodedSRC1 = _gpr[i.reg.rs];
      de->decodedSRC2 = i.imm.imm;
      de->decodedDST = i.reg.rt;
      de->_writeREG = FALSE;
      de->_writeFREG = FALSE;
      de->_hiWPort = FALSE;
      de->_loWPort = FALSE;
      de->_memControl = TRUE;
      break;

   case 0x11:			// floating-point
      _fpinst++;
      switch (i.freg.fmt) {
      case 4:			// mtc1
         de->_opControl = func_mtc1;
         de->decodedSRC1 = _gpr[i.freg.ft];
         de->decodedDST = i.freg.fs;
         de->_writeREG = FALSE;
         de->_writeFREG = TRUE;
         de->_hiWPort = FALSE;
         de->_loWPort = FALSE;
         de->_memControl = FALSE;
	 break;

      case 0:			// mfc1
         de->_opControl = func_mfc1;
         de->decodedSRC1 = _fpr[(i.freg.fs)>>1].l[FP_TWIDDLE^((i.freg.fs)&1)];
         de->decodedDST = i.freg.ft;
         de->_writeREG = TRUE;
         de->_writeFREG = FALSE;
         de->_hiWPort = FALSE;
         de->_loWPort = FALSE;
         de->_memControl = FALSE;
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


