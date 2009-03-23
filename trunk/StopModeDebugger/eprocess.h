// eprocess.h
//
// Copyright (c) 1998-1999 Symbian Ltd.  All rights reserved.
//

class EpocProcess
	{
public:
	EpocProcess(const EpocDebugger &aEpocDebugger, TLinAddr aTargetAddress);

	TInt Name(char *aBuffer);
	// process aware memory access
	TInt ReadWord(TLinAddr aAddress, TUint32 &aVal);
	TInt WriteWord(TLinAddr aAddress, TUint32 aVal);
	TInt ReadMemory(TLinAddr aAddress, TInt count, char * aBuffer);
	TInt WriteMemory(TLinAddr aAddress, TInt count, char * aBuffer);
	TLinAddr HomeAddress(TLinAddr aRunAddress);
public:
	EpocDebugger *iEpocDebugger;
	TLinAddr iTargetAddress;
	};

class EpocThread
	{
public:
	EpocThread(const EpocDebugger &aEpocDebugger, TLinAddr aTargetAddress);

	TInt Name(char *aBuffer);
	// thread aware register access
	TInt ReadRegister(TLinAddr aAddress, TUint32 &aVal);
	TInt WriteRegister(TLinAddr aAddress, TUint32 aVal);
public:
	EpocDebugger *iEpocDebugger;
	TLinAddr iTargetAddress;
	};
