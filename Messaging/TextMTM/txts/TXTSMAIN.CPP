// TXTSMAIN.CPP
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#include <e32std.h>
#include "txtspan.h"

GLDEF_C void gPanic(TTxtsPanic aPanic)
	{
	_LIT(KTXTSPanic,"TXTS");
	User::Panic(KTXTSPanic,aPanic);
	}
