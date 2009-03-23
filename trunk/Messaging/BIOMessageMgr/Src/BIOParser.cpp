// BIOParser.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#include "BIOParser.h"


CBioParser::CBioParser(CMsvEntry* aEntry)
		:iMsvEntry(aEntry)
	{

	}

CBioParser::~CBioParser()
	{
	delete iMessageBody;
	if(iRegisteredParserDll)
		{
		delete iRegisteredParserDll;
		iRegisteredParserDll = NULL;
		}
	iFs.Close( );
	delete iBioDb;
	}

CBioParser* CBioParser::NewL(CMsvEntry* aEntry)
	{
	CBioParser* self= new (ELeave) CBioParser(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CBioParser::ConstructL()
	{
	iFs.Connect();
	iFs.SetSessionPath(KBifDir);
	iBioDb = CBIODatabase::NewL(iFs);

	}

CBIOExampleParser* CBioParser::CreateParserL()
	{
	TUid messageUid;
	messageUid.iUid = iMsvEntry->Entry().iBioType;

	TFileName parserDllName(iBioDb->GetBioParserNameL(messageUid));

	if (iRegisteredParserDll)
		{
		delete iRegisteredParserDll;
		iRegisteredParserDll = NULL;
		}

    iRegisteredParserDll = CRegisteredParserDll::NewL(parserDllName);

    RLibrary parserlibrary;
    User::LeaveIfError(iRegisteredParserDll->GetLibrary(iFs, parserlibrary));

    typedef CBIOExampleParser* (*NewParserL)(CRegisteredParserDll& aRegisteredParserDll, CMsvEntry& aEntry, RFs& aFs);

    TInt entrypointordinalnumber=1;
    TLibraryFunction libFunc=parserlibrary.Lookup(entrypointordinalnumber);
    if (libFunc==NULL)
        User::Leave(KErrBadLibraryEntryPoint);
    NewParserL pFunc=(NewParserL) libFunc;
    TInt refcount=iRegisteredParserDll->DllRefCount();
    CBIOExampleParser* parser=NULL;
    TRAPD(ret,parser=((*pFunc)(*iRegisteredParserDll, *iMsvEntry, iFs)));
    if ((ret!=KErrNone) && (iRegisteredParserDll->DllRefCount()==refcount))
        iRegisteredParserDll->ReleaseLibrary();

	User::LeaveIfError(ret);

    return parser;
	}

void CBioParser::ParserL()
	{
	ExtractMessageBodyL();

	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	wait->iStatus = KRequestPending;
	wait->Start();

	CBIOExampleParser* parser = CreateParserL();
	// Parsing the message
	parser->ParseL(wait->iStatus, *iMessageBody);
	parser->Cancel();
	delete parser;


	CleanupStack::PopAndDestroy();  // wait
	}

/**
 Get the message body
*/
void CBioParser::ExtractMessageBodyL()
	{

	CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL();
    CleanupStack::PushL(paraFormatLayer);
    CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL();
    CleanupStack::PushL(charFormatLayer);
    CRichText* richText = CRichText::NewL(paraFormatLayer, charFormatLayer);
    CleanupStack::PushL(richText);

    CMsvStore* store = iMsvEntry->ReadStoreL();
	CleanupStack::PushL(store);

    if (!store->HasBodyTextL())
    	User::Leave(KErrNotFound);

    store->RestoreBodyTextL(*richText);

	TInt messageLength = richText->DocumentLength();
	if (iMessageBody)
		{
		delete iMessageBody;
		iMessageBody = NULL;
		}
	iMessageBody = HBufC::NewL(messageLength);

	TPtr messDes = iMessageBody->Des();
	TInt length = messDes.Length();
	while (length < messageLength)
		{
		TPtrC desc = richText->Read(length, messageLength-length);
		messDes.Append(desc);
		length+=desc.Length();
		}
	CleanupStack::PopAndDestroy(4, paraFormatLayer); // store, text, charFormatLayer, paraFormatLayer
	}


