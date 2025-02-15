/*
 * main.cpp
 *
 * CS422 2024-25 II Assignment 1
 * Pintool file for Part A 
 *
 * Created on Feb 15, 2025
 */

#include <iostream>
#include <fstream>
#include <assert.h>
#include "pin.H"
using std::cerr;
using std::endl;
using std::ios;
using std::ofstream;
using std::string;

ofstream OutFile;

typedef struct _bbl_val {
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
} bbl_val;

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

void print_bbl_val(const bbl_val& val) {
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

double calc_cpi(const bbl_val& val) {
    // Sum all fields except ins_cnt
    INT64 sum_other_fields = val.load + val.store + val.nop + val.call_drct + val.call_indrct +
                             val.ret + val.uncond_br + val.cond_br + val.logical + val.rotate_shift +
                             val.flagop + val.vector + val.cmov + val.mmx_sse + val.syscall +
                             val.alu + val.misc;

    // Compute the weighted sum
    INT64 weighted_sum = val.nop + val.call_drct + val.call_indrct + val.ret +
                         val.uncond_br + val.cond_br + val.logical + val.rotate_shift +
                         val.flagop + val.vector + val.cmov + val.mmx_sse + val.syscall +
                         val.alu + val.misc + 70 * (val.load + val.store);

    // Prevent division by zero
    return (sum_other_fields != 0) ? static_cast<double>(weighted_sum) / sum_other_fields : 0.0;
}

static UINT32 bbl_counter = 0;

#define BBL_MAX 10000000
static bbl_val bbl_data[BBL_MAX];
static UINT64 bbl_cnt[BBL_MAX]; // how many times has each BBL been run

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 icount = 0;
static UINT64 ff_cnt;

VOID bbl_ins_count(INT32 idx) {
    icount += bbl_data[idx].ins_cnt;
}

// check if we are doing with fast forwarding
const UINT64 range_len = 1e9;
ADDRINT ff_valid() {
    return icount >= ff_cnt;
}

ADDRINT terminate() {
    return icount >= ff_cnt + range_len;
}

// analysis lessgoo
VOID analysis(INT32 idx) {
    bbl_cnt[idx]++;
}

VOID exit_routine() {
    bbl_val results;
    memset(&results, 0, sizeof results);

    INT64 ins_tot = 0;

    for (UINT32 i = 0; i < bbl_counter; i++) {
        accumulate_bbl_val(&results, &bbl_data[i], bbl_cnt[i]);
        ins_tot += bbl_data[i].ins_cnt;
    }
    print_bbl_val(results);

    OutFile << "BBL counter: " << bbl_counter << "\n"; 
    OutFile << "Mean BBL sz (ins): " << static_cast<double>(ins_tot/bbl_counter) << "\n";
    OutFile << "CPI: " << calc_cpi(results) << "\n";

    exit(0);
}

// Pin calls this function every time a new basic block is encountered
VOID Trace(TRACE trace, VOID* v)
{
    // Visit every basic block  in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        bbl_val* data = &bbl_data[bbl_counter];
        bbl_counter++;
        assert(bbl_counter < BBL_MAX);
        //if (bbl_counter == BBL_MAX) bbl_counter = 1;

        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)) {
            // Type B stuff
            UINT32 mem_op_cnt = INS_MemoryOperandCount(ins);
            for (UINT32 mem_op = 0; mem_op < mem_op_cnt; ++mem_op) {
                if (INS_MemoryOperandIsRead(ins, mem_op)) {
                    data->load += (INS_MemoryOperandSize(ins, mem_op) + 3) / 4;
                } else if (INS_MemoryOperandIsWritten(ins, mem_op)) {
                    data->store += (INS_MemoryOperandSize(ins, mem_op) + 3) / 4;
                }
            }

            // Type A stuff
            switch (INS_Category(ins)) {
                case XED_CATEGORY_NOP:
                    data->nop++;
                    break;
                case XED_CATEGORY_CALL:
                    if (INS_IsDirectCall(ins)) data->call_drct++;
                    else data->call_indrct++;
                    break;
                case XED_CATEGORY_RET:
                    data->ret++;
                    break;
                case XED_CATEGORY_UNCOND_BR:
                    data->uncond_br++;
                    break;
                case XED_CATEGORY_COND_BR:
                    data->cond_br++;
                    break;
                case XED_CATEGORY_LOGICAL:
                    data->logical++;
                    break;
                case XED_CATEGORY_ROTATE:
                case XED_CATEGORY_SHIFT:
                    data->rotate_shift++;
                    break;
                case XED_CATEGORY_FLAGOP:
                    data->flagop++;
                    break;
                case XED_CATEGORY_AVX:
                case XED_CATEGORY_AVX2:
                case XED_CATEGORY_AVX2GATHER:
                case XED_CATEGORY_AVX512:
                    data->vector++;
                    break;
                case XED_CATEGORY_CMOV:
                    data->cmov++;
                    break;
                case XED_CATEGORY_MMX:
                case XED_CATEGORY_SSE:
                    data->mmx_sse++;
                    break;
                case XED_CATEGORY_SYSCALL:
                    data->syscall++;
                    break;
                case XED_CATEGORY_X87_ALU:
                    data->alu++;
                    break;
                default:
                    data->misc++;
            }
        }

        data->ins_cnt = BBL_NumIns(bbl);

        INS_InsertIfCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) terminate, IARG_END);
        INS_InsertThenCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) exit_routine, IARG_END);

        INS_InsertIfCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) ff_valid, IARG_END);
        INS_InsertThenPredicatedCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) analysis, IARG_UINT32, bbl_counter - 1, IARG_END);

        BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR) bbl_ins_count, IARG_UINT32, bbl_counter - 1, IARG_END);
    }
}

KNOB< string > KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "inscount.out", "specify output file name");
KNOB< string > KnobFastForward(KNOB_MODE_WRITEONCE, "pintool", "f", "0", "Skip over f billion instructions : default (0)");

// This function is called when the application exits
VOID Fini(INT32 code, VOID* v)
{
    // Write to a file since cout and cerr maybe closed by the application
    OutFile.setf(ios::showbase);
    OutFile << "Count " << icount << endl;
    OutFile.close();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char* argv[])
{
    memset(bbl_data, 0, sizeof bbl_data);
    memset(bbl_cnt, 0, sizeof bbl_cnt);

    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    OutFile.open(KnobOutputFile.Value().c_str());

    ff_cnt = std::stoi(KnobFastForward.Value()) * 1e9;

    // Register Instruction to be called to instrument instructions
    TRACE_AddInstrumentFunction(Trace, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}
