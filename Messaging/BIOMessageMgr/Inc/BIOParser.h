// BIOParser.h
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#ifndef __BIOPARSER_H__
#define __BIOPARSER_H__


// system Includes
#include <e32base.h>
#include <bsp.h>
#include <biodb.h>
#include <msvreg.h>
#include <MSVIDS.h>
#include <regpsdll.h>

#include "Bioexampleparser.h"

const TUid	KUidBIOVCardMsg = {0x10005534};    // VCard contact entries
_LIT(KBifDir,"C:\\" );


class CBioParser : CBase
{
public:
	static CBioParser* NewL(CMsvEntry* aEntry);
	~CBioParser();

	void ParserL();

private:
	CBIOExampleParser* CreateParserL();
	void ExtractMessageBodyL();
	CBioParser(CMsvEntry* aEntry);
	void ConstructL();

public:
	CRegisteredParserDll*	iRegisteredParserDll;
	CBIODatabase*			iBioDb;
	RFs 					iFs;
	CMsvEntry*				iMsvEntry;
	HBufC*					iMessageBody;
};

#endif /*__BIOPARSER_H__*/
