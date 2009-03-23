// TXTCPAN.H
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#if !defined(__TXTCPAN_H__)
#define __TXTCPAN_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

//
//	TTxtcPanic: MTM panics
//
enum TTxtcPanic
	{
	ETxtcNoBodyText,
	ETxtcBadMtmTypeUid,
	ETxtcNoCMsvEntrySet,
	ETxtcEntryTypeNotSupported,
	ETxtcCommandNotSupported,
	ETxtcInvalidServiceId,
	};

GLREF_C void gPanic(TTxtcPanic aPanic);

#endif
