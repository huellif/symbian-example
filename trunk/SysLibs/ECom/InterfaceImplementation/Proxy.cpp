//
// Proxy.cpp
// Copyright © 1997-2001 Symbian Ltd. All rights reserved.
//

#include <e32std.h>
#include <ImplementationProxy.h>

#include "CImplementationClassOne.h"
#include "CImplementationClassTwo.h"

// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10009DC3,	CImplementationClassOne::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x10009DC4,	CImplementationClassTwo::NewL)
	};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

