// CreateStaticDLL.cpp
//
// Copyright (c) 2000-2005 Symbian Software Ltd.  All rights reserved.
// This program creates a dll.

#include "CreateStaticDLL.h"
#include <e32uid.h>

// construct/destruct

EXPORT_C CMessenger* CMessenger::NewLC(CConsoleBase& aConsole, const TDesC& aString)
	{
	CMessenger* self=new (ELeave) CMessenger(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL(aString);
	return self;
	}

CMessenger::~CMessenger() // destruct - virtual, so no export
	{
	delete iString;
	}

EXPORT_C void CMessenger::ShowMessage()
	{
	_LIT(KFormat1,"%S\n");
	iConsole.Printf(KFormat1, iString); // notify completion
	}

// constructor support
// don't export these, because used only by functions in this DLL, eg our NewLC()

CMessenger::CMessenger(CConsoleBase& aConsole) // first-phase C++ constructor
	: iConsole(aConsole)
	{
	}

void CMessenger::ConstructL(const TDesC& aString) // second-phase constructor
	{
	iString=aString.AllocL(); // copy given string into own descriptor
    }

