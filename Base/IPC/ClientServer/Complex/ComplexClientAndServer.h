// ComplexClientAndServer.h
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

#if !defined(__ComplexClientAndServer_H__)
#define __ComplexClientAndServer_H__

#include <e32base.h>

//server name

_LIT(KCountServerName,"MultiCountServer");

//the server version. A version must be specifyed when creating a session with the server
const TUint KCountServMajorVersionNumber=0;
const TUint KCountServMinorVersionNumber=1;
const TUint KCountServBuildVersionNumber=1;

//
IMPORT_C TInt StartThread(RThread& aServerThread);


//opcodes used in message passing between client and server
enum TCountServRqst
	{
	ECountServCreateSubSession = 1,
	ECountServCloseSubSession,
	ECountServInitSubSession,
	ECountServCloseSession,
	ECountServIncrease,
	ECountServIncreaseBy,
	ECountServDecrease,
	ECountServDecreaseBy,
	ECountServValue,
	ECountServReset,
	ECountServResourceCountMarkStart,
	ECountServResourceCountMarkEnd,
	ECountServResourceCount
	};


enum TCountServLeave
    {
	ENonNumericString = 99
    };
    
#endif    
