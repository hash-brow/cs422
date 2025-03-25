#include <iostream>
#include <fstream>
#include <set>
#include "pin.H"
#include <cstdlib>

/* Macro and type definitions */
#define BILLION 1000000000

using namespace std;

#include "include/predictor.h"
#include "include/btb.h"
#include "include/hashbtb.h"

/* Global variables */
//std::ostream * out = &cerr;
std::ofstream OutFile;

UINT64 insFootPrint = 0;
UINT64 memFootPrint = 0;

ADDRINT fastForwardDone = 0;
UINT64 icount = 0; //number of dynamically executed instructions

UINT64 fastForwardIns; // number of instruction to fast forward
UINT64 maxIns; // maximum number of instructions to simulate

/* Command line switches */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "", "specify file name for HW1 output");
KNOB<UINT64> KnobFastForward(KNOB_MODE_WRITEONCE, "pintool", "f", "0", "number of instructions to fast forward in billions");

/* Print out help message. */
INT32 Usage()
{
	cerr << "CS422 Homework 2" << endl;
	cerr << KNOB_BASE::StringKnobSummary() << endl;
	return -1;
}

VOID InsCount(UINT32 c)
{
	icount += c;
}

ADDRINT FastForward(void)
{
	return (icount >= fastForwardIns && icount < maxIns);
}

VOID FastForwardDone(void)
{
	fastForwardDone = 1;
}

ADDRINT IsFastForwardDone(void)
{
	return fastForwardDone;
}

ADDRINT Terminate(void)
{
        return (icount >= maxIns);
}

namespace Predictors {
        FNBT fnbt { };
        Bimodal bimodal { };
        GAg gag { };
        SAg sag { };
        Gshare gshare { };
        Hybrid hybrid { };
        Hybrid_majority hybrid_majority { };
        Hybrid_tournament hybrid_tournament { };
}

BTB btb;
HASHBTB hashbtb;

VOID StatDump(void)
{
	OutFile << "FNBT : " << Predictors::fnbt << "\n";
	OutFile << "Bimodal : " << Predictors::bimodal << "\n";
	OutFile << "SAg : " << Predictors::sag << "\n";
	OutFile << "GAg : " << Predictors::gag << "\n";
	OutFile << "gshare : " << Predictors::gshare << "\n";
	OutFile << "Combined2 : " << Predictors::hybrid << "\n";
	OutFile << "Combined3Majority : " << Predictors::hybrid_majority << "\n";
	OutFile << "Combined3 : " << Predictors::hybrid_tournament << "\n";
	
	OutFile << "BTB1 : "; btb.print(OutFile);
	OutFile << "BTB2 : "; hashbtb.print(OutFile);	
	
	exit(0);
}

void analysis(ADDRINT addr_ins, ADDRINT addr_branch, bool taken) {
	// std::cout << "analysis" << std::endl;

	data_t data { addr_ins, addr_branch, taken };
	result_t result { };

	Predictors::fnbt + data;
	Predictors::bimodal + data;

	result.GAg = Predictors::gag + data; 
	result.SAg = Predictors::sag + data;
	result.gshare = Predictors::gshare + data;

	data.result = result;
	Predictors::hybrid + data;
	Predictors::hybrid_majority + data;
	Predictors::hybrid_tournament + data;

	hashbtb.update_ghr(taken);
}

VOID indirect_flow(INS ins) {
	// OutFile << "indirect" << std::endl;
	
	UINT32 ins_size = INS_Size(ins);

	INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)IsFastForwardDone, IARG_END);
	INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR)BTB::btb_fill, IARG_PTR, &btb, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_BRANCH_TAKEN, IARG_UINT32, ins_size, IARG_END);

	INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)IsFastForwardDone, IARG_END);
	INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR)HASHBTB::btb_fill, IARG_PTR, &hashbtb, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_BRANCH_TAKEN, IARG_UINT32, ins_size, IARG_END);
}

/* Instruction instrumentation routine */
VOID Trace(TRACE trace, VOID *v)
{
	for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
		BBL_InsertIfCall(bbl, IPOINT_BEFORE, (AFUNPTR) Terminate, IARG_END);
        	BBL_InsertThenCall(bbl, IPOINT_BEFORE, (AFUNPTR) StatDump, IARG_END);

		BBL_InsertIfCall(bbl, IPOINT_BEFORE, (AFUNPTR) FastForward, IARG_END);
		BBL_InsertThenCall(bbl, IPOINT_BEFORE, (AFUNPTR) FastForwardDone, IARG_END);

		for (INS ins = BBL_InsHead(bbl); ; ins = INS_Next(ins)) {
            if (INS_IsBranch(ins) && INS_HasFallThrough(ins)) { 
				INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) IsFastForwardDone, IARG_END);
				INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR) analysis, 
								IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR,
								IARG_BRANCH_TAKEN, IARG_END);
			} else if (INS_IsIndirectControlFlow(ins)) {
				indirect_flow(ins);
			}

			if (ins == BBL_InsTail(bbl)) break;
		}

		/* Called for each BBL */
        	BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR) InsCount, IARG_UINT32, BBL_NumIns(bbl), IARG_END);
	}
}

/* Fini routine */
VOID Fini(INT32 code, VOID * v)
{
        assert(6 > 7);
}

int main(int argc, char *argv[])
{
	// Initialize PIN library. Print help message if -h(elp) is specified
	// in the command line or the command line is invalid 
	if (PIN_Init(argc, argv))
		return Usage();

	/* Set number of instructions to fast forward and simulate */
	fastForwardIns = KnobFastForward.Value() * BILLION;
	maxIns = fastForwardIns + BILLION;

        OutFile.open(KnobOutputFile.Value().c_str());

	// Register function to be called to instrument instructions
	TRACE_AddInstrumentFunction(Trace, 0);

	// Register function to be called when the application exits
	PIN_AddFiniFunction(Fini, 0);

	cerr << "===============================================" << endl;
	cerr << "This application is instrumented by HW1" << endl;
	if (!KnobOutputFile.Value().empty())
		cerr << "See file " << KnobOutputFile.Value() << " for analysis results" << endl;
	cerr << "===============================================" << endl;

	// Start the program, never returns
	PIN_StartProgram();

	return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */

