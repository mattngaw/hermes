/*
 * Copyright (C) 2009-2021 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

// https://software.intel.com/sites/landingpage/pintool/docs/98484/Pin/html/index.html#MallocMT
 
#include <stdio.h>
#include <stdio.h>
#include "pin.H"

using std::string;
 
KNOB< string > KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "../traces/sum_parallel.trace", "specify output file name");
 
//==============================================================
//  Analysis Routines
//==============================================================
// Note:  threadid+1 is used as an argument to the PIN_GetLock()
//        routine as a debugging aid.  This is the value that
//        the lock is set to, so it must be non-zero.
 
// lock serializes access to the output file.
FILE* out;
PIN_LOCK pinLock;
 
// Note that opening a file in a callback is only supported on Linux systems.
// See buffer-win.cpp for how to work around this issue on Windows.
//
// This routine is executed every time a thread is created.
VOID ThreadStart(THREADID threadid, CONTEXT* ctxt, INT32 flags, VOID* v)
{
    PIN_GetLock(&pinLock, threadid + 1);
    fprintf(out, "t%d begin\n", threadid);
    fflush(out);
    PIN_ReleaseLock(&pinLock);
}
 
// This routine is executed every time a thread is destroyed.
VOID ThreadFini(THREADID threadid, const CONTEXT* ctxt, INT32 code, VOID* v)
{
    PIN_GetLock(&pinLock, threadid + 1);
    fprintf(out, "t%d end code %d\n", threadid, code);
    fflush(out);
    PIN_ReleaseLock(&pinLock);
}
 
VOID LoadSingle(ADDRINT addr, THREADID threadid, UINT32 instId)
{
    PIN_GetLock(&pinLock, threadid + 1);
    fprintf(out, "t%d ld 0x%lx\n", threadid, (unsigned long)addr);
    fflush(out);
    PIN_ReleaseLock(&pinLock);
}

VOID StoreSingle(ADDRINT addr, THREADID threadid, UINT32 instId)
{
    PIN_GetLock(&pinLock, threadid + 1);
    fprintf(out, "t%d st 0x%lx\n", threadid, (unsigned long)addr);
    fflush(out);
    PIN_ReleaseLock(&pinLock);
}

 
//====================================================================
// Instrumentation Routines
//====================================================================
 
VOID Instruction(INS ins, void * v)
{
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Instrument each memory operand. If the operand is both read and written
    // it will be processed twice.
    // Iterating over memory operands ensures that instructions on IA-32 with
    // two read operands (such as SCAS and CMPS) are correctly handled.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
                // map sparse INS addresses to dense IDs
                const ADDRINT iaddr = INS_Address(ins);

                    INS_InsertPredicatedCall(
                        ins, IPOINT_BEFORE, (AFUNPTR) LoadSingle,
                        IARG_MEMORYOP_EA, memOp,
                        IARG_THREAD_ID,
                        IARG_UINT32, iaddr,
                        IARG_END);
        }
        
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
                const ADDRINT iaddr = INS_Address(ins);

                    INS_InsertPredicatedCall(
                        ins, IPOINT_BEFORE,  (AFUNPTR) StoreSingle,
                        IARG_MEMORYOP_EA, memOp,
                        IARG_THREAD_ID,
                        IARG_UINT32, iaddr,
                        IARG_END);
        }
    }
}
 
// This routine is executed once at the end.
VOID Fini(INT32 code, VOID* v) { fclose(out); }
 
/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
 
INT32 Usage()
{
    PIN_ERROR("This Pintool prints a trace of malloc calls in the guest application\n" + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}
 
/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
 
int main(INT32 argc, CHAR** argv)
{
    // Initialize the pin lock
    PIN_InitLock(&pinLock);
 
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();
    PIN_InitSymbols();
 
    out = fopen(KnobOutputFile.Value().c_str(), "w");
 
    INS_AddInstrumentFunction(Instruction, 0);
 
    // Register Analysis routines to be called when a thread begins/ends
    PIN_AddThreadStartFunction(ThreadStart, 0);
    PIN_AddThreadFiniFunction(ThreadFini, 0);
 
    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
 
    // Never returns
    PIN_StartProgram();
 
    return 0;
}