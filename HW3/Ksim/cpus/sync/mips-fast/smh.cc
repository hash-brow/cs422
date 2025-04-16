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
   mw->_opResultLo = (a1 << s1) | (mc->_subregOperand & ~(~0UL << s1));
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
   mw->_opResultLo = (ar1 >> s1) | (mc->_subregOperand & ~(~(unsigned)0 >> s1));
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

