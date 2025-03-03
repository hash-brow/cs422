/*
 * utils.c
 * cs422 hw 1
 *
 * created on feb 19, 2025
 */

#include "utils.h"

using std::ios;
using std::endl;

VOID accumulate_bbl_val(bbl_val* dest, const bbl_val* src, UINT64 mul) {
    dest->ins_cnt += src->ins_cnt * mul;
    dest->load += src->load * mul;
    dest->store += src->store * mul;
    dest->nop += src->nop * mul;
    dest->call_drct += src->call_drct * mul;
    dest->call_indrct += src->call_indrct * mul;
    dest->ret += src->ret * mul;
    dest->uncond_br += src->uncond_br * mul;
    dest->cond_br += src->cond_br * mul;
    dest->logical += src->logical * mul;
    dest->rotate_shift += src->rotate_shift * mul;
    dest->flagop += src->flagop * mul;
    dest->vector += src->vector * mul;
    dest->mmx_sse += src->mmx_sse * mul;
    dest->syscall += src->syscall * mul;
    dest->alu += src->alu * mul;
    dest->misc += src->misc * mul;

    for(UINT32 i = 0; i < 20; i++){
        dest->instr_length[i] += src->instr_length[i] * mul;
    }

    for(UINT32 i = 0; i < 10; i++){
        dest->op_count[i] += src->op_count[i] * mul;
        dest->reg_reads[i] += src->reg_reads[i] * mul;
        dest->reg_writes[i] += src->reg_writes[i] * mul;
        
	if(mul) dest->mem_ops[i] += src->mem_ops[i];
        if(mul) dest->mem_reads[i] += src->mem_reads[i];
        if(mul) dest->mem_writes[i] += src->mem_writes[i];
    }
    
    if(mul) dest->total_mem_bytes += src->total_mem_bytes;
    if(mul) dest->mem_instr_count += src->mem_instr_count;
    if(mul) dest->max_mem_bytes = std::max(dest->max_mem_bytes, src->max_mem_bytes);

    if(mul) dest->min_imm = std::min(dest->min_imm, src->min_imm);
    if(mul) dest->max_imm = std::max(dest->max_imm, src->max_imm);

    if(mul) dest->max_disp = std::max(dest->max_disp, src->max_disp);
    if(mul) dest->min_disp = std::min(dest->min_disp, src->min_disp);
}

void print_bbl_val(std::ostream &OutFile, const bbl_val& val) {
    // Compute the sum of all fields except ins_cnt
    INT64 sum_other_fields = val.load + val.store + val.nop + val.call_drct + val.call_indrct +
                             val.ret + val.uncond_br + val.cond_br + val.logical + val.rotate_shift +
                             val.flagop + val.vector + val.cmov + val.mmx_sse + val.syscall +
                             val.alu + val.misc;

    double divisor = (sum_other_fields != 0) ? static_cast<double>(sum_other_fields) : 1.0; // Prevent division by zero

    // Print header
    OutFile << std::left << std::setw(20) << "Field"
            << std::setw(12) << "Value"
            << "Percentage\n";

    OutFile << std::string(45, '-') << "\n";

    // Print data
    OutFile << std::left << std::setw(20) << "ins_cnt"
            << std::setw(12) << val.ins_cnt
            << "-\n";

    OutFile << std::fixed << std::setprecision(2);

    OutFile << std::left << std::setw(20) << "load"
            << std::setw(12) << val.load
            << (val.load / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "store"
            << std::setw(12) << val.store
            << (val.store / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "nop"
            << std::setw(12) << val.nop
            << (val.nop / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "call_drct"
            << std::setw(12) << val.call_drct
            << (val.call_drct / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "call_indrct"
            << std::setw(12) << val.call_indrct
            << (val.call_indrct / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "ret"
            << std::setw(12) << val.ret
            << (val.ret / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "uncond_br"
            << std::setw(12) << val.uncond_br
            << (val.uncond_br / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "cond_br"
            << std::setw(12) << val.cond_br
            << (val.cond_br / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "logical"
            << std::setw(12) << val.logical
            << (val.logical / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "rotate_shift"
            << std::setw(12) << val.rotate_shift
            << (val.rotate_shift / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "flagop"
            << std::setw(12) << val.flagop
            << (val.flagop / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "vector"
            << std::setw(12) << val.vector
            << (val.vector / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "cmov"
            << std::setw(12) << val.cmov
            << (val.cmov / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "mmx_sse"
            << std::setw(12) << val.mmx_sse
            << (val.mmx_sse / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "syscall"
            << std::setw(12) << val.syscall
            << (val.syscall / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "alu"
            << std::setw(12) << val.alu
            << (val.alu / divisor) * 100 << "%\n";

    OutFile << std::left << std::setw(20) << "misc"
            << std::setw(12) << val.misc
            << (val.misc / divisor) * 100 << "%\n";

    OutFile << "1. Instruction Length Distribution:\n";
    for(int i = 0; i < 16; i++) {
        OutFile << i << " bytes: " << val.instr_length[i] << "\n";
    }
    OutFile << "\n2. Operands per Instruction:\n";
    for(int i = 0; i < 10; i++) {
        OutFile << i << " operands: " << val.op_count[i] << "\n";
    }
    OutFile << "\n3. Register Read Operands:\n";
    for(int i = 0; i < 10; i++) {
        OutFile << i << " reads: " << val.reg_reads[i] << "\n";
    }
    OutFile << "\n4. Register Write Operands:\n";
    for(int i = 0; i < 10; i++) {
        OutFile << i << " writes: " << val.reg_writes[i] << "\n";
    }
    OutFile << "\n5. Memory Operands per Instruction:\n";
    for(int i = 0; i < 10; i++) {
        if(i == 0){
		UINT32 tot = 0;
		for (int j = 0; j < 10; j++) tot += val.mem_ops[j];

		OutFile << i << " operands: " << val.ins_cnt - tot << "\n";
	}
	else OutFile << i << " operands: " << val.mem_ops[i] << "\n";
    }
    OutFile << "\n6. Memory Read Operands:\n";
    for(int i = 0; i < 10; i++) {
        // if(i==0)OutFile << i << " operands: " << val.mem_reads[i] << "\n";
        OutFile << i << " operands: " << val.mem_reads[i] << "\n";
    }
    OutFile << "\n7. Memory Write Operands:\n";
    for(int i = 0; i < 10; i++) {
        // if(i==0)OutFile << i << " operands: " << stats.mem_writes[i] << "\n";
        OutFile << i << " operands: " << val.mem_writes[i] << "\n";
    }
    OutFile << "\n8. Memory Bytes Touched:\n";
    OutFile << "Max: " << val.max_mem_bytes << " bytes\n";
    double avgMemBytes = static_cast<double>(val.total_mem_bytes) / val.mem_instr_count;
    OutFile << "Average: " << std::fixed << std::setprecision(5) 
            << avgMemBytes << " bytes\n";

    OutFile << "\n9. Immediate Values:\n";
    OutFile << "Max: " << val.max_imm << "\n";
    OutFile << "Min: " << val.min_imm << "\n";
    OutFile << "\n10. Displacement Values:\n";
    OutFile << "Max: " << val.max_disp << "\n";
    OutFile << "Min: " << val.min_disp << "\n";
}

