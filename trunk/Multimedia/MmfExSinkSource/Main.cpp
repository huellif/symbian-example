//
// main.cpp
// Copyright 1997-2005 Symbian Software Ltd.  All rights reserved.
//

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

#include <ImplementationProxy.h>
#include "mmfexdes.h"
#include "MmfExSinkSourceUIDs.hrh"

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KDescriptorSourceUID,	CMMFExDescriptor::NewSourceL),
		IMPLEMENTATION_PROXY_ENTRY(KDescriptorSinkUID,	CMMFExDescriptor::NewSinkL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

