void
Mipc::func_add_addu (Mipc *mc, unsigned ins)
{
   //printf("Encountered unimplemented instruction: add or addu.\n");
   //printf("You need to fill in func_add_addu in exec_helper.cc to proceed forward.\n");
   //exit(0);
	mc->_opResultLo = (unsigned)(mc->_decodedSRC1 + mc->_decodedSRC2);
}

void
Mipc::func_and (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1 & mc->_decodedSRC2;
}

void
Mipc::func_nor (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = ~(mc->_decodedSRC1 | mc->_decodedSRC2);
}

void
Mipc::func_or (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1 | mc->_decodedSRC2;
}

void
Mipc::func_sll (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC2 << mc->_decodedShiftAmt;
}

void
Mipc::func_sllv (Mipc *mc, unsigned ins)
{
   //printf("Encountered unimplemented instruction: sllv.\n");
   //printf("You need to fill in func_sllv in exec_helper.cc to proceed forward.\n");
   //exit(0);
	mc->_opResultLo = mc->_decodedSRC2 << (mc->_decodedSRC1 & 0x1F);
}

void
Mipc::func_slt (Mipc *mc, unsigned ins)
{
   if (mc->_decodedSRC1 < mc->_decodedSRC2) {
      mc->_opResultLo = 1;
   }
   else {
      mc->_opResultLo = 0;
   }
}

void
Mipc::func_sltu (Mipc *mc, unsigned ins)
{
   if ((unsigned)mc->_decodedSRC1 < (unsigned)mc->_decodedSRC2) {
      mc->_opResultLo = 1;
   }
   else {
      mc->_opResultLo = 0;
   }
}

void
Mipc::func_sra (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC2 >> mc->_decodedShiftAmt;
}

void
Mipc::func_srav (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC2 >> (mc->_decodedSRC1 & 0x1f);
}

void
Mipc::func_srl (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = (unsigned)mc->_decodedSRC2 >> mc->_decodedShiftAmt;
}

void
Mipc::func_srlv (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = (unsigned)mc->_decodedSRC2 >> (mc->_decodedSRC1 & 0x1f);
}

void
Mipc::func_sub_subu (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = (unsigned)mc->_decodedSRC1 - (unsigned)mc->_decodedSRC2;
}

void
Mipc::func_xor (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1 ^ mc->_decodedSRC2;
}

void
Mipc::func_div (Mipc *mc, unsigned ins)
{
   if (mc->_decodedSRC2 != 0) {
      mc->_opResultHi = (unsigned)(mc->_decodedSRC1 % mc->_decodedSRC2);
      mc->_opResultLo = (unsigned)(mc->_decodedSRC1 / mc->_decodedSRC2);
   }
   else {
      mc->_opResultHi = 0x7fffffff;
      mc->_opResultLo = 0x7fffffff;
   }
}

void
Mipc::func_divu (Mipc *mc, unsigned ins)
{
   if ((unsigned)mc->_decodedSRC2 != 0) {
      mc->_opResultHi = (unsigned)(mc->_decodedSRC1) % (unsigned)(mc->_decodedSRC2);
      mc->_opResultLo = (unsigned)(mc->_decodedSRC1) / (unsigned)(mc->_decodedSRC2);
   }
   else {
      mc->_opResultHi = 0x7fffffff;
      mc->_opResultLo = 0x7fffffff;
   }
}

void
Mipc::func_mfhi (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_hi;
}

void
Mipc::func_mflo (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_lo;
}

void
Mipc::func_mthi (Mipc *mc, unsigned ins)
{
   mc->_opResultHi = mc->_decodedSRC1;
}

void
Mipc::func_mtlo (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1;
}

void
Mipc::func_mult (Mipc *mc, unsigned ins)
{
   unsigned int ar1, ar2, s1, s2, r1, r2, t1, t2;
                                                                                
   ar1 = mc->_decodedSRC1;
   ar2 = mc->_decodedSRC2;
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
   mc->_opResultHi = r2;
   mc->_opResultLo = r1;
}

void
Mipc::func_multu (Mipc *mc, unsigned ins)
{
   unsigned int ar1, ar2, s1, s2, r1, r2, t1, t2;
                                                                                
   ar1 = mc->_decodedSRC1;
   ar2 = mc->_decodedSRC2;
                                                                                
   t1 = (ar1 & 0xffff) * (ar2 & 0xffff);
   r1 = t1 & 0xffff;              // bottom 16 bits
                                                                                
   // compute next set of 16 bits
   t1 = (ar1 & 0xffff) * (ar2 >> 16) + (t1 >> 16);
   t2 = (ar2 & 0xffff) * (ar1 >> 16);
                                                                                
   r1 = r1 | (((t1+t2) & 0xffff) << 16); // bottom 32 bits
   r2 = (ar1 >> 16) * (ar2 >> 16) + (t1 >> 16) + (t2 >> 16) +
            (((t1 & 0xffff) + (t2 & 0xffff)) >> 16);
                            
   mc->_opResultHi = r2;
   mc->_opResultLo = r1;                                                    
}

void
Mipc::func_jalr (Mipc *mc, unsigned ins)
{
   mc->_btaken = 1;
   mc->_num_jal++;
   mc->_opResultLo = mc->_pc + 8;
}

void
Mipc::func_jr (Mipc *mc, unsigned ins)
{
   mc->_btaken = 1;
   mc->_num_jr++;
}

void
Mipc::func_await_break (Mipc *mc, unsigned ins)
{
}

void
Mipc::func_syscall (Mipc *mc, unsigned ins)
{
   mc->fake_syscall (ins);
}

void
Mipc::func_addi_addiu (Mipc *mc, unsigned ins)
{
   //printf("Encountered unimplemented instruction: addi or addiu.\n");
   //printf("You need to fill in func_addi_addiu in exec_helper.cc to proceed forward.\n");
   //exit(0);
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_opResultLo = (unsigned)(mc->_decodedSRC1 + mc->_decodedSRC2);
}

void
Mipc::func_andi (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1 & mc->_decodedSRC2;
}

void
Mipc::func_lui (Mipc *mc, unsigned ins)
{
   //printf("Encountered unimplemented instruction: lui.\n");
   //printf("You need to fill in func_lui in exec_helper.cc to proceed forward.\n");
   //exit(0);
   mc->_opResultLo = (mc->_decodedSRC2 << 16);
}

void
Mipc::func_ori (Mipc *mc, unsigned ins)
{
   //printf("Encountered unimplemented instruction: ori.\n");
   //printf("You need to fill in func_ori in exec_helper.cc to proceed forward.\n");
   //exit(0);
   mc->_opResultLo = (mc->_decodedSRC1 | mc->_decodedSRC2);
}

void
Mipc::func_slti (Mipc *mc, unsigned ins)
{
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   if (mc->_decodedSRC1 < mc->_decodedSRC2) {
      mc->_opResultLo = 1;
   }
   else {
      mc->_opResultLo = 0;
   }
}

void
Mipc::func_sltiu (Mipc *mc, unsigned ins)
{
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   if ((unsigned)mc->_decodedSRC1 < (unsigned)mc->_decodedSRC2) {
      mc->_opResultLo = 1;
   }
   else {
      mc->_opResultLo = 0;
   }
}

void
Mipc::func_xori (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1 ^ mc->_decodedSRC2;
}

void
Mipc::func_beq (Mipc *mc, unsigned ins)
{
   mc->_num_cond_br++;
   //printf("Encountered unimplemented instruction: beq.\n");
   //printf("You need to fill in func_beq in exec_helper.cc to proceed forward.\n");
   //exit(0);
   mc->_btaken = (mc->_decodedSRC1 == mc->_decodedSRC2);
}

void
Mipc::func_bgez (Mipc *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = !(mc->_decodedSRC1 >> 31);
}

void
Mipc::func_bgezal (Mipc *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = !(mc->_decodedSRC1 >> 31);
   mc->_opResultLo = mc->_pc + 8;
}

void
Mipc::func_bltzal (Mipc *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = (mc->_decodedSRC1 >> 31);
   mc->_opResultLo = mc->_pc + 8;
}

void
Mipc::func_bltz (Mipc *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = (mc->_decodedSRC1 >> 31);
}

void
Mipc::func_bgtz (Mipc *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = (mc->_decodedSRC1 > 0);
}

void
Mipc::func_blez (Mipc *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = (mc->_decodedSRC1 <= 0);
}

void
Mipc::func_bne (Mipc *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = (mc->_decodedSRC1 != mc->_decodedSRC2);
}

void
Mipc::func_j (Mipc *mc, unsigned ins)
{
   mc->_btaken = 1;
}

void
Mipc::func_jal (Mipc *mc, unsigned ins)
{
   mc->_num_jal++;
   //printf("Encountered unimplemented instruction: jal.\n");
   //printf("You need to fill in func_jal in exec_helper.cc to proceed forward.\n");
   //exit(0);
   mc->_btaken = 1;
   mc->_opResultLo = mc->_pc + 8;
}

void
Mipc::func_lb (Mipc *mc, unsigned ins)
{
   signed int a1;

   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_lbu (Mipc *mc, unsigned ins)
{
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_lh (Mipc *mc, unsigned ins)
{
   signed int a1;
                                                                                
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_lhu (Mipc *mc, unsigned ins)
{
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_lwl (Mipc *mc, unsigned ins)
{
   signed int a1;
   unsigned s1;
                                                                                
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_lw (Mipc *mc, unsigned ins)
{
   mc->_num_load++;
   //printf("Encountered unimplemented instruction: lw.\n");
   //printf("You need to fill in func_lw in exec_helper.cc to proceed forward.\n");
   //exit(0);
   
   signed int a1;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1 + mc->_decodedSRC2);
}

void
Mipc::func_lwr (Mipc *mc, unsigned ins)
{
   unsigned ar1, s1;
                                                                                
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_lwc1 (Mipc *mc, unsigned ins)
{
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_swc1 (Mipc *mc, unsigned ins)
{
   mc->_num_store++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_sb (Mipc *mc, unsigned ins)
{
   mc->_num_store++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_sh (Mipc *mc, unsigned ins)
{
   mc->_num_store++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_swl (Mipc *mc, unsigned ins)
{
   unsigned ar1, s1;
                                                                                
   mc->_num_store++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_sw (Mipc *mc, unsigned ins)
{
   mc->_num_store++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_swr (Mipc *mc, unsigned ins)
{
   unsigned ar1, s1;
                                                                                
   mc->_num_store++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
Mipc::func_mtc1 (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1;
}

void
Mipc::func_mfc1 (Mipc *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1;
}

