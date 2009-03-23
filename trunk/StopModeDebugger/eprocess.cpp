// eprocess.cpp
//
// Copyright (c) 1998-1999 Symbian Ltd.  All rights reserved.
//

TInt EpocProcess::Name(char *name)
//
// Get the name of this process into the supplied buffer
//
	{
	// get the name of this process
	TLinAddr nameptr=iTargetAddress + _FOFF(TDebugEntryProcess, iName);
	TInt result=KErrArgument
	result=iEpocDebugger->ReadDes(nameptr, name);
	return result;
	}

TInt EpocProcess::ReadWord(TLinAddr address, TUint32 * word)
//
// Read a word from this processes address space
// if the process is current then we can read directly from the
// running section
// if the process is not current the we must read the data from
// the home section
//
	{

	__TRACE("Process::ReadWord(%08x) from process %s", address, m_name);
	if (gEpocDebugger->IsRomAddress(address))
		{
		__TRACE("\n");
		return m_pParent->ReadWord(address, word);
		}

	TLinAddr * currentprocess = iEpocDebugger->CurrentProcess();
	TInt r=false;
	if (currentprocess==iTargetAddress)
		{
		// this thread is current, so we can just use the address
		r = iEpocDebugger->ReadWord(address, word);
		__TRACE(" from current = %08x\n", *word);
		}
	else
		{
		// this thread is not current, we need to convert the
		// address to an address in the home section
		TLinAddr homeAddr=gEpocDebugger->HomeAddress(address);
		if (homeAddr!=NULL) // found in this process
			r = iEpocDebugger->ReadWord(homeAddr, word);
		else
			r= KErrArgument;
		__TRACE(" in home section = %08x\n", *word);
		}
	return r;
	}

TInt EpocProcess::WriteWord(TLinAddr address, TUint32 word)
//
// Write a word to this process
// if the process is current then we can write directly to the
// running section
// if the process is not current the we must write the data to
// the home section
//
	{
	
	__TRACE("Process::WriteWord(%08x, %d) to process %s", address, word, m_name);
	if (gEpocDebugger->IsRomAddress(address))
		return KErrArgument;

	TLinAddr * currentprocess = iEpocDebugger->CurrentProcess();
	bool r=false;
	if (currentprocess==iTargetAddress)
		{
		__TRACE(" current\n");
		// this thread is current, so we can just use the address
		r = m_pParent->WriteWord(address, word);
		}
	else
		{
		// this thread is not current, we need to convert the
		// address to an address in the home section
		TLinAddr homeAddr=HomeAddress(address);
		if (homeAddr!=NULL)
			r = iEpocDebugger->WriteWord(address, word);
		__TRACE(" in home section @ %08x\n", homeAddr);
		}
	return r;
	}

TInt EpocProcess::ReadMemory(TLinAddr address,  TInt count, char * buffer)
//
// Read memory from this processes address space
// if the process is current then we can read directly from the
// running section
// if the process is not current the we must read the data from
// the home section
//
	{

	__TRACE("Process::ReadMemory(%08x,%d) from process %s", address, count, m_name);
	if (iEpocDebugger->IsRomAddress(address))
		return iEpocDebugger->ReadMemory(address, page, count, buffer);

	TLinAddr * currentprocess = iEpocDebugger->CurrentProces();
	TInt r=KErrArgument;
	if (currentprocess==iTargetAddress)
		{
		// this process is current, so we can just use the address in the running section
		r = iEpocDebugger->ReadMemory(address, count, buffer);
		__TRACE(" in running section\n");
		}
	else
		{
		// this process is not current, we need to convert the
		// address to an address in the home section
		TLinAddr homeAddr=HomeAddress(address);
		if (homeAddr!=NULL) // not found in this process
			r = iEpocDebugger->ReadMemory(homeAddr, count, buffer);
		else
			{ // the memory does not exist in this process
			r=KErrArguemnt;
			}
		__TRACE(" in home section %08x\n", homeAddr);
		}
	return r;
	}

TInt EpocProcess::WriteMemory(TLinAddr address, TInt count, TUint8 * buffer)
//
// Write memory to this process
// if the process is current then we can write directly to the
// running section
// if the process is not current the we must write the data to
// the home section
//
	{

	__TRACE("Process::WriteMemory(%08x, %d) to process %s", address, count, m_name);
	if (iEpocDebugger->IsRomAddress(address))
		return KErrArgument;

	TLinAddr * currentprocess = (iEpocDebugger->CurrentProcess());
	TInt r=KErrArgument;
	if (currentprocess==iTargetAddress)
		{
		__TRACE(" in running section\n");
		// this thread is current, so we can just use the address
		r = iEpocDebugger->WriteMemory(address, count, buffer);
		}
	else
		{
		__TRACE(" in home section\n");
		// this thread is not current, we need to convert the
		// address to an address in the home section
		TLinAddr homeAddr=HomeAddress(address);
		if (homeAddr!=NULL)
			r = iEpocDebugger->WriteMemory(address, count, buffer);
		}
	return r;
	}


TLinAddr EpocProcess::HomeAddress(TLinAddr aRunAddress)
//
// return the home address for the given run address in this process
// aProcess is the address of the TDebugEntryProcess on the target
// return NULL if the address does not exist in this process
//
	{

	// rom addresses remain the same and are visible to all processes
	if (IsRomAddress(aRunAddress))
		return aRunAddress;

	TDebugEntryChunk *chunks=iEpocDebugger->iChunks;
	TInt chunkCount=iChunkCount;
	// read the chunk list to convert the given run address to a home address
	int i;
	for (i=0; i<chunkCount; i++)
		{
		TDebugEntryChunk *c=&(chunks[i]);
		// is the chunk in this process
		if (c->iOwningProcess && (c->iOwningProcess != (TDebugEntryProcess *)aProcess))
			continue;
		// the chunk is in this process, check it for a match
		TLinAddr runEndAddr=c->iRunAddress + c->iSize;
		if ((aRunAddress>=c->iRunAddress) && (aRunAddress<=runEndAddr))
			return c->iHomeAddress + aRunAddress - c->iRunAddress; //  return the home address
		}
	return NULL;
	}



//
// Epoc Thread aware functions
//

TInt EpocThread::ReadRegister(TRegister reg, TUint32 * value)
//
// Read a CPU register from this thread
// if the thread is current then we can read the CPU register directly
// if the thread is not current then we must read from the thread's saved context
// in the kernel
//
{

//	__TRACE("Thread::ReadRegister(%d) from thread %s\n", reg,m_name);
	if (reg<0)
		return KErrArgument;
	TLinAddr * currentthread = iEpocDebugger->CurrentThread();
	TInt r=KErrArgument;
    if (currentthread == iTargetAddress)
		{
        // read CPU registers since this is the currently executing thread
		r=iEpocDebugger->ReadRegister(reg, value);
		}
    else
	    {
        // read registers from the context-saved registers 
		if (iTargetAddress!=NULL)
			{
			TLinAddr threadcontextptr=iTargetAddress + _FOFF(TDebugEntryThread, iContext);
			TLinAddr threadcontext;
			r = iEpocDebugger->ReadWord(threadcontextptr, &threadcontext);
			if (r==KErrNone)
				r = iEpocDebugger->ReadWord(threadcontext + (reg * sizeof(TUint32)), value);
			}
		}
//	__TRACE("Thread::ReadRegister(%d) returned %08x (valid=%d)\n", reg, *value, r);
    return r;
}

TInt EpocThread::WriteRegister(TRegister reg, TUint32 * value)
//
// Write a CPU register to this thread
// if the thread is current then we can write the CPU register directly
// if the thread is not current then we must write to the thread's saved context
// in the kernel
//
	{

//	__TRACE("Thread::WriteRegister(%d) value %08x to thread %s\n", *value, m_name);
	TLinAddr * currentthread = iEpocDebugger->CurrentThread();
	TInt r=KErrArgument;
	if (currentthread == iTargetAddress)
		{
	    // read CPU registers since this is the currently executing thread
		r=iEpocDebugger->WriteRegister(reg, value);
		}
    else
		{
		// write registers into the context-saved registers
		if (iTargetAddress!=NULL)
			{
			TLinAddr threadcontextptr=iTargetAddress + _FOFF(TDebugEntryThread, iContext);
			TLinAddr threadcontext;
			r = iEpocDebugger->ReadWord(threadcontextptr, &threadcontext);
			if (r==KErrNone)
				r = iEpocDebugger->WriteWord(threadcontext + (reg * sizeof(TUint32)), *value);
			}
		}
    return r;
	}

