//
// Proxy.cpp
// Copyright © 1997-2001 Symbian Ltd. All rights reserved.
//

#include <e32std.h>
#include <ImplementationProxy.h>

#include "ExampleResolver.h"


// Map the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10009DD0,	CExampleResolver::NewL)
	};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

