// ClientServer.h
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.
//
#include <e32base.h>


// server name

_LIT(KCountServerName,"CountServer");

// A version must be specified when creating a session with the server

const TUint KCountServMajorVersionNumber=0;
const TUint KCountServMinorVersionNumber=1;
const TUint KCountServBuildVersionNumber=1;

IMPORT_C TInt StartThread(RThread& aServerThread);


// Function codes (opcodes) used in message passing between client and server
enum TCountServRqst
	{
	ECountServCreate = 1,
	ECountServSetFromString,
	ECountServClose,
    ECountServUnsupportedRequest,
	ECountServIncrease,
	ECountServIncreaseBy,
	ECountServDecrease,
	ECountServDecreaseBy,
	ECountServValue,
	ECountServReset
	};

enum TCountServLeave
{
	ENonNumericString = 99
};
