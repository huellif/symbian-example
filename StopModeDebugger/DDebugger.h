// ddebugger.h
//
// Copyright (c) 1998-1999 Symbian Ltd.  All rights reserved.
//
#ifndef __DDEBUGGER_H__
#define __DDEBUGGER_H__

#include "epoc.h"

// These structures are defined by EPOC and should match the
// definitions there

//
// Definitions reflecting the target's debugger structures
// for use on HOST to match TARGET layout
//

// requires DPassiveDebugger version 1.1 on the target
const TInt KPassiveDebuggerMajorVersionNumber=1;
const TInt KPassiveDebuggerMinorVersionNumber=1;
const TInt KPassiveDebuggerBuildVersionNumber=0;

const TUint32 KDebuggerChangeProcess = 0x01;
const TUint32 KDebuggerChangeThread = 0x02;
const TUint32 KDebuggerChangeLibrary = 0x04;
const TUint32 KDebuggerChangeChunk = 0x08;

class TPtrC
	{
public:
	TInt iLength;
	TUint16 *iPtr;
	};

template <class T>
class RArray
	{
public:
	TInt iCount;
	T** iEntries;
//	TInt iEntrySize;
//	TInt iKeyOffset;
	TInt iAllocated;
	TInt iGranularity;
	};

//
//
//

class TDebugEntry
	{
public:
	TAny *iObject;
	TPtrC iName;
	};

class TDebugEntryProcess;
class TDebugEntryChunk : public TDebugEntry
	{
public:
	TDebugEntryProcess *iOwningProcess;
	TInt iSize;
	TLinAddr iRunAddress;
	TLinAddr iHomeAddress;
	};

class TDebugEntryProcess : public TDebugEntry
	{
public:
	TInt iTextSize;
	TInt iConstDataSize;
	TInt iDataSize;
	TInt iBssSize;
	TLinAddr iCodeRunAddress;
	TLinAddr iCodeHomeAddress;
	TDebugEntryChunk *iDataBssChunk;
	};

class TDebugEntryThread : public TDebugEntry
	{
public:
	TDebugEntryProcess *iOwningProcess;
	TAny *iContext;
	TDebugEntryChunk *iStackHeapChunk;
	};

class TDebugEntryLibrary : public TDebugEntry
	{
public:
	TInt iTextSize;
	TInt iConstDataSize;
	TInt iDataSize;
	TInt iBssSize;
	TLinAddr iCodeAddress;
	TLinAddr iDataRunAddress;
	TLinAddr iDataHomeAddress;
	};

class TRomEntry;
class TRomDir;

class DDebugger
	{
public:
	TUint32 dummy_virtual_function_pointer;
	TUint32 iVersion;
	TInt iLocked;
	TDebugEntryThread *iCurrentThread;
	RArray<TDebugEntryProcess *> iProcessArray;
	RArray<TDebugEntryLibrary *> iLibraryArray;
	RArray<TDebugEntryThread *> iThreadArray;
	RArray<TDebugEntryChunk *> iChunkArray;
	TUint32 iReserved[6];
	TUint32 iChangeFlags;
	RArray<TPhysAddr> iShadowPageArray;
	TLinAddr iShadowChunkBase;
	};

#endif