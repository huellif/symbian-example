// TXTSPAN.H
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#if !defined(__TXTSPAN_H__)
#define __TXTSPAN_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

//
//	TTxtsPanic: MTM panics
//
enum TTxtsPanic
	{
	ETxtsNoBodyText,
	ETxtsInvalidFileName,
	ETxtsInvalidService,
	ETxtsStrayRequest,
	ETxtsInvalidNullOperation,
	ETxtsInvalidDestination,
	ETxtsAlreadyBusy,
	ETxtsInvalidChangeDetails,
	ETxtsInvalidEntryIndex,
	};

GLREF_C void gPanic(TTxtsPanic aPanic);

#endif
