//
// main.cpp
// Copyright © 1997-2005 Symbian Software Ltd.  All rights reserved.
//

#include <e32base.h>
#include <ecom.h>
#include <ImplementationProxy.h>

#include "MMFExPcm8toPcm16codec.h"
#include "UIDs.hrh"

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KExCodecUID,	CMMFExPcm8Pcm16Codec::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

