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
#include <set>
#include "pin.H"

using std::cerr;
using std::endl;
using std::ios;
using std::ofstream;
using std::string;

#include "paging.h"
#include "utils.h"

// NOTE: comment this for final timing runs
// #define ASSERTS 

ofstream OutFile;

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

#define BBL_MAX 1000000
static bbl_val bbl_data[BBL_MAX];
static UINT64 bbl_cnt[BBL_MAX]; // how many times has each BBL been run

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 icount = 0;
static UINT64 ff_cnt;

static UINT64 cmov_cnt = 0;
static UINT64 cset_cnt = 0;

VOID bbl_ins_count(INT32 idx) {
    icount += bbl_data[idx].ins_cnt;
}

// check if we are doing with fast forwarding
const UINT64 range_len = 1e9;
static uint32_t ff_flag = 0;
ADDRINT ff_valid() {
    return (ff_flag = (icount >= ff_cnt));
}

ADDRINT terminate() {
    return icount >= ff_cnt + range_len;
}

// analysis lessgoo
VOID analysis(INT32 idx) {
    bbl_cnt[idx]++;
}

// returns the argument to be passed
// for use in InsertIf, should ideally be inlined by PIN
ADDRINT return_arg(uint32_t arg) {
    return arg;
}

// a nice multiple of PG_SZ * PG_TABLE_SZ pls
//#define MAX_MEM (1 << 27)

// 16 MB
#define MEM_CHUNK (1 << 24)
uint64_t PT_CHUNK = (MEM_CHUNK / PG_SZ);
uint64_t PG_CHUNK = (MEM_CHUNK / GRAN);
#define BUF (1 << 10) // 1 KB


// Reallocs are fairly costly, and can probably be optimized further by only
// using mallocs and some slightly diff. jugaadu structure
// However, perlbench runs in only 5 mins so is the effort even worth it?
// Maybe for ones like xalancbmk (or whatever) which take up a lot of memory, yeah
//
// I'm implementing some diff. jugaad rn, I'm just gonna double PT_CHUNK
// and PG_CHUNK every time I use them
// That way more the memory which is being used, bigger the chunks
// so smaller no. of calls to realloc (yay?)

static pde_t pgdir[PG_DIR_SZ];
static pte_t *ptables;
static bool *pages; 

static uint32_t ptables_sz = 0; 
static uint32_t pages_sz = 0;

static uint32_t ptable_counter = 0;
static uint32_t pg_counter = 0;

// returns 1 on success
// yeah yeah, the name I know xD
static inline __attribute__((always_inline)) ADDRINT pgdir_addr_exists_not(ADDRINT addr) {
    return ((pgdir[PDX(addr)] & PDE_P) == 0);
}

static inline __attribute__((always_inline)) ADDRINT ptable_addr_exists_not(ADDRINT addr) {
    pte_t pte = pgdir[PDX(addr)] & ~(PDE_P);
    return ((ptables[pte + PTX(addr)] & PTE_P) == 0);
}

static inline __attribute__((always_inline)) void pgdir_addr_create(ADDRINT addr) {
    if (ptable_counter >= ptables_sz) {
        ptables_sz += PT_CHUNK;
        ptables = (pte_t*)realloc(ptables, (ptables_sz * sizeof(pte_t)) + BUF);
        memset(ptables + ptables_sz - PT_CHUNK, 0, (PT_CHUNK * sizeof(pte_t)) + BUF);
        PT_CHUNK *= 2;
    }

    pgdir[PDX(addr)] = ptable_counter | PDE_P;
    ptable_counter += PG_TABLE_SZ;
}

static inline __attribute__((always_inline)) void ptable_addr_create(ADDRINT addr) {
    if (pg_counter >= pages_sz) {
        pages_sz += PG_CHUNK;
        pages = (bool*)realloc(pages, (pages_sz * sizeof(bool)) + BUF);
        memset(pages + pages_sz - PG_CHUNK, 0, (PG_CHUNK * sizeof(bool)) + BUF);
        PG_CHUNK *= 2;
    }

    pte_t pte = pgdir[PDX(addr)] & ~(PDE_P);
    ptables[pte + PTX(addr)] = pg_counter | PTE_P;
    pg_counter += (PG_SZ / GRAN);
}

static inline __attribute__((always_inline)) void touch(ADDRINT addr, uint32_t sz) {
    ADDRINT saddr = PGDOWN(addr);
    ADDRINT eaddr = PGDOWN(addr + sz - 1); 

    for (ADDRINT i = saddr; i <= eaddr; i += GRAN) {
        if (pgdir_addr_exists_not(i)) {
            pgdir_addr_create(i);
            ptable_addr_create(i);
        } else if (ptable_addr_exists_not(i)) {
            ptable_addr_create(i);
        }

        pte_t pte = pgdir[PDX(i)] & ~(PDE_P);
        uint32_t pg = ptables[pte + PTX(i)] & ~(PTE_P);
        pages[pg + PGX(i)] = 1;
    }


}

static inline __attribute__((always_inline)) void mem_analysis(INT32 idx, bbl_val* data, UINT32 inst_memOp, UINT32 inst_memRead, UINT32 inst_memWrite, UINT32 inst_memBytes, ADDRINT max_disp , ADDRINT min_disp) {
    data->mem_ops[std::min(inst_memOp, (UINT32)9)]++;
    data->mem_reads[std::min(inst_memRead, (UINT32)9)]++;
    data->mem_writes[std::min(inst_memWrite,(UINT32) 9)]++;
    data->total_mem_bytes += inst_memBytes;
    if(inst_memBytes)data->mem_instr_count++;
    if(inst_memBytes > data->max_mem_bytes) data->max_mem_bytes = inst_memBytes;
    data->max_disp = std::max(data->max_disp, static_cast<ADDRDELTA>(max_disp));
    data->min_disp = std::min(data->min_disp, static_cast<ADDRDELTA>(min_disp));

}

VOID exit_routine() {
    bbl_val results;
    memset(&results.ins_cnt, 0, sizeof(results) - sizeof(std::vector<uint32_t>));

    INT64 ins_tot = 0;

    std::set<uint32_t> instr_accesses;

    for (UINT32 i = 0; i < bbl_counter; i++) {
        if (bbl_cnt[i]) {
            for (auto &addr: bbl_data[i].static_addr) instr_accesses.insert(addr);
        }

        accumulate_bbl_val(&results, &bbl_data[i], bbl_cnt[i]);
        ins_tot += bbl_data[i].ins_cnt;
    }
    results.cmov += cmov_cnt;
    results.misc += cset_cnt;
    print_bbl_val(static_cast<std::ostream&>(OutFile), results);

    OutFile << "BBL counter: " << bbl_counter << "\n"; 
    OutFile << "Mean BBL sz (ins): " << static_cast<double>(ins_tot/bbl_counter) << "\n";
    OutFile << "CPI: " << calc_cpi(results) << "\n";

    uint32_t data_accesses = 0;
    for (uint32_t i = 0; i < pages_sz; i++) 
        data_accesses += pages[i];

    OutFile << "Data Chunk Accesses: " << data_accesses << "\n";
    OutFile << "Instr Chunk Accesses: " << instr_accesses.size() << "\n";

    exit(0);
}

VOID increment_cmov() { cmov_cnt++; }
VOID increment_setcc() { cset_cnt++; }

// Pin calls this function every time a new basic block is encountered
VOID Trace(TRACE trace, VOID* v)
{
    // TODO:
    // can a mem op be both load & store? I think so, for eg. inc 0x69
    // but in Part A Type B I have put an ELSE?
    // So thats a bug? But counts still match with the reference given??? 

    // Visit every basic block  in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        INS_InsertIfCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) terminate, IARG_END);
        INS_InsertThenCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) exit_routine, IARG_END);

        bbl_val* data = &bbl_data[bbl_counter];
        bbl_counter++;

#ifdef ASSERTS
        assert(bbl_counter < BBL_MAX);
#endif

        // your avg instruction is prolly aligned to word sz, so this
        // resize should be good enough in like >90% of cases?
        data->static_addr.resize((BBL_Size(bbl) >> 5) + 1);

        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)) {
            // Part D stuff
            UINT32 size = INS_Size(ins);
            if(size < 20) data->instr_length[size]++;

            UINT32 opCount = INS_OperandCount(ins);
            if(opCount < 10) data->op_count[opCount]++;

            UINT32 rRegs = INS_MaxNumRRegs(ins);
            if(rRegs < 10) data->reg_reads[rRegs]++;

            UINT32 wRegs = INS_MaxNumWRegs(ins);
            if(wRegs < 10) data->reg_writes[wRegs]++;

            UINT32 mcount = INS_MemoryOperandCount(ins);
            UINT32 inst_memOp = 0;
            UINT32 inst_memRead = 0;
            UINT32 inst_memWrite = 0;
            UINT32 inst_memBytes = 0;
            ADDRDELTA max_disp = INT32_MIN;
            ADDRDELTA min_disp = INT32_MAX;

            for (UINT32 i = 0; i < mcount; i++) {
                bool isRead = INS_MemoryOperandIsRead(ins, i);
                bool isWrite = INS_MemoryOperandIsWritten(ins, i);
                UINT32 sz = INS_MemoryOperandSize(ins, i);
                if (isRead) {
                    inst_memRead++;
                    inst_memOp++;
                }
                if (isWrite) {
                    inst_memWrite++;
                    inst_memOp++;
                }
                if (isRead || isWrite){
                    inst_memBytes += sz;
                }

                ADDRDELTA disp = INS_OperandMemoryDisplacement(ins, i);
                max_disp = std::max(max_disp, disp);
                min_disp = std::min(min_disp, disp);
            }

            for(UINT32 i = 0; i < opCount; i++){
                if(INS_OperandIsImmediate(ins,i)){
                    INT32 imm = static_cast<INT32>(INS_OperandImmediate(ins, i));
                    if(!data->found_imm){
                        data->max_imm = imm;
                        data->min_imm = imm;
                        data->found_imm = true;
                    } else {
                        data->max_imm = std::max(data->max_imm, imm);
                        data->min_imm = std::min(data->min_imm, imm);
                    }
                }
            }
            
            if(mcount){
                INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)ff_valid, IARG_END);
                INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)mem_analysis, IARG_UINT32,bbl_counter-1, IARG_PTR, data, IARG_UINT32, inst_memOp, IARG_UINT32, inst_memRead, IARG_UINT32, inst_memWrite, IARG_UINT32, inst_memBytes, IARG_ADDRINT, max_disp , IARG_ADDRINT, min_disp, IARG_END);
            }

            // Part C stuff
            ADDRINT start_addr = INS_Address(ins);
            ADDRINT end_addr = start_addr + INS_Size(ins) - 1;
            start_addr >>= 5; end_addr >>= 5;

            for (ADDRINT chnk = start_addr; chnk <= end_addr; chnk++)  
                data->static_addr.push_back(chnk);

            // Part A Type B stuff
            UINT32 mem_op_cnt = INS_MemoryOperandCount(ins);
            for (UINT32 mem_op = 0; mem_op < mem_op_cnt; ++mem_op) {
                if (INS_MemoryOperandIsRead(ins, mem_op)) {
                    data->load += (INS_MemoryOperandSize(ins, mem_op) + 3) / 4;
                } 
                if (INS_MemoryOperandIsWritten(ins, mem_op)) {
                    data->store += (INS_MemoryOperandSize(ins, mem_op) + 3) / 4;
                }
            }

            if (INS_Category(ins) == XED_CATEGORY_CMOV) {
                INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) ff_valid, IARG_END);
                INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) increment_cmov, IARG_END);
                continue;
            }

            if (INS_Category(ins) == XED_CATEGORY_SETCC) {
                INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) ff_valid, IARG_END);
                INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) increment_setcc, IARG_END);
                continue;
            }

            // Part A Type A stuff
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

        INS_InsertIfCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) ff_valid, IARG_END);
        INS_InsertThenCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) analysis, IARG_UINT32, bbl_counter - 1, IARG_END);

        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)) {
            UINT32 mem_op_cnt = INS_MemoryOperandCount(ins);

            for (UINT32 j = 0; j < mem_op_cnt; ++j) {
                // TODO: replace this if with a call to return_arg(ff_flag)
                // issue is race conditions at first ins of bbl
                INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) ff_valid, IARG_END);
                INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) touch, IARG_MEMORYOP_EA, j, IARG_MEMORYOP_SIZE, j, IARG_END);
            }
        }

        BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR) bbl_ins_count, IARG_UINT32, bbl_counter - 1, IARG_END);
    }
}

KNOB< string > KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "inscount.out", "specify output file name");
KNOB< string > KnobFastForward(KNOB_MODE_WRITEONCE, "pintool", "f", "0", "Skip over f billion instructions : default (0)");

// This function is called when the application exits
VOID Fini(INT32 code, VOID* v)
{
    OutFile << "WARNING: Fini called!" << "\n";
    exit_routine();
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
    for (int i = 0; i < BBL_MAX; ++i) {
        memset(&bbl_data[i].ins_cnt, 0, sizeof(bbl_data[i]) - sizeof(std::vector<uint32_t>));
    }

    memset(bbl_cnt, 0, sizeof bbl_cnt);

    memset(pgdir, 0, sizeof pgdir);

    ptables = (pte_t*)malloc((sizeof(pte_t) * PT_CHUNK) + BUF); 
    memset(ptables, 0, (sizeof(pte_t) * PT_CHUNK) + BUF);
    ptables_sz += PT_CHUNK;

    pages = (bool*)malloc((sizeof(bool) * PG_CHUNK) + BUF);
    memset(pages, 0, (sizeof(bool) * PG_CHUNK) + BUF);
    pages_sz += PG_CHUNK;

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
