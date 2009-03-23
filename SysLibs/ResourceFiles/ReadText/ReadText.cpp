// ReadText.cpp
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.

// This example writes the text "hello world!" from a resource file to the console
// Use it as the basis of all examples which depend on BAFL

#include "CommonToResourceFilesEx.h"
#include <ReadText.rsg>

	// do the example
LOCAL_C void doExampleL()
    {
	_LIT(KFormat,"%S\n");

	// open resource file
	RResourceFile resourceFile; // resource file pointer
	// open  file on the emulator(__WINS__  is defined for the Windows emulator)
	// (leave if error)
	#if defined(__WINS__)
	_LIT(KZSystemDataHelloRsc,"Z:\\Resource\\apps\\ReadText.rsc");
	resourceFile.OpenL(fsSession, KZSystemDataHelloRsc);
	#endif

	// open file on the target phone
	// ( __EPOC32__ is defined for all target hardware platforms regardless of processor type/hardware architecture)
	#if defined(__EPOC32__)
	_LIT(KCSystemDataHelloRsc,"C:\\Resource\\apps\\ReadText.rsc");
	resourceFile.OpenL(fsSession, KCSystemDataHelloRsc);
	#endif


	HBufC8* dataBuffer=resourceFile.AllocReadLC(HELLO);
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
