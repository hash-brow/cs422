/*
 * utils.h
 * cs422 hw 1
 *
 * created on feb 19, 2025
 */

#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include "pin.H"

typedef struct _bbl_val {
    // list of static addresses which are touched in the BBL
    // basically just instruction addresses, as mem addresses
    // have to be inserted in a predicated fashion (even in static)
    // not the actual VA, but divided by gran.
    std::vector<uint32_t> static_addr; 

    UINT64 ins_cnt;

    UINT64 load;
    UINT64 store;

    UINT64 nop;
    UINT64 call_drct;
    UINT64 call_indrct;
    UINT64 ret;
    UINT64 uncond_br;
    UINT64 cond_br;
    UINT64 logical;
    UINT64 rotate_shift;
    UINT64 flagop;
    UINT64 vector;
    UINT64 cmov;
    UINT64 mmx_sse;
    UINT64 syscall;
    UINT64 alu;
    UINT64 misc;

    UINT64 memOps=0;
    UINT64 instr_length[20] = {0};
    UINT64 op_count[10] = {0};
    UINT64 reg_reads[10] = {0};
    UINT64 reg_writes[10] = {0};
    UINT64 mem_ops[10] = {0};
    UINT64 mem_reads[10] = {0};
    UINT64 mem_writes[10] = {0};
    UINT64 max_mem_bytes = 0, total_mem_bytes = 0, mem_instr_count = 0;
    INT32 max_imm = INT32_MIN, min_imm = INT32_MAX;
    ADDRDELTA max_disp = INT32_MIN, min_disp = INT32_MAX;
    bool found_imm = false;
} bbl_val;

VOID accumulate_bbl_val(bbl_val* dest, const bbl_val* src, UINT64 mul);
void print_bbl_val(std::ostream &OutFile, const bbl_val& val);

#endif // _UTILS_H
