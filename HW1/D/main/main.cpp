/*
 * main_bb.cpp
 *
 * CS422 2024-25 II Assignment 1
 * PIN Tool for Part D (ISA statistics) using Basic Block instrumentation
 *

 */
#include "pin.H"
#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <vector>
#include <limits>
#include <iomanip>
#include <climits>
 
 using std::ofstream;
 using std::cerr;
 using std::endl;
 using std::ios;
 using std::vector;
 
#define MY_MAX(a, b) ((a) > (b) ? (INT32)(a) : (INT32)(b))
#define MY_MIN(a, b) ((a) < (b) ? (INT32)(a) : (INT32)(b))

ofstream OutFile;
 
 typedef struct _bbl_val {
    UINT64 ins_cnt=0;
    UINT64 memOps=0;
    UINT64 instr_length[16] = {0};  
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
    bool found_disp = false;
} bbl_val;
 
static bbl_val stats;
static UINT32 bbl_counter =0;
static UINT64 icount = 0;
static UINT64 ff_cnt = 0;
#define BBL_MAX 1000000
static bbl_val bbl_data[BBL_MAX];
static UINT64 bbl_cnt[BBL_MAX];
static UINT64 bbl_cnt_mem[BBL_MAX];

KNOB<UINT64> KnobFastForward(KNOB_MODE_WRITEONCE, "pintool", "f", "0", "Skip over f billion instructions");
KNOB<std::string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "test.out", "Output file");

 VOID UpdateCounters(const bbl_val* src, UINT64 mul, UINT64 mem_mul)
 {
     
     stats.ins_cnt += src->ins_cnt*mul;
     
     for(UINT32 i=0;i<16;i++){
        stats.instr_length[i] += src->instr_length[i]*mul;
     }

     for(UINT32 i=0;i<10;i++){
        stats.op_count[i] += src->op_count[i]*mul;
     }
     
     for(UINT32 i=0;i<10;i++){
        stats.reg_reads[i] += src->reg_reads[i]*mul;
     }

     for(UINT32 i=0;i<10;i++){
        stats.reg_writes[i] += src->reg_writes[i]*mul; 
     }
     for(UINT32 i=0;i<10;i++){
        stats.mem_ops[i] += src->mem_ops[i]*mem_mul; 
     }

     for(UINT32 i=0;i<10;i++){
        stats.mem_reads[i] += src->mem_reads[i]*mem_mul; 
     }
     
     for(UINT32 i=0;i<10;i++){
        stats.mem_writes[i] += src->mem_writes[i]*mem_mul; 
     }
     
     if (src->memOps > 0) {
         stats.total_mem_bytes += src->total_mem_bytes*mem_mul;
         stats.mem_instr_count += src->mem_instr_count*mem_mul;
         stats.max_mem_bytes = MY_MAX(stats.max_mem_bytes, src->max_mem_bytes);
     }

     if (src->found_imm) {
         if (!stats.found_imm) {
             stats.max_imm = src->max_imm;
             stats.min_imm = src->min_imm;
             stats.found_imm = true;
         } else {
             stats.max_imm = MY_MAX(stats.max_imm, src->max_imm);
             stats.min_imm = MY_MIN(stats.min_imm, src->min_imm);
         }
     }

     if (src->found_disp) {
         if (!stats.found_disp) {
             stats.max_disp = src->max_disp;
             stats.min_disp = src->min_disp;
             stats.found_disp = true;
         } else {
             stats.max_disp = MY_MAX(stats.max_disp, src->max_disp);
             stats.min_disp = MY_MIN(stats.min_disp, src->min_disp);
         }
     }
 }

 VOID analysis(INT32 idx) {
    bbl_cnt[idx]++;
}

VOID mem_analysis(INT32 idx) {
    bbl_cnt_mem[idx]++;
}

VOID bbl_ins_count(INT32 idx) {
    icount += bbl_data[idx].ins_cnt;
}

ADDRINT ff_valid() {
    return (icount >= ff_cnt) && (icount <= ff_cnt+1000000000);
}

ADDRINT Terminate() {
    return icount > ff_cnt + 1000000000ULL;
}

VOID Fini(INT32 code, VOID *v) {
    //OutFile.open(KnobOutputFile.Value().c_str());
    OutFile << "1. Instruction Length Distribution:\n";
    for(int i = 0; i < 16; i++) {
        OutFile << i << " bytes: " << stats.instr_length[i] << "\n";
    }
    OutFile << "\n2. Operands per Instruction:\n";
    for(int i = 0; i < 10; i++) {
        OutFile << i << " operands: " << stats.op_count[i] << "\n";
    }
    OutFile << "\n3. Register Read Operands:\n";
    for(int i = 0; i < 10; i++) {
        OutFile << i << " reads: " << stats.reg_reads[i] << "\n";
    }
    OutFile << "\n4. Register Write Operands:\n";
    for(int i = 0; i < 10; i++) {
        OutFile << i << " writes: " << stats.reg_writes[i] << "\n";
    }
    OutFile << "\n5. Memory Operands per Instruction:\n";
    for(int i = 0; i < 10; i++) {
        if(i==0)OutFile << i << " operands: " << stats.mem_ops[i]+stats.ins_cnt-stats.mem_instr_count << "\n";
        else OutFile << i << " operands: " << stats.mem_ops[i] << "\n";
    }
    OutFile << "\n6. Memory Read Operands:\n";
    for(int i = 0; i < 10; i++) {
        if(i==0)OutFile << i << " operands: " << stats.mem_reads[i]+stats.ins_cnt-stats.mem_instr_count << "\n";
        else OutFile << i << " operands: " << stats.mem_reads[i] << "\n";
    }
    OutFile << "\n7. Memory Write Operands:\n";
    for(int i = 0; i < 10; i++) {
        if(i==0)OutFile << i << " operands: " << stats.mem_writes[i]+stats.ins_cnt-stats.mem_instr_count << "\n";
        else OutFile << i << " operands: " << stats.mem_writes[i] << "\n";
    }
    OutFile << "\n8. Memory Bytes Touched:\n";
    OutFile << "Max: " << stats.max_mem_bytes << " bytes\n";
    if (stats.mem_instr_count > 0) {

        double avgMemBytes = static_cast<double>(stats.total_mem_bytes) / stats.mem_instr_count;
        OutFile << "Average: " << std::fixed << std::setprecision(5) 
                << avgMemBytes << " bytes\n";
    } else {
        OutFile << "Average: 0.00 bytes\n";
    }
    OutFile << "\n9. Immediate Values:\n";
    OutFile << "Max: " << (stats.found_imm ? stats.max_imm : 0) << "\n";
    OutFile << "Min: " << (stats.found_imm ? stats.min_imm : 0) << "\n";
    OutFile << "\n10. Displacement Values:\n";
    OutFile << "Max: " << (stats.found_disp ? stats.max_disp : 0) << "\n";
    OutFile << "Min: " << (stats.found_disp ? stats.min_disp : 0) << "\n";
    
}

VOID ExitRoutine(VOID* v) {
    //stats = {};
   INT64 ins_tot = 0;

   for (UINT32 i = 0; i < bbl_counter; i++) {
       UpdateCounters(&bbl_data[i], bbl_cnt[i] , bbl_cnt_mem[i]);
       ins_tot += bbl_data[i].ins_cnt;
   }
   Fini(0,v);
   OutFile << "BBL counter: " << bbl_counter << "\n"; 
   OutFile << "Mean BBL sz (ins): " << static_cast<double>(ins_tot/bbl_counter) << "\n";
   
   OutFile.close();
   
    exit(0);
}


 VOID Trace(TRACE trace, VOID* v){

    for(BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl=BBL_Next(bbl)){
        bbl_val* data= &bbl_data[bbl_counter];
        bbl_counter++;
        assert(bbl_counter<BBL_MAX);

        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)) {
            UINT32 size = INS_Size(ins);
            data->ins_cnt++;
            if (size < 16)
            data->instr_length[size]++;
            
            UINT32 opCount = INS_OperandCount(ins);
            if (opCount < 10)
               data->op_count[opCount]++;
            
            UINT32 rRegs = INS_MaxNumRRegs(ins);
            if (rRegs < 10)
               data->reg_reads[rRegs]++;
            
            UINT32 wRegs = INS_MaxNumWRegs(ins);
            if (wRegs < 10)
               data->reg_writes[wRegs]++;
            
            UINT32 mcount = INS_MemoryOperandCount(ins);
            UINT32 inst_memOp = 0;
            UINT32 inst_memRead = 0;
            UINT32 inst_memWrite = 0;
            UINT32 inst_memBytes = 0;
            for (UINT32 i = 0; i < mcount; i++) {
                bool isRead = INS_MemoryOperandIsRead(ins, i);
                bool isWrite = INS_MemoryOperandIsWritten(ins, i);
                UINT32 sz = INS_MemoryOperandSize(ins, i);
                if (isRead) {
                    inst_memRead++;
                    inst_memBytes += sz;
                }
                if (isWrite) {
                    inst_memWrite++;
                    inst_memBytes += sz;
                }
                if (isRead && isWrite)inst_memOp += 2;
                else if (isRead || isWrite)inst_memOp += 1;

                ADDRDELTA disp = INS_OperandMemoryDisplacement(ins, i);
                if(disp>0)data->found_disp = true;
                data->max_disp = MY_MAX(data->max_disp, disp);
                data->min_disp = MY_MIN(data->min_disp, disp);
            }
            if (data->memOps > 0) {
                data->mem_ops[MY_MIN(inst_memOp,9)]++;
                data->mem_reads[MY_MIN(inst_memRead,9)]++;
                data->mem_writes[MY_MIN(inst_memWrite,9)]++;
                data->total_mem_bytes += inst_memBytes;
                data->max_mem_bytes = MY_MAX(data->max_mem_bytes,inst_memBytes);
            }
   
            for (UINT32 i = 0; i < opCount; i++) {
                if (INS_OperandIsImmediate(ins, i)) {
                    INT32 imm = static_cast<INT32>(INS_OperandImmediate(ins, i));
                    if(!data->found_imm) {
                        data->found_imm = true;
                        data->max_imm = data->min_imm = imm;
                    }
                    else{
                        data->max_imm = MY_MAX(data->max_imm, imm);
                        data->min_imm = MY_MIN(data->min_imm, imm);
                    }
                }
            }
        }

        INS_InsertIfCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) Terminate, IARG_END);
        INS_InsertThenCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR)ExitRoutine, IARG_END);

        INS_InsertIfCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) ff_valid, IARG_END);
        INS_InsertThenCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) analysis, IARG_UINT32, bbl_counter - 1, IARG_END);

        INS_InsertIfCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) ff_valid, IARG_END);
        INS_InsertThenPredicatedCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR) mem_analysis, IARG_UINT32, bbl_counter - 1, IARG_END);

        BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR) bbl_ins_count, IARG_UINT32, bbl_counter - 1, IARG_END);
    }

}
 
 INT32 Usage()
{
    cerr << "This tool analyses the nature of each instruction being executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

 int main(int argc, char *argv[]) {

    //bbl_data[BBL_MAX] = {};  
    memset(bbl_cnt, 0, sizeof bbl_cnt);
    memset(bbl_cnt_mem, 0, sizeof bbl_cnt_mem);
    if (PIN_Init(argc, argv)) return Usage();

    OutFile.open(KnobOutputFile.Value().c_str());
    ff_cnt = KnobFastForward.Value() * 1e9;

    TRACE_AddInstrumentFunction(Trace, 0);
    PIN_AddFiniFunction(Fini, 0);
    PIN_StartProgram();
    return 0;
 }
 