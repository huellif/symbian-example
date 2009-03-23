// BIOMessage.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#include "BIOMessage.h"


CBioMessage::CBioMessage()
	{	
		
	}

/**
Cleaning the application
	- closing the sendAs server
	- deleting the console window
*/
CBioMessage::~CBioMessage()
	{	
	iSendAs.Close();
	delete iConsole;
	}

CBioMessage* CBioMessage::NewL()
	{
	CBioMessage* self = new (ELeave) CBioMessage();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
Creating a console window
*/
void CBioMessage::ConstructL()
	{
	
	iConsole = Console::NewL(KTestTitle,TSize(KConsFullScreen,KConsFullScreen));
	_LIT(KTextWelcome, "### BIO messaging ### \n");
	iConsole->Printf ( KTextWelcome );

	_LIT(KTextStartApp, "\n Starting the BIO message application ............ \n");
	iConsole->Printf ( KTextStartApp );
	
	_LIT(KTextPressAKey, "\n Press any key to step through the example ............ \n");
	iConsole->Printf ( KTextPressAKey );
	iConsole->Getch ();

	_LIT(KTextInit, "\n #1. Intializing ............ \n");
	iConsole->Printf ( KTextInit );
	
	iConsole->Getch ();	
		
	}	

void CBioMessage::StartL()
	{
	Connect();
	RSendAsMessage message;
	CreateL(message);
	SendL(message);
	
	_LIT(KTextParserTitle, " \n\n### BIO Message Parsing ### \n");
	iConsole->Printf ( KTextParserTitle );
	
	ParseL();	
	CleanupStack::PopAndDestroy(4, iObserver); //selection, iEntry, iSession, iObserver
	}

/**
Creating a session
	- based on the time slots
	- connecting to the server
*/
void CBioMessage::Connect()
	{
	_LIT(KTextConn, "\n #2. Connecting to message server ............ \n");
	iConsole->Printf ( KTextConn );
	iConsole->Getch ();

	TInt err = iSendAs.Connect(KSendAsDefaultMessageSlots);
	if ( err == KErrNone)
		{
		_LIT(KTextErrInn, "\n Connection is successful\n");
		iConsole->Printf ( KTextErrInn );

		}
	else
		{
		_LIT(KTextErrOut, "\n\n Connection has failed\n");
		iConsole->Printf ( KTextErrOut );
		}
	}

void CBioMessage::CleanMailFolder(TMsvId aFolderId)
	{
	CDummyObserver* observer = new(ELeave) CDummyObserver;
	CleanupStack::PushL(observer);
	CMsvSession* session = CMsvSession::OpenSyncL(*observer);
	CleanupStack::PushL(session);	
	CMsvEntry* entry = CMsvEntry::NewL(*session, aFolderId, 
		TMsvSelectionOrdering(KMsvNoGrouping,EMsvSortByNone,ETrue));
	CleanupStack::PushL(entry);
	
	CMsvEntrySelection* selection = entry->ChildrenL();
	CleanupStack::PushL(selection);

	TMsvLocalOperationProgress progress;
	if (selection->Count() > 0)
		{
		entry->DeleteL(*selection, progress);
		}

	CleanupStack::PopAndDestroy(4); // selection, entry, session, observer
	}

/**
Creating a BIO message
@param aMessage An RSendAsMessage reference, used to create a message
*/
void CBioMessage::CreateL( RSendAsMessage& aMessage)
	{
	CSendAsMessageTypes* messageTypes = CSendAsMessageTypes::NewL();
	CleanupStack::PushL(messageTypes);	

	iSendAs.FilteredMessageTypesL(*messageTypes);

	TUid sendAsMtmUid;
		
	// Returning the message UID based on the message type 
	// such as 0 for SMTP Client, 1 for SMS Client, 2 for Infrared client MTM and 3 for Bluetooth client MTM.
	sendAsMtmUid = messageTypes->UidFromNameL(_L("SMS Client"));
	
	CleanupStack::PopAndDestroy(messageTypes);	

	_LIT(KTextCreate, "\n #3. Creating a BIO message ............ \n");
	iConsole->Printf ( KTextCreate );
	iConsole->Getch ();
	
	CleanMailFolder(KMsvDraftEntryId);
	
	iObserver = new(ELeave) CDummyObserver;
	CleanupStack::PushL(iObserver);
	iSession = CMsvSession::OpenSyncL(*iObserver);
	CleanupStack::PushL(iSession);
	iEntry = CMsvEntry::NewL(*iSession, KMsvDraftEntryId, 
	TMsvSelectionOrdering(KMsvNoGrouping,EMsvSortByNone,ETrue));
	CleanupStack::PushL(iEntry);

	iSelection = iEntry->ChildrenL();
	CleanupStack::PushL(iSelection);
	
	TInt count = iSelection->Count();
	if ( count == 0) // Check no message
		{
		CleanupStack::PopAndDestroy(iSelection);
		iSelection = NULL;
		}	

	aMessage.CreateL(iSendAs, sendAsMtmUid);
	aMessage.SetBodyTextL(KBodyText);
	aMessage.AddRecipientL(KAddress1, KAlias1, RSendAsMessage::ESendAsRecipientTo);
	aMessage.AddRecipientL(KAddress3, RSendAsMessage::ESendAsRecipientCc);
	aMessage.AddRecipientL(KAddress4, KAlias2, RSendAsMessage::ESendAsRecipientCc);

	TUid bioType = KUidBIOVCardMsg;
	aMessage.SetBioTypeL(bioType);	
	iEntry->SetEntryL(KMsvSentEntryId);
	
	
		
	iEntry->SetEntryL(KMsvDraftEntryId);
	iSelection = iEntry->ChildrenL();
	CleanupStack::PushL(iSelection);	
	if(iSelection->Count() >= 1)
		{
		iEntry->SetEntryL((*iSelection)[0]);
		}
	}

/**
Sending the BIO message and closing
@param aMessage An RSendAsMessage reference, used to send a message
*/
void CBioMessage::SendL(RSendAsMessage& aMessage)
	{
	_LIT(KTextSend, "\n #4. Sending the BIO message ............ \n");
	iConsole->Printf ( KTextSend );
	iConsole->Getch ();
	
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	wait->iStatus = KRequestPending;
	wait->Start();
	
	aMessage.SendMessageAndCloseL();
	
	CleanupStack::PopAndDestroy();  // wait
	
	_LIT(KTextClose, "\n The BIO message has been sent \n");
	iConsole->Printf ( KTextClose );
	iConsole->Getch ();
	}

/**
Parsing the BIO message
*/
void CBioMessage::ParseL()
	{
	_LIT(KTextParse, "\n #5. Parsing the BIO message ............ \n");
	iConsole->Printf ( KTextParse );
	iConsole->Getch ();
	
	CBioParser* bioParser = CBioParser::NewL(iEntry);
	bioParser->ParserL();
	
	_LIT(KTextParsed, "\n The BIO message has been parsed \n");
	iConsole->Printf ( KTextParsed );
	iConsole->Getch ();
	
	delete bioParser;
	}

LOCAL_C void MainL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install( scheduler );
	
	CBioMessage* app = CBioMessage::NewL();
	CleanupStack::PushL(app);
	app->StartL();
	CleanupStack::PopAndDestroy(app);
	
	delete scheduler;
	}

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(cleanup == NULL)
    	{
    	return KErrNoMemory;
    	}
    TRAPD(err, MainL());
	if(err != KErrNone)
		{	
		User::Panic(_L("failed to complete"),err);
		}

    delete cleanup;
    __UHEAP_MARKEND;
    return KErrNone;
	}
