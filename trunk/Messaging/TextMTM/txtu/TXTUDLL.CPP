// TXTUDLL.CPP
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#include <E32STD.H>
#include "TXTU.H"
#include "TXTUPAN.H"

// Global function declarations

GLDEF_C void Panic(TTxtuMtmUdPanic aPanic)
// Panic function
	{
	_LIT(KTEXTMTMUIPanic,"TEXT MTMUI");
	User::Panic(KTEXTMTMUIPanic, aPanic);
	}

EXPORT_C CBaseMtmUi* NewMtmUiL(CBaseMtm& aMtm, CRegisteredMtmDll& aRegisteredDll)
// Factory function
	{
	return CTextMtmUi::NewL(aMtm, aRegisteredDll);
	}
