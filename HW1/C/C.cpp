/*
 * Copyright (C) 2004-2021 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <fstream>
#include "pin.H"
using std::cerr;
using std::endl;
using std::ios;
using std::ofstream;
using std::string;

ofstream OutFile;

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 icount = 0;
static UINT64 fast_forward_count = 0;
static UINT64 repcount = 0;
static UINT64 count2 = 0;

// This function is called before every instruction is executed
VOID insCount() { icount++; }

ADDRINT terminate() {
	return (icount >= fast_forward_count + 1000000000);
}

ADDRINT fastForward() {
	return (icount >= fast_forward_count && icount);
}

void myExitRoutine() {
	OutFile.setf(ios::showbase);
	OutFile << "Count " << icount << endl;
	OutFile << "Reported Count " << repcount << endl;
	OutFile << "Second Call " << count2 << endl;
	OutFile.close();
	
	exit(0);
}

//void myPredicatedAnalysis(...){
	//TODO
//}
//

void analysis() {
	repcount++;
}

void call2() {
	count2++;
}

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID* v)
{
	INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)insCount, IARG_END);
	INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)terminate, IARG_END);
	INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR)myExitRoutine, IARG_END);
	INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)fastForward, IARG_END);
	INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis, IARG_END);	
	INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)fastForward, IARG_END);
	INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR)call2, IARG_END);
	// INS_InsertThenPredicatedCall(ins, IPOINT_BEFORE, myPredicatedAnalysis, ..., IARG_END);
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
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char* argv[])
{
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    OutFile.open(KnobOutputFile.Value().c_str());
    fast_forward_count = (UINT64) std::stoi(KnobFastForward.Value()) * (UINT64)1000000000;

    OutFile << fast_forward_count << endl;

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}

