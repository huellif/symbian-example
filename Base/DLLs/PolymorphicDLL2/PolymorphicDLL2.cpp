// PolymorphicDLL2.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

// This program creates a polymorphic interface DLL which
// executable "UsingDLLs" dynamically links to.
//


#include "PolymorphicDLL2.h"
#include <e32def.h>
#include <e32uid.h>


// Function to construct a CMessenger object. Note that this function
// is exported at ordinal 1 and is not a member of any class.

EXPORT_C CMessenger* NewMessenger()
	{
	return new (ELeave) CGermanMessenger;
	}

// Class member functions

// second-phase constructor
void CGermanMessenger::ConstructL(CConsoleBase* aConsole, const TDesC& aName)
	{
	iConsole=aConsole;    // remember console
	iName=aName.AllocL(); // copy given string into own descriptor
    }

// destructor
CGermanMessenger::~CGermanMessenger()
		{
		delete iName;
		}

// show message
void CGermanMessenger::ShowMessage()
	{
	_LIT(KFormat1,"Guten Tag, Ich heisse %S \n");
	iConsole->Printf(KNullDesC);
	iConsole->Printf(KFormat1, iName);
	}
