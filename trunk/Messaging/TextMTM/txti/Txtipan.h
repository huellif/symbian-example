// txtipan.h
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#if !defined(__TXTIPAN_H__)
#define __TXTIPAN_H__

//
//	TTxtiMtmUdPanic: MTM panics
//

enum TTxtiMtmUdPanic
	{
	ETxtiMtmUdWrongMtm,
	ETxtiMtmUdFoldersNotSupported,
	ETxtiMtmUdAttachmentsNotSupported,
	ETxtiMtmUdNoIconForAttachment,
	ETxtiMtmUdRootEntryGiven,
	ETxtiMtmUdUnknownOperationId
	};

extern void Panic(TTxtiMtmUdPanic aPanic);

#endif
