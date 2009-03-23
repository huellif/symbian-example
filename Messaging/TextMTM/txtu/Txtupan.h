// TXTUPAN.H
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

//
//	TTxtuMtmUdPanic: MTM panics
//
enum TTxtuMtmUdPanic
	{
	ETextMtmUiSelectionIsNotOneMessage,
	ETextMtmUiOnlyWorksWithMessageEntries,
	ETextMtmUiEmptySelection,
	ETextMtmUiWrongMtm,
	ETextMtmUiWrongEntryType,
	ETextMtmUiInvalidNullPointer,
	};

extern void Panic(TTxtuMtmUdPanic aPanic);
