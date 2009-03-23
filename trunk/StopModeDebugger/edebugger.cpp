// edebugger.cpp
//
// Copyright (c) 1998-1999 Symbian Ltd.  All rights reserved.
//

#include "edebugger.h"
#include "ddebugger.h"
#include "epoc.h"

//
// Constructer for DPassiveDebugger interface on the HOST
//
EpocDebugger::EpocDebugger(const Hardware &aHardware)
	{
	TRACE("EpocDebugger::EpocDebugger()\n");
	//
	// The interface for accessing the target
	//
	iHardware=&aHardware;
	}

int EpocDebugger::Create()
//
// Second phase construction
//
	{
	//
	// Create the host debug structures
	//
	iChunks=NULL;
	iChunkCount=0;
	iBreakPoints=new TLinAddr [KMaxBreakPoints];
	iBreakInstruction=new TLinAddr [KMaxBreakPoints];
	iShadowPages=new TShadowPage [KMaxShadowPages];
	// TODO: check mem alloc errors

	//
	// Clear the host debug structures
	// we assume we have no breakpoints and no shadowed areas of ROM
	//
	int i;
	for (i=0; i<KMaxBreakPoints; i++)
		iBreakPoints[i]=KNullBreakPoint;
	for (i=0; i<KMaxShadowPages; i++)
		iShadowPages[i].iRomPage=NULL;

	//
	// Turn off permissions-checking on the super page on the target
	//
	TUint32 dacr;
	UnlockDomains(dacr);
	//
	if (TargetHalted()) return KErrInUse;
	//
	// Get the address of the debugger structure from the target
	//
	TLinAddr debugger;
	bool result = ReadWord(KDebuggerAddress, &debugger);
	if (result)
		iDebugger=debugger;
	else
		iDebugger=NULL;
	if (iDebugger==NULL) return KErrGeneral;

	//
	// Check the version number
	//
	TUint32 requiresversion=(TUint32) ((KPassiveDebuggerMajorVersionNumber<<24) | (KPassiveDebuggerMinorVersionNumber<<16));
	TUint32 version;
	ReadWord(iDebugger + _FOFF(DDebugger, iVersion), version);
	if (version < requiresversion)
		return KErrGeneral;

	//
	// Can do other debugger initialisation here
	//
	return KErrNone;
	}

EpocDebugger::~EpocDebugger()
//
//
//
	{

	TRACE("EpocDebugger::~EpocDebugger()\n");
	delete [] iBreakPoints;
	delete [] iBreakInstruction;
	delete [] iShadowPages;
	delete iChunks;
	}


bool EpocDebugger::ReadWord(TLinAddr address, TUint32 * word)
//
// Read a word from the target
// returns false on error
//
	{
	return iHardware->ReadWord(address, word);
	}

bool EpocDebugger::WriteWord(TLinAddr address, TUint32 word)
//
// Write a word to the target
// returns false on error
//
	{
	return iHardware->WriteWord(address, word);
	}

bool EpocDebugger::ReadMemory(TLinAddr address, TInt count, TUint8 * buffer)
//
// Read memory from the target
// returns false on error
//
	{
	return iHardware->ReadMemory(address, 0, count, buffer);
	}

bool EpocDebugger::WriteMemory(TLinAddr address, TInt count, TUint8 * buffer)
//
// Write memory to the target
// returns false on error
//
	{
	return iHardware->WriteMemory(address, 0, count, buffer);
	}

bool EpocDebugger::ReadRegister(TRegister reg, TUint32 * value)
//
// Read a register from the target
// returns false on error
//
	{
	return iHardware->ReadRegister(reg, value);
	}

bool EpocDebugger::WriteRegister(TRegister reg, TUint32 * value)
//
// Write a register on the target
// returns false on error
//
	{
	return iHardware->WriteRegister(reg, value);
	}

bool EpocDebugger::ReadDes(TLinAddr address, TUint8 *buffer)
//
// Read an epoc descriptor from the target
// This function assumes 'address' the address of a TPtrC16 on the target
// returns a c string in the buffer provided
// returns false on error
//
	{

	int deslength;
	bool result=iHardware->ReadWord(address, &deslength);
	if (!result)
		return result;
	deslength&=0x00ffffff; // mask out descriptor type
	TLinAddr iptr;
	result=iHardware->ReadWord(address+4, &iptr);
	if (!result)
		return result;
	if (iptr==NULL)
		deslength=0;

	int bytesptr=iptr & 3; // number of bytes into the word that the string starts
	int size=((deslength+bytesptr+3)&~3)<<1;
	result=iOs->ReadMemory(iptr & ~3, 0, size, buffer);
	if (!result)
		return result;

	buffer[len]=0; // terminate the string
	return true;
	}

TLinAddr EpocDebugger::Debugger()
//
// return the address of the DPassiveDebugger structure on the target
//
	{
	return iDebugger;
	}

bool EpocDebugger::ReadChunks()
//
// Read the chunks from the target
//
	{

	int chunkCount;
	TLinAddr chunkPtrArray;
	TLinAddr address=Debugger() + _FOFF(DDebugger, iChunkArray);
	bool r=ReadWord(address + _FOFF(RArray<TDebugEntryChunk *>, iCount), &chunkCount);
	if (r)
		ReadWord(address + _FOFF(RArray<TDebugEntryChunk *>, iEntries), &chunkPtrArray);
	if (!r) return r;

	TInt size=chunkCount * sizeof(TDebugEntryChunk);
	iChunkCount=0;
	iChunks=(TDebugEntryChunk *)realloc(iChunks, size);
	if (iChunks==NULL)
		return false;
	iChunkCount=chunkCount;

	// read all the TDebugEntryChunk into the host buffers
	int i;
	TDebugEntryChunk *c=iChunks;
	for (i=0; i<chunkCount; i++, c++, chunkPtrArray+=4)
		{
		// read ptr to chunk
		TLinAddr chunkp;
		r=ReadWord(chunkPtrArray, &chunkp);
		// read the chunk
		r=ReadMemory(chunkp, sizeof(TDebugEntryChunk), c);
		}

#if defined __DEBUG_CHUNKS__
	c=iChunks;
	for (i=0; i<chunkCount; i++, c++)
		{
		__TRACE("Chunk %2d:  Object=%08x  Proc=%08x  Home=%08x  Run=%08x  Size=%x\n", i, c->iObject, c->iOwningProcess, c->iHomeAddress, c->iRunAddress,  c->iSize);
		}
#endif

	return r;
	}

TLinAddr EpocDebugger::FindLibrary(const char *aLibName)
//
// Find the TDebugEntryLibrary on the target that matches the given library name
// returns the address of the TDebugEntryLibrary on the target
// returns NULL if not found
//
	{

	int len=strlen(aLibName);
	char name[KMaxName+1];
	int libCount;
	TLinAddr libPtrArray;
	TLinAddr address=Debugger() + _FOFF(DDebugger, iLibraryArray);
	bool r=ReadWord(address + _FOFF(RArray<TDebugEntryLibrary *>, iCount), &libCount);
	if (r)
		ReadWord(address + _FOFF(RArray<TDebugEntryLibrary *>, iEntries), &libPtrArray);
	if (!r) return r;

	// iterate through all the libraries on the target to find a match
	int i;
	for (i=0; i<libCount; i++, libPtrArray+=4)
		{
		// read ptr to lib
		TLinAddr libp;
		r=ReadWord(libPtrArray, &libp);
		if (!r) continue;
		// read its name
		TLinAddr nameptr=libp + _FOFF(TDebugEntryLibrary, iName);
		r=ReadDes(nameptr, name);
		if (!r) continue;

		// does it match?
		if (strnicmp(aLibName, name, len)==0)
			{
			__TRACE("EpocDebugger::FindLibrary(%s) matched %s\n", aLibName, name);
			return libp;
			}
		}
	return NULL;
	}

TLinAddr EpocDebugger::FindProcess(const char *aProcName)
//
// Find the TDebugEntryProcess on the target that matches the given process name
// returns the address of the TDebugEntryProcess on the target
// returns NULL if not found
//
	{

	int len=strlen(aProcName);
	char name[KMaxName+1];
	int pCount;
	TLinAddr pPtrArray;
	TLinAddr address=Debugger() + _FOFF(DDebugger, iProcessArray);
	bool r=ReadWord(address + _FOFF(RArray<TDebugEntryProcess *>, iCount), &pCount);
	if (r)
		ReadWord(address + _FOFF(RArray<TDebugEntryProcess *>, iEntries), &pPtrArray);
	if (!r) return r;

	// iterate through all processes on the target to find a match
	int i;
	for (i=0; i<pCount; i++, pPtrArray+=4)
		{
		// read ptr to proc
		TLinAddr procp;
		r=ReadWord(pPtrArray, &procp);
		if (!r) continue;
		// read its name
		TLinAddr nameptr=procp + _FOFF(TDebugEntryProcess, iName);
		r=gEpocDebugger->ReadDes(nameptr, name);
		if (!r) continue;

		// does it match?
		if (strnicmp(aProcName, name, len)==0)
			{
			__TRACE("EpocDebugger::FindProcess(%s) matched %s\n", aProcName, name);
			return procp;
			}
		}
	return NULL;
	}

TLinAddr EpocDebugger::CurrentThread()
//
// Return the address of the TDebugEntryThread on the target of the
// currently scheduled thread
//
	{

	TLinAddr address=Debugger() + _FOFF(DDebugger, iCurrentThread);
	int current;
	bool result = ReadWord(address, &current);
	if (!result)
		return NULL;
	return current;
	}

TLinAddr EpocDebugger::CurrentProcess()
//
// Return the currently scheduled process
// returns the address of the TDebugEntryProcess on the target
//
	{

	TLinAddr address=CurrentThread();
	return OwningProcess(address);
	}

TLinAddr EpocDebugger::OwningProcess(TLinAddr aThread)
//
// Return the owning process of the given thread
// aThread is the address of the TDebugEntrythread on the target
// returns NULL on error
// return the address of the owning TDebugEntryProcess on the target
//
	{

	if (aThread==NULL)
		return NULL;
	TLinAddr address=aThread + _FOFF(TDebugEntryThread, iOwningProcess);
	TLinAddr owner;
	bool result = ReadWord(address, &owner);
	if (!result)
		return NULL;
	return owner;
	}

TUint32 EpocDebugger::Handle(TLinAddr aObject)
//
// Return a unique id for the given object
// The object may be a TDebugEntry of any type
// return NULL on error
//
	{

	TUint32 handle;
	// use the object pointer as the unique identifier
	TLinAddr objectptr=aObject + _FOFF(TDebugEntry, iObject);
	bool r=ReadWord(objectptr, &handle);
	if (!r)
		return NULL;
	return handle;
	}

int EpocDebugger::NumberOfProcesses()
//
// Return the number of processes currently running on the target
//
	{

	// address of process array on target
	TLinAddr processArray = gEpocDebugger->Debugger() + _FOFF(DDebugger, iProcessArray);
	TLinAddr processArrayCount = processArray + _FOFF(RArray, iCount);
    int numProcesses;
    bool result = ReadWord(processArrayCount, &numProcesses);
	if (!result)
		return 0;
	return numProcesses;
	}

int EpocDebugger::NumberOfThreads()
//
// Return the number of threads currently running on the target
//
	{

	TLinAddr threadArray = Debugger() + _FOFF(DDebugger, iThreadArray);
	TLinAddr threadArrayCount = threadArray + _FOFF(RArray<TDebugEntryThread>, iCount);
    int numThreads;
	bool result = ReadWord(threadArrayCount, &numThreads);
	if (!result)
		return 0;
	return numThreads;
	}

bool EpocDebugger::IsRomAddress(TLinAddr aAddress)
//
// return true if the given address is part of ROM
//
	{
	return (aAddress>=0x50000000 && aAddress <0x51000000);
	}

int EpocDebugger::NextFreeBreakPointIndex()
//
// Find the next free breakpoint
//
	{

	if (iBreakPoints==NULL)
		return KErrNotFound;
	int i;
	for (i=0; i<KMaxBreakPoints; i++)
		{
		if (iBreakPoints[i]==KNullBreakPoint)
			return i;
		}
	return KErrNotFound;
	}

int EpocDebugger::BreakPointIndex(TLinAddr address)
//
// find the index of the breakpoint set at the given address
//
	{

	if (iBreakPoints==NULL)
		return KErrNotFound;
	int i;
	for (i=0; i<KMaxBreakPoints; i++)
		{
		if (iBreakPoints[i]==address)
			return i;
		}
	return KErrNotFound;
	}

int EpocDebugger::BreakPointCount(TLinAddr page)
//
// Return the number of breakpoints set in the given page on the target
//
	{

	if (iBreakPoints==NULL)
		return 0;
	page &= ~KPageMask;
	int count=0;
	int i;
	for (i=0; i<KMaxBreakPoints; i++)
		{
		unsigned long bp=iBreakPoints[i];
		if (bp==KNullBreakPoint)
			continue;
		if ((bp & ~KPageMask) == page)
			count++;
		}
	return count;
	}

void EpocDebugger::FreeShadowPage(int aShadowPageIndex)
//
// Free up the specified unused shadowpage
//
	{

	// copy the ROM's saved PTE back to restore the original ROM page
	int i=aShadowPageIndex;
	TLinAddr rompteptr=PageTableEntryAddress(iShadowPages[i].iRomPage);
	WriteWord(rompteptr, iShadowPages[i].iRomPte);
	Flush();
	iShadowPages[i].iAge=0;
	iShadowPages[i].iRomPage=NULL;
	iShadowPages[i].iRamPage=NULL;
	iShadowPages[i].iRomPte=NULL;
	}

int EpocDebugger::NextFreeShadowPage()
//
// Find an available shadow page
//
	{

	if (iShadowPages==NULL)
		return KErrNotFound;
	int i;
	for (i=0; i<KMaxShadowPages; i++)
		{
		if (iShadowPages[i].iRomPage==NULL)
			return i;
		}
	// all shadow pages have been used up
	// so we need to clean out a shadow page
	// that hasn't been used for a while
	for (i=0; i<KMaxShadowPages; i++)
		{
		if (BreakPointCount(iShadowPages[i].iRomPage)==0)
			{
			FreeShadowPage(i);
			return i;
			}
		}
	return KErrNotFound;
/*
//
// TODO: implement LRU for shadowpages
//
	int p=LeastRecentlyUsedShadowPage();
	if (p==KErrNotFound)
		return p;
	FreeShadowPage(p);
*/
	}

TLinAddr EpocDebugger::ShadowChunkBase()
//
// Find the linear address of the shadow pages allocated on the target
//
	{

	TLinAddr shadowchunkbaseptr=gEpocDebugger->Debugger() + _FOFF(DDebugger, iShadowChunkBase);
	TLinAddr shadowchunkbase;
	bool r=ReadWord(shadowchunkbaseptr, &shadowchunkbase);
	if (!r)
		return NULL;
	return shadowchunkbase;
	}

bool EpocDebugger::Copy(TLinAddr dest, TLinAddr src, int count)
//
// target memcpy
//
	{

	long *buffer=new long [count];
	bool r;
	r=iOs->ReadMemory(src, 0, count, buffer);
	if (r)
		r=iOs->WriteMemory(dest, 0, count, buffer);
	delete buffer;
	return r;
	}

bool EpocDebugger::ShadowPage(TLinAddr page)
//
// Shadow a page of Rom by replacing the PTE of the Rom page
// with one from the pre-allocated Ram shadow pages
// Must also mark the Ram page as UserRo
//
	{

	int i=NextFreeShadowPage();
	if (i==KErrNotFound)
		return false;
	page &= ~KPageMask;

	TLinAddr shadowchunkbase=ShadowChunkBase();
	TLinAddr shadowpage=shadowchunkbase + i*KPageSizeInBytes;
	// copy the data from the ROM into our new shadow page
	bool r=Copy(shadowpage, page, KPageSizeInBytes);
	if (!r) return r;

	// remap the RAM page into where the ROM page was
	TLinAddr shadowpteptr=PageTableEntryAddress(shadowpage);
	TLinAddr rompteptr=PageTableEntryAddress(page);
	TUint32 shadowpte;
	r=ReadWord(shadowpteptr, &shadowpte);
	shadowpte &= ~KPtePermissionMask; // sup ro, user ro
	if (r)
		r=ReadWord(rompteptr, &iShadowPages[i].iRomPte);
	if (r)
		r=WriteWord(rompteptr, shadowpte);
	if (r)
		{
		iShadowPages[i].iRomPage=page; // mark shadow page as used
		iShadowPages[i].iRamPage=shadowpage; // mark which shadow page was used
		}
	Flush();
	return r;
	}

int EpocDebugger::ShadowPageIndex(TLinAddr aLinAddr)
//
// return the index of the shadow page being used for the given address
// or KErrNotFound if page is not shadowed
//
	{

	TLinAddr page=aLinAddr & ~KPageMask;
	int i;
	for (i=0; i<KMaxShadowPages; i++)
		{
		if (iShadowPages[i].iRomPage==page)
			return i;
		}
	return KErrNotFound;
	}

TPhysAddr EpocDebugger::LinearToPhysical(TLinAddr aLinAddr)
//
// Convert a linear to a physical address by parsing the page table info
// on the target
//
	{

	TUint pdeindex=aLinAddr >> 22;
	TLinAddr pdeptr=KPageDirectory + (pdeindex << 2);
	TUint32 pde;
	bool r=ReadWord(pdeptr, &pde);
	if (!r)
		return NULL;
	if ((pde & 3) == 1)
		{
		// coarse page
		// get the page table id
		TLinAddr ptinfoptr=KPageTableInfo + (pdeindex << 2);
		TUint32 ptinfo;
		r=ReadWord(ptinfoptr, &ptinfo);
		if (!r)
			return NULL;
		ptinfo >>= KPageTableInfoIdShift; // page table id

		// get the pte
		TLinAddr pteptr=PageTableLinearAddress(ptinfo);
		pteptr+=((aLinAddr&KChunkSizeMinus1)>>KPageSizeShift)<<2;
		TUint32 pte;
		r=ReadWord(pteptr, &pte);
		if (!r)
			return NULL;

		// calculate the physical address
		TPhysAddr phys = ((pte &~ KPageMask)+(aLinAddr & KPageMask));
		return phys;
		}

	if ((pde & 3) == 2)
		{
		// TODO 1M section
		}
	return NULL;
	}

bool EpocDebugger::UnlockDomains(Tuint32 &dacr)
//
// This function writes manager access to all domains
//
	{
	bool r;
	TUint32 d=0xffffffff;
	r=iHardware->WriteRegister(CP15_DACR, &d);
	return r;
	}

bool EpocDebugger::LockDomains(TUint32 dacr)
//
// This function restores the DACR
//
	{

	// leave EPOC to repair the dacr
	// or
	// return iHardware->WriteRegister(CP15_DACR, &dacr);
	return true;
	}

bool EpocDebugger::Flush()
//
// Flush TLBs and both caches
//
	{

	__TRACE("EpocDebugger::Flush()\n");
	bool r;
	TUint32 val=0;
	r=iOs->WriteRegister(CP15_FLUSH_D_TLB, &val);
	val=0;
	if (r)
		r=iOs->WriteRegister(CP15_FLUSH_I_TLB, &val);
	val=0;
	if (r)
		r=iOs->WriteRegister(CP15_FLSH_D, &val);
	val=0;
	if (r)
		r=iOs->WriteRegister(CP15_FLSH_I, &val);
	return r;
	}

bool EpocDebugger::ThawShadowPage(TLinAddr address)
//
// Mark the permissions of the shadow page at the given address
// to UserRw
//
	{

	bool r;
	TLinAddr rompteptr=PageTableEntryAddress(address);
	TUint32 rompte;
	r=ReadWord(rompteptr, &rompte);
	rompte|=KPtePermissionUserRw;
	if (r)
		r=WriteWord(rompteptr, rompte);
	Flush();
	return r;
	}

bool EpocDebugger::FreezeShadowPage(TLinAddr address)
//
// Mark the permissions of the shadow page at the given address
// to UserRo
//
	{

	bool r;
	TLinAddr rompteptr=PageTableEntryAddress(address);
	TUint32 rompte;
	r=ReadWord(rompteptr, &rompte);
	rompte&=~KPtePermissionUserRw;
	if (r)
		r=WriteWord(rompteptr, rompte);
	Flush();
	return r;
	}

TLinAddr EpocDebugger::ShadowPageAddress(TLinAddr aRomAddr)
//
// Get the shadow page address of the given rom address
//
	{

	TLinAddr page=aRomAddr & ~KPageMask;
	int i;
	for (i=0; i<KMaxShadowPages; i++)
		{
		if (iShadowPages[i].iRomPage==page)
			return iShadowPages[i].iRamPage + (aRomAddr & KPageMask);
		}
	return NULL;
	}

bool EpocDebugger::SetBreakPoint(TLinAddr address)
//
// Set a breakpoint at the given address
// if the address is in ROM then shadow the page as necessary
//
	{

	if (iBreakPoints==NULL)
		return false;
	int bp=NextFreeBreakPointIndex();
	if (bp==KErrNotFound)
		return false;

	TUint32 dacr=0;
	bool r=UnlockDomains(dacr);
	if (!r)
		return r;

	TUint32 data;
	unsigned long page=address & ~KPageMask;
	if (IsRomAddress(address))
		{
#if defined __SHADOW_ROM_PAGES__
		// check to see if this page is already shadowed
		int shadowPage=ShadowPageIndex(page);
		// shadow the page if neccesary
		if (shadowPage==KErrNotFound)
			r=ShadowPage(page);
		if (!r)
			return r;
		// add breakpoint
		r=iHardware->SetSwBreakPoint(address);
#else
		r=iHardware->SetHwBreakPoint(address);
#endif
		}
	else
		{
		//read the location before we write it,
		r=ReadWord(address, &data);
		r=iHardware->SetSwBreakPoint(address);
		if (r) r=Flush();
		}
	if (r)
		{
		iBreakPoints[bp]=address;
		iBreakInstruction[bp]=data;
		LockDomains(dacr);
		}
	return r;
	}

bool EpocDebugger::ClearBreakPoint(TLinAddr aAddress)
//
// Remove the breakpoint previously set at the given address
//
	{

	int bp=BreakPointIndex(aAddress);
	if (bp==KErrNotFound)
		return false;
	bool r=false;
	if (IsRomAddress(aAddress))
		{
		// breakpoint in Rom
#if defined __SHADOW_ROM_PAGES__
		r=iOs->ClearSwBreakPoint(aAddress);
#else
		r=iOs->ClearHwBreakPoint(aAddress);
#endif
		}
	else
		{
		// breakpoint in Ram loaded code
		r=iOs->ClearSwBreakPoint(aAddress);
		}
	if (r) r=Flush();
	if (r)
		iBreakPoints[bp]=KNullBreakPoint;
	return r;
	}

bool RootName(char *dst, const char *src)
//
// Parse the filename in src for the root name of the DLL or EXE
//
	{

	int len=strlen(src);
	const char *p=src+len-1;

	// get the filename from the path
	while (p>=src)
		{
		char c=*p;
		if ((c=='\\') || (c=='/'))
			break;
		p--;
		}
	strcpy(dst, p+1);

	// drop ext
	char *pp=dst;
	while (1)
		{
		if (*pp=='.')
			{
			*pp=0;
			break;
			}
		if (*pp==0)
			break;
		pp++;
		}
	return true;
	}

bool EpocDebugger::GetRelocInfo(char *LibName, TLinAddr &codeSectionStart, TLinAddr &dataSectionStart)
//
// Match the library symbol filename with a library (or process) on the target
// return the code and data addresses of the library on the target
//
	{

	char rootname[KMaxName+1];
	bool r=RootName(rootname, LibName);
	if (!r)
		return r;

	// check libraries first
	TLinAddr lib=FindLibrary(rootname);
	if (lib!=NULL)
		{
		ReadWord(lib + _FOFF(TDebugEntryLibrary, iCodeAddress), &codeSectionStart);
		ReadWord(lib + _FOFF(TDebugEntryLibrary, iDataRunAddress), &dataSectionStart);
		return true;
		}
	// check processes
	TLinAddr proc=FindProcess(rootname);
	if (proc!=NULL)
		{
		TLinAddr chunkptr=NULL;
		ReadWord(proc + _FOFF(TDebugEntryProcess, iCodeRunAddress), &codeSectionStart);
		ReadWord(proc + _FOFF(TDebugEntryProcess, iDataBssChunk), &chunkptr);
		if (chunkptr!=NULL)
			ReadWord(chunkptr+_FOFF(TDebugEntryChunk, iRunAddress), &dataSectionStart);
		return true;
		}
	return false;
	}

int EpocDebugger::HaltEvent()
//
// Read the state of the target
// this should be called every time the target is halted, before any
// other EpocDebugger functions are invoked
//
	{

	if (!iHardware->TargetHalted()) return KErrInUse;
	TUint32 dacr;
	UnlockDomains(dacr);
	TLinAddr thread=CurrentThread();
	TLinAddr process=OwningProcess(thread);
	iCurrentProcess=process;
	iCurrentThread=thread;
	ReadChunks();
	return KErrNone;
	}
