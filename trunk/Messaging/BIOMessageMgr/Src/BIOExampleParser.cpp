// BIOExampleParser.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//


#include <e32std.h>
#include <eikenv.h>

#include "Bioexampleparser.h"

CBIOExampleParser::CBIOExampleParser(CRegisteredParserDll& aRegisteredParserDll, CMsvEntry& aEntry, RFs& aFs)
:CBaseScriptParser2(aRegisteredParserDll, aEntry, aFs)
    {
    }

CBIOExampleParser::~CBIOExampleParser()
	{
	Cancel();
	}
	
EXPORT_C CBIOExampleParser* CBIOExampleParser::NewL(CRegisteredParserDll& aRegisteredParserDll, CMsvEntry& aEntry, RFs& aFs)
	{
    CBIOExampleParser* self = new(ELeave) CBIOExampleParser(aRegisteredParserDll, aEntry, aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
	}

void CBIOExampleParser::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

/**
Parsing the message.
@param aStatus An TRequestStatus reference, used to return the status
@param aSms An TDesC reference
*/
void CBIOExampleParser::ParseL(TRequestStatus& aStatus, const TDesC& /*aSms*/)
	{
	aStatus = iStatus;
	SetActive();

	TMsvEntry entry = iEntry.Entry();
	entry.iMtmData3 = 1;
	iEntry.ChangeL(entry);

	TRequestStatus* status = &aStatus ;
	User::RequestComplete(status, KErrNone);
	}

/**
Processing the message.
@param aStatus An TRequestStatus reference, used to return the status
*/
void CBIOExampleParser::ProcessL(TRequestStatus& aStatus)
	{
	aStatus = iStatus;
	SetActive();

	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CBIOExampleParser::DoCancel()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrCancel);
	}

void CBIOExampleParser::RunL()
	{
	}


