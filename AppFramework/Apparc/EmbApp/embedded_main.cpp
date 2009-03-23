// embedded_main.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#include "embedded.h"
#include <eikstart.h>

// DLL entry point function.
TInt E32Dll()
	{
	return KErrNone;
	}

// Creates and returns an instance of the CApaApplication-derived class.
static CApaApplication* NewApplication()
	{
	return new CExampleApplication;
	}

// Map the interface UIDs
static const TImplementationProxy ImplementationTable[]=
	{
	IMPLEMENTATION_PROXY_ENTRY(0xE800008F, NewApplication)
	};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount=sizeof(ImplementationTable)/sizeof(ImplementationTable[0]);
	return ImplementationTable;
	}
