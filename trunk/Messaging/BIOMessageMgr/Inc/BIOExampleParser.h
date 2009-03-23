// BIOExampleParser.h
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#if !defined(__BIOEXAMPLEPARSER_H__)
#define __BIOEXAMPLEPARSER_H__


#include <bsp.h>
#include <msventry.h>


class CBIOExampleParser : public CBaseScriptParser2
{
public:
	IMPORT_C static CBIOExampleParser* NewL(CRegisteredParserDll& aRegisteredParserDll, CMsvEntry& aEntry, RFs& aFs);
	~CBIOExampleParser();
	void ParseL(TRequestStatus& aStatus, const TDesC& aSms);
	void ProcessL(TRequestStatus& aStatus);

private:
    void DoCancel();
    void RunL();

    CBIOExampleParser(CRegisteredParserDll& aRegisteredParserDll, CMsvEntry& aEntry, RFs& aFs);
    void ConstructL();
};

#endif /*__BIOEXAMPLEPARSER_H__*/
