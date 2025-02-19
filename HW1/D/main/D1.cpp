/*
 * main.cpp
 *
 * CS422 2024-25 II Assignment 1
 * Pintool file for Part D with Fast Forwarding
 *
 * Created on Feb 16, 2025
 */
 #include <iostream>
 #include <fstream>
 #include <iomanip>
 #include <assert.h>
 #include <vector>
 #define MAX(a, b) ((a) > (b) ? (INT32)(a) : (INT32)(b))
 #define MIN(a, b) ((a) < (b) ? (INT32)(a) : (INT32)(b))

 #include "pin.H"
 
 using std::cerr;
 using std::endl;
 using std::ios;
 using std::ofstream;
 using std::string;
 ofstream OutFile;
 
 typedef struct _bbl_val {
     UINT64 ins_cnt=0;
     UINT64 memOps=0;
     UINT64 instr_length[16] = {0};  
     UINT64 operand_count[10] = {0};  
     UINT64 reg_reads[10] = {0};      
     UINT64 reg_writes[10] = {0};     
     UINT64 mem_operands[10] = {0};   
     UINT64 mem_reads[10] = {0};      
     UINT64 mem_writes[10] = {0};     
     UINT64 max_mem_bytes = 0, total_mem_bytes = 0, mem_instr_count = 0;
     INT32 max_immediate = INT32_MIN, min_immediate = INT32_MAX;
     ADDRDELTA max_displacement = INT32_MIN, min_displacement = INT32_MAX; 
     bool found_immediate = false;
     bool found_displacement = false;
 } bbl_val;
 
static bbl_val stats;
static UINT64 icount = 0;
static UINT64 ff_cnt = 0;
KNOB<UINT64> KnobFastForward(KNOB_MODE_WRITEONCE, "pintool", "f", "0", "Fast forward in billions");
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "isa_analysis.out", "Output file");

VOID Fini(INT32 code, VOID *v);
VOID InstrumentGeneric(INS ins);
VOID Generic_Wrapper(INS ins);
VOID AnalyzeGenericInstruction(UINT32 instr_length, UINT32 operands, 
                              UINT32 reg_reads, UINT32 reg_writes);
VOID InstrumentMemOps(INS ins);
VOID MemOps_Wrapper(INS ins);
VOID AnalyzeMemoryInstruction(UINT32 memOpCnt, UINT32 readCnt,
    UINT32 writeCnt,UINT32 total_bytes,
   ADDRDELTA* displacements, UINT32 disp_count);
VOID AnalyzeImmediate(INT32* immediates, INT32 imm_cnt);
    
 ADDRINT ff_valid() {
     return (icount >= ff_cnt) && (icount <= ff_cnt+1000000000);
 }
 VOID CountInstruction() {
     icount++; 
 }
 ADDRINT Terminate() {
    return icount > ff_cnt + 1000000000;
}
 VOID ExitRoutine(VOID* v) {
    Fini(0, v);
    exit(0);
}
 // Analysis routine for 1,2,3,4,9

 VOID InstrumentGeneric(INS ins){
    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)ff_valid, IARG_END);   
    INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR)Generic_Wrapper, IARG_PTR, ins, IARG_END);   
 }
 VOID Generic_Wrapper(INS ins) {
    UINT32 size = INS_Size(ins);
    UINT32 opCount = INS_OperandCount(ins);
    UINT32 rRegs = INS_MaxNumRRegs(ins);
    UINT32 wRegs = INS_MaxNumWRegs(ins);
    AnalyzeGenericInstruction(size, opCount, rRegs, wRegs);
    
    std::vector<INT32> immediates;
    for(UINT32 i = 0; i < INS_OperandCount(ins); i++) {
        if(INS_OperandIsImmediate(ins, i)) {
            immediates.push_back(INS_OperandImmediate(ins, i));
        }
    }
    AnalyzeImmediate(&immediates[0], static_cast<INT32>(immediates.size()));

}

INS_Size VOID AnalyzeGenericInstruction(UINT32 instr_length, UINT32 operands, 
    UINT32 reg_reads, UINT32 reg_writes) {
     stats.ins_cnt++;
    if (instr_length < 16) stats.instr_length[instr_length]++;
    if (operands < 10) stats.operand_count[operands]++;
    if (reg_reads < 10) stats.reg_reads[reg_reads]++;
    if (reg_writes < 10) stats.reg_writes[reg_writes]++;

 }

 VOID AnalyzeImmediate(INT32* immediates, INT32 imm_cnt) {
    for(INT32 i=0;i<imm_cnt;i++){
        if(!stats.found_immediate) {
            stats.max_immediate = stats.min_immediate = immediates[i];
            stats.found_immediate = true;
        } else {
            stats.max_immediate = MAX(stats.max_immediate, immediates[i]);
            stats.min_immediate = MIN(stats.min_immediate, immediates[i]);
        }
    }
}
 
 // Analysis routine for 5,6,7,8, and 10
 VOID InstrumentMemOps(INS ins) {
    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)ff_valid, IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR)MemOps_Wrapper, IARG_PTR, ins, IARG_END);
 }
 VOID MemOps_Wrapper(INS ins) {
    //if (!INS_IsMemoryRead(ins) && !INS_IsMemoryWrite(ins)) return;

    UINT32 opCount = INS_MemoryOperandCount(ins);
    UINT32 memOpCnt =0;
    UINT32 readCnt=0;
    UINT32 writeCnt=0;
    UINT32 totalBytes = 0;
    std::vector<ADDRDELTA> displacements;
    
    for(UINT32 i=0; i<opCount; i++) {
        bool isRead = INS_MemoryOperandIsRead(ins,i);
        bool isWrite = INS_MemoryOperandIsWritten(ins,i);

        if(isRead){
            readCnt++;
            totalBytes += INS_MemoryOperandSize(ins,i);
            memOpCnt++;
        }
        if(isWrite){
            writeCnt++;
            memOpCnt++;
            totalBytes+=INS_MemoryOperandSize(ins,i);
        }
        displacements.push_back(INS_OperandMemoryDisplacement(ins, i));

    }

    AnalyzeMemoryInstruction(memOpCnt, readCnt, writeCnt, totalBytes, displacements.data(), static_cast<UINT32>(displacements.size()));
}
VOID AnalyzeMemoryInstruction(UINT32 memOpCnt, UINT32 readCnt,
                                UINT32 writeCnt,UINT32 total_bytes,
                               ADDRDELTA* displacements, UINT32 disp_count) {
    stats.memOps++;
    stats.mem_operands[MIN(memOpCnt, 9)]++;
    stats.mem_reads[MIN(readCnt,9)]++;
    stats.mem_writes[MIN(writeCnt,9)]++;
    if(memOpCnt > 0) {
        stats.total_mem_bytes += total_bytes;
        stats.mem_instr_count++;
        stats.max_mem_bytes = MAX(stats.max_mem_bytes, total_bytes);
    }
    for(UINT32 i = 0; i < disp_count; i++) {
        ADDRDELTA disp = displacements[i];
        if(!stats.found_displacement) {
            stats.max_displacement = stats.min_displacement = disp;
            stats.found_displacement = true;
        } else {
            stats.max_displacement = MAX(stats.max_displacement, disp);
            stats.min_displacement = MIN(stats.min_displacement, disp);
        }
    }
}

VOID Fini(INT32 code, VOID *v) {
    OutFile.open(KnobOutputFile.Value().c_str());
    OutFile << "1. Instruction Length Distribution:\n";
    for(int i = 0; i < 16; i++) {
        OutFile << i << " bytes: " << stats.instr_length[i] << "\n";
    }
    OutFile << "\n2. Operands per Instruction:\n";
    for(int i = 0; i < 10; i++) {
        OutFile << i << " operands: " << stats.operand_count[i] << "\n";
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
        if(i==0)OutFile << i << " operands: " << stats.mem_operands[i]+stats.ins_cnt-stats.memOps << "\n";
        else OutFile << i << " operands: " << stats.mem_operands[i] << "\n";
    }
    OutFile << "\n6. Memory Read Operands:\n";
    for(int i = 0; i < 10; i++) {
        if(i==0)OutFile << i << " operands: " << stats.mem_reads[i]+stats.ins_cnt-stats.memOps << "\n";
        else OutFile << i << " operands: " << stats.mem_reads[i] << "\n";
    }
    OutFile << "\n7. Memory Write Operands:\n";
    for(int i = 0; i < 10; i++) {
        if(i==0)OutFile << i << " operands: " << stats.mem_writes[i]+stats.ins_cnt-stats.memOps << "\n";
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
    OutFile << "Max: " << (stats.found_immediate ? stats.max_immediate : 0) << "\n";
    OutFile << "Min: " << (stats.found_immediate ? stats.min_immediate : 0) << "\n";
    OutFile << "\n10. Displacement Values:\n";
    OutFile << "Max: " << (stats.found_displacement ? stats.max_displacement : 0) << "\n";
    OutFile << "Min: " << (stats.found_displacement ? stats.min_displacement : 0) << "\n";
    OutFile.close();
}
 int main(int argc, char *argv[]) {
    PIN_Init(argc, argv);
    ff_cnt = KnobFastForward.Value() * 1000000000ULL;  

    INS_AddInstrumentFunction([](INS ins, VOID* v) {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)CountInstruction, IARG_END);
    }, 0);

    INS_AddInstrumentFunction([](INS ins, VOID* v) {
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)Terminate, IARG_END);
        INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR)ExitRoutine, IARG_END);
    }, 0);
 
    INS_AddInstrumentFunction([](INS ins, VOID* v) {
        InstrumentGeneric(ins);
        if (INS_IsMemoryRead(ins) || INS_IsMemoryWrite(ins))InstrumentMemOps(ins);
    }, 0);    
    PIN_AddFiniFunction(Fini, 0);
    PIN_StartProgram();
    return 0;
 }
 