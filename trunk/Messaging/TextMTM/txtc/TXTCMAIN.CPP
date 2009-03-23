// TXTCMAIN.CPP
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#include <e32std.h>
#include "txtcpan.h"

GLDEF_C void gPanic(TTxtcPanic aPanic)
	{
	_LIT(KTxtCPanic,"TXTC");
	User::Panic(KTxtCPanic,aPanic);
	}
