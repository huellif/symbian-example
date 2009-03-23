// SigCheck.cpp
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.

//	This example, shows how the signature of a resource file is
//	checked before it is used.


#include "CommonToResourceFilesEx.h"
#include <SigCheck.rsg> // resources


// Do the example
LOCAL_C void doExampleL()
    {
	_LIT(KFormat,"%S\n");

		// Pointer to a resource file
	RResourceFile resourceFile; 
		// open resource file on the emulator(__WINS__  is defined for the Windows emulator)
		//(leave if error)
	#if defined(__WINS__)
	_LIT(KZSystemDataSigRsc,"Z:\\Resource\\apps\\SigCheck.rsc");
	resourceFile.OpenL(fsSession, KZSystemDataSigRsc);
	#endif

		// open a resource file on the target phone
		// ( __EPOC32__ is defined for all target hardware platforms regardless of processor type/hardware architecture)
	#if defined(__EPOC32__)
	_LIT(KCSystemDataSigRsc,"C:\\Resource\\apps\\SigCheck.rsc");
	resourceFile.OpenL(fsSession, KCSystemDataSigRsc);
	#endif
		// check resource file signature
	TInt version=23; // version 023
	resourceFile.ConfirmSignatureL(version);

		// confirm, and leave if error
		// read resource from file
	HBufC8* dataBuffer=resourceFile.AllocReadLC(R_BASE_HELLO);
	TResourceReader reader;
	reader.SetBuffer(dataBuffer);
	TPtrC textdata = reader.ReadTPtrC();
		// write string to test console
	console->Printf(KFormat, &textdata);
		// clean up data buffer
	CleanupStack::PopAndDestroy(); // finished with dataBuffer
		// close resource file
	resourceFile.Close(); // finished with resourceFile
	}

