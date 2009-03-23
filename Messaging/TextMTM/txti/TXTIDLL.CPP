// TXTIDLL.CPP
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__TXTI_H__)
#include "txti.h"
#endif
#if !defined(__TXTIPAN_H__)
#include "txtipan.h"
#endif

// 
// Global function declarations
//

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason/* aReason*/)	
// DLL entry point
	{
	return (KErrNone);
	}
#endif

GLDEF_C void Panic(TTxtiMtmUdPanic aPanic)
// Panic function
	{
	_LIT(KTXTIPanic,"TXTI");
	User::Panic(KTXTIPanic, aPanic);
	}


EXPORT_C CBaseMtmUiData* NewMtmUiDataLayerL(CRegisteredMtmDll& aRegisteredDll)
// Factory function
	{
	return CTxtiMtmUiData::NewL(aRegisteredDll);
	}
