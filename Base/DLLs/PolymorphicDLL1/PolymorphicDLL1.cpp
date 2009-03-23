// PolymorphicDLL1.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

// This program creates a polymorphic interface DLL which
// executable "eulibdru" dynamically linkes to.
//

#include "PolymorphicDLL1.h"
#include <e32def.h>
#include <e32uid.h>



// Function to construct a CMessenger object. Note that this function
// is exported at ordinal 1 and is not a member of any class.

EXPORT_C CMessenger* NewMessenger()
	{
	return new (ELeave) CFrenchMessenger;
	}

// Class member functions

// second-phase constructor
void CFrenchMessenger::ConstructL(CConsoleBase* aConsole, const TDesC& aName)
	{
	iConsole=aConsole;    // remember console
	iName=aName.AllocL(); // copy given string into own descriptor
    }

// destructor
CFrenchMessenger::~CFrenchMessenger()
		{
		delete iName;
		}

// show message
void CFrenchMessenger::ShowMessage()
	{
	_LIT(KFormat1,"Bonjour, Je m'appelle %S \n");
	iConsole->Printf(KNullDesC);
	iConsole->Printf(KFormat1, iName);
	}
