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
    dest->cmov += src->cmov * mul;
    dest->mmx_sse += src->mmx_sse * mul;
    dest->syscall += src->syscall * mul;
    dest->alu += src->alu * mul;
    dest->misc += src->misc * mul;
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
}

