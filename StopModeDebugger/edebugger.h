// edebugger.h
//
// Copyright (c) 1998-1999 Symbian Ltd.  All rights reserved.
//
#ifndef __EDEBUGGER_H__
#define __EDEBUGGER_H__

#include "epoc.h"
#include "ddebugger.h"

// global source control
#define __TRACE TRACE
#define __DEBUG_NEW

#define __HALT_NOTIFICATION__  // host debugger supports halt notification
#define __SHADOW_ROM_PAGES__   // target allows ROM pages to be shadowed
// #define __DEBUG_CHUNKS__
// #define __DEBUG_THREADS__

//
// Class for accessing the target's debugger
//

struct TShadowPage
	{
	int iAge;
	//
	TLinAddr iRomPage;
	TLinAddr iRamPage;
	TUint32 iRomPte;
	};

enum TRegister
	{
	R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15, // CPU registers
	R8_FIQ, R9_FIQ, R10_FIQ, R11_FIQ, R12_FIQ, R13_FIQ, R14_FIQ, // FIQ mode shadowed registers
	R13_IRQ, R14_IRQ, // IRQ mode shadowed registers
	R13_SVC, R14_SVC, // SVC mode shadowed registers
	CP15_DACR, CP15_R0, CP15_R1, CP15_R2, // CP15 (MMU) registers
	// etc... etc...
	}

class EpocDebugger
	{
public:
	EpocDebugger(const Hardware &aHardware);
	~EpocDebugger();
	int Create();

	// target accessors
	TLinAddr Debugger();
	bool IsRomAddress(TLinAddr aAddress);
	bool ReadWord(TLinAddr address, TUint32 * word);
	bool WriteWord(TLinAddr address, TUint32 word);
	bool ReadMemory(TLinAddr address, int count, TUint32 * buffer);
    bool WriteMemory(TLinAddr address, int count, TUint32 * buffer);
	bool ReadDes(TLinAddr address, char * buffer);
	bool ReadRegister(TRegister reg, TUint32 * value);
	bool WriteRegister(TRegister reg, TUint32 * value);

	// object handling
	TLinAddr FindLibrary(const char *aLibName);
	TLinAddr FindProcess(const char *aLibName);
	bool ReadChunks();

	// thread and process handing
	TLinAddr CurrentThread();
	TLinAddr CurrentProcess();
	TLinAddr OwningProcess(TLinAddr aThread);
	TUint32 Handle(TLinAddr aObject);
	int NumberOfProcesses();
	int NumberOfThreads();
	TPhysAddr LinearToPhysical(TLinAddr aLinAddr);

	// breakpoint handing
    bool SetBreakPoint(TLinAddr address);
    bool ClearBreakPoint(TLinAddr address);
	bool Copy(TLinAddr dest, TLinAddr src, int count);

	// MMU handling
	bool Flush();
	bool UnlockDomains(TUint32 &dacr);
	bool LockDomains(TUint32 dacr);

private:
	int NextFreeBreakPointIndex();
	int BreakPointIndex(TLinAddr address);
	int BreakPointCount(TLinAddr page);

	void FreeShadowPage(int aShadowPageIndex);
	int NextFreeShadowPage();
	TLinAddr ShadowChunkBase();

	int ShadowPageIndex(TLinAddr page);
	TLinAddr ShadowPageAddress(TLinAddr aRomAddress);
	bool ShadowPage(TLinAddr page);
	bool ThawShadowPage(TLinAddr address);
	bool FreezeShadowPage(TLinAddr address);
public:
	Hardware *iHardware; // the target hardware interface
	TLinAddr iDebugger; // the address of the DPassiveDebugger structure on the target
//
	TLinAddr *iBreakPoints;
	TUint32 *iBreakInstruction;
	TShadowPage *iShadowPages;
	TDebugEntryChunk *iChunks;
	TInt iChunkCount;
	};

#endif