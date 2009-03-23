// BasicSession.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

// This example opens the file server session, and prints 
//"Hello File Server!" to the console.
// Used as the basis of all F32 examples

// Note that no data is written.

#include "CommonFramework.h"
#include <f32file.h>
	

LOCAL_D RFs fsSession;

// start of real example

LOCAL_C void doExampleL()
    {
	// open file server session
	User::LeaveIfError(fsSession.Connect()); // connect session
	// say hello
	_LIT(KGreeting,"Hello File Server!\n");
	console->Printf(KGreeting);
	fsSession.Close(); // close file server session
	}
