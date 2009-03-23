// MultiRead1.cpp
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.

/*
	This example, together with MultiRead2, shows how to use multiple
	resource files with cross-referenced resources.

    It introduces the class CMultipleResourceFileReader, capable of
	reading resources from multiple	resource files. However, only one
	resource file is used. The example MultipleReader2 uses more than one resource file
*/

#include "CommonToResourceFilesEx.h"

#include <MultiRead1.rsg> // resources
#include "MultiRead.h"

// Do the example
void doExampleL()
	{
	_LIT(KFormat,"%S\n");

	// allocate multi-reader
	CMultipleResourceFileReader* multiReader =
			CMultipleResourceFileReader::NewLC();

	// open resource file on the emulator(__WINS__  is defined for the Windows emulator)
	// (leave if error)
	#if defined(__WINS__)
	// add MultipleReader1 version 23
	_LIT(KZSystemDataBasigbRsc,"Z:\\Resource\\apps\\MultiRead1.rsc");
	multiReader->AddResourceFileL(KZSystemDataBasigbRsc,23);
	#endif

	// open a resource file on the target phone
	// ( __EPOC32__ is defined for all target hardware platforms regardless of processor type/hardware architecture)
	#if defined(__EPOC32__)
	// add MultipleReader1 version 23
	_LIT(KCSystemDataBasigbRsc,"C:\\Resource\\apps\\MultiRead1.rsc");
	multiReader->AddResourceFileL(KCSystemDataBasigbRsc,23);
	#endif

	// read string resource from file into a descriptor
	HBufC8* dataBuffer=multiReader->AllocReadLC(R_BASE_HELLO);
	TResourceReader reader;
	reader.SetBuffer(dataBuffer);
	TPtrC textdata = reader.ReadTPtrC();

	// write string to test console
	console->Printf(KFormat, &textdata);
	// clean up data buffer
	CleanupStack::PopAndDestroy(); // finished with dataBuffer

	// cleanup multi-reader
	CleanupStack::PopAndDestroy(); // multi-reader
	}

