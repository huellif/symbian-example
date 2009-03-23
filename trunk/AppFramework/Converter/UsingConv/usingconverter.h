// CUsingConverter.h
//
// Copyright (c) 2006 Symbian  Software Ltd.  All rights reserved.
//
//
// @file
// This contains Class CUsingConverter
#if (!defined __USINGCONVRTER_H__)
#define __USINGCONVRTER_H__

#include <concnf.h>
#include <conlist.h>

class CUsingConverter : public CBase
	{
public:
	CUsingConverter();
	~CUsingConverter();
	void EgConverterL();
private:
	void ConvertFileL(CCnaConverterList& aList, TInt aConverterUid, const TDesC& aInputFile, const TDesC& aOutputFile);
private:
	RFs iFs;
	};
#endif
