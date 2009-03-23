// SendAs2Example.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//
/** 
@file
This is a simple example that demonstrates the use of the SendAs v2 protocol to send messages 
*/
#include "SendAs2Example.h"

_LIT(KBodyText, "You are limited only by your fears. Remember it is attitude, more than aptitude,that determines your altitude");
_LIT(KAddress1, "ban-sindhub01@ban-sindhub01.intra");
_LIT(KAddress3, "ban-sindhub01@ban-sindhub01.intra");
_LIT(KAddress4, "test1@symbian.com");
_LIT(KAlias1, "Alias1");
_LIT(KAlias2, "Alias2");
_LIT(KAccount1, " \n SendAs Account1 \n");
_LIT(KAccount2, " \n SendAs Account2 \n");
_LIT(KAccount3, " \n SendAs Account3 \n");
_LIT(KAccountNames,"\n The list of accounts present are: \n");
_LIT(KTextWelcome, " SendAs2 Example \n");
_LIT(KTextStartApp, "\n Starting the SendAs2 Example application .... \n");
_LIT(KTextPressAKey, "\n Press any key to step through the example .... \n");
_LIT(KTextConn, "\n Connecting to sendAs server .... \n");
_LIT(KTextErrInn, "\n Connection is successful\n");
_LIT(KTextErrOut, "\n\n Connection has failed\n");	
_LIT(KTextCreate, "\n Creating a SendAs message .... \n");
_LIT(KTextSend, "\n Sending the sendAs message .... \n");
_LIT(KTextClose, "\n The SendAs message has been sent \n");
_LIT(KPressAnyKey, "\n Press any key.... \n");
_LIT(KExitKey, "\n Press any key to exit the application.... \n");
_LIT(KSmsClient, "SMS Client");

/**
Allocates and constructs a CSendAs2Example object.
Initialises all member data to their default values.
*/	
CSendAs2Example* CSendAs2Example::NewL()
	{
	CSendAs2Example* self = new (ELeave) CSendAs2Example();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
/**
Constructor
*/	
CSendAs2Example::CSendAs2Example()
	{	
		
	}

/**
Creating a console window
*/
void CSendAs2Example::ConstructL()
	{
	iConsole = Console::NewL(KTextWelcome,TSize(KConsFullScreen,KConsFullScreen));
	iConsole->Printf ( KTextStartApp );
	iConsole->Printf ( KTextPressAKey );
	iConsole->Getch ();
	}	
	
/**
Cleans the application
	- closes the sendAs server
	- deletes the console window
	- deletes all other member variables
*/
CSendAs2Example::~CSendAs2Example()
	{	
	delete iSelection;
	iSelection = NULL;
	
	delete iEntry;
	iEntry = NULL;
	
	delete iSession;
	iSession = NULL;
	
	delete iObserver;
	iObserver = NULL;
	
	iSendAs.Close();	
	delete iConsole;
	}
	
/**
Main function which calls other functions.
Creates a session observer and sets the entry on drafts folder.
Also, gets a selection containing the IDs of all the context children.
@leave KErrNoMemory, system wide error codes	
*/
void CSendAs2Example::StartL()
	{
	iObserver = new(ELeave) CDummyObserver;
	iSession = CMsvSession::OpenSyncL(*iObserver);

	TMsvId aFolderId = KMsvDraftEntryId;
	iEntry = CMsvEntry::NewL(*iSession, aFolderId, 
		TMsvSelectionOrdering(KMsvNoGrouping,EMsvSortByNone,ETrue));
	
	iSelection = iEntry->ChildrenL();

	Connect();
	RSendAsMessage message;
	CleanMessageFolderL();
	CreateL(message);
	DisplayAccountL();
	CapabilityFilterL();
	SendL(message);
	}

/**
Creates a session
	- based on the time slots
	- connects to the server
	@leave system wide error codes
*/
void CSendAs2Example::Connect()
	{
	iConsole->Printf ( KTextConn );
	TInt err = iSendAs.Connect(KSendAsDefaultMessageSlots);
	if ( err == KErrNone)
		{
		iConsole->Printf ( KTextErrInn );
		}
	else
		{
		iConsole->Printf ( KTextErrOut );
		}
	}

/**
Clears the message folder passed in the entry before sending any new messages.
*/
void CSendAs2Example::CleanMessageFolderL()
	{
	TMsvLocalOperationProgress progress;
	if (iSelection->Count() > 0)
		{
		iEntry->DeleteL(*iSelection, progress);
		}	
	}

/**
Creates a SendAs message
@param aMessage An RSendAsMessage reference, used to create a message
*/
void CSendAs2Example::CreateL( RSendAsMessage& aMessage)
	{
		
	CSendAsMessageTypes* messageTypes = CSendAsMessageTypes::NewL();
	CleanupStack::PushL(messageTypes);	

	// Filter all MTMs that can send messages. 
	// This list can be refined by applying filters using FilterAgainstCapability.
	iSendAs.FilteredMessageTypesL(*messageTypes);

	TUid sendAsMtmUid;
		
	// Returning the message UID based on the message type 
	// such as 0 for SMTP Client, 1 for SMS Client
	// 2 for Infrared client MTM and 3 for Bluetooth client MTM.
	sendAsMtmUid = messageTypes->UidFromNameL(KSmsClient);
	
	CleanupStack::PopAndDestroy(messageTypes);	
	iConsole->Printf ( KTextCreate );
	
	TInt count = iSelection->Count();
	iEntry->SetEntryL(KMsvDraftEntryId);
	
	aMessage.CreateL(iSendAs, sendAsMtmUid);
	// Set the body text of this message using a plain descriptor. 
	// The object must have an open message to use this method.
	aMessage.SetBodyTextL(KBodyText);
	// Add recipients to this message.
	aMessage.AddRecipientL(KAddress1, KAlias1, RSendAsMessage::ESendAsRecipientTo);
	aMessage.AddRecipientL(KAddress3, RSendAsMessage::ESendAsRecipientCc);
	aMessage.AddRecipientL(KAddress4, KAlias2, RSendAsMessage::ESendAsRecipientCc);

	CMsvEntry* entry = CMsvEntry::NewL(*iSession, KMsvDraftEntryId, 
		TMsvSelectionOrdering(KMsvNoGrouping,EMsvSortByNone,ETrue));
	CleanupStack::PushL(entry);
	
	// Sets the context to the specified entry.
	entry->SetEntryL(KMsvDraftEntryId);
	if(iSelection->Count() >= 1)
		{
		entry->SetEntryL((*iSelection)[0]);
		};
		
	CleanupStack::PopAndDestroy(entry);
	}
	
/**
Displays the names of all the available accounts.
Creates accounts, if not already created.
*/
void CSendAs2Example::DisplayAccountL()
	{
	CSendAsAccounts* accounts = CSendAsAccounts::NewL();
	CleanupStack::PushL(accounts);
	
	// Append 3 new accounts and associated name pair.
	if(accounts->Count()==0)
		{
		accounts->AppendAccountL(KAccount1, 0xaaaaaaaa);	
		}
	
	if(accounts->Count()==1)
		{
		accounts->AppendAccountL(KAccount2, 0x55555555);	
		}
		
	if(accounts->Count()==2)
		{
		accounts->AppendAccountL(KAccount3, 0x22222222);	
		}
	iConsole->Printf( KAccountNames );
	iConsole->Printf ( KPressAnyKey );
	iConsole->Getch ();
	
	// Display the array of names of accounts for this message type
	for(TInt i = 0; i < 3; i++)
		{
		TPtrC array = accounts->AccountNames().MdcaPoint(i);
		iConsole->Printf( array );	
		}
		
	CleanupStack::PopAndDestroy(accounts);
	
	}

/**
Refines the connected session's available message types. 
Applying a filter will remove all mtms which do not support the supplied capability
@leave system wide error codes
*/
void CSendAs2Example::CapabilityFilterL()
	{
	// Get a list of MTMs available to send the message
	// By specifying the features that the MTMs must have.
	// Eg: set filter to test that KUidMtmQueryMaxBodySize >= bodySize
	User::LeaveIfError(iSendAs.FilterAgainstCapability(KUidMtmQuerySupportAttachments));
	User::LeaveIfError(iSendAs.FilterAgainstCapability(KUidMtmQueryMaxBodySize, 0x10, RSendAs::ESendAsGreaterThan));
	User::LeaveIfError(iSendAs.FilterAgainstCapability(KUidMtmQuerySupportedBody, KMtm16BitBody, RSendAs::ESendAsBitwiseAnd));
	User::LeaveIfError(iSendAs.FilterAgainstCapability(KUidMtmQuerySupportedBody, KMtm16BitBody, RSendAs::ESendAsBitwiseAnd));
	User::LeaveIfError(iSendAs.FilterAgainstCapability(KUidMtmQuerySupportSubject));
	User::LeaveIfError(iSendAs.FilterAgainstCapability(KUidMtmQuerySupportsFolder));
	User::LeaveIfError(iSendAs.FilterAgainstCapability(KUidMtmQueryMaxRecipientCount));
	}
	
/**
Sends the sendAs2 message
@param aMessage An RSendAsMessage reference, used to send a message
*/
void CSendAs2Example::SendL(RSendAsMessage& aMessage)
	{
	iConsole->Printf ( KTextSend );
	iConsole->Printf ( KPressAnyKey );
	iConsole->Getch ();
	
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	wait->iStatus = KRequestPending;
	wait->Start();
	// Asynchronously send the message.
	// This will only be allowed to happen if the caller 
	// holds sufficient capabilities to perform this action. 
	// If the caller does not hold these capabilities, then 
	// the message send will be automatically demoted to a confirmed send.
	aMessage.SendMessage(wait->iStatus);
	
	CleanupStack::PopAndDestroy(wait);
	iConsole->Printf ( KTextClose );
	iConsole->Printf ( KExitKey );
	iConsole->Getch ();
	}

LOCAL_C void MainL()
	{
	// Creates an Active Scheduler to handle asychronous calls.
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	/*
	If an Active Scheduler has been created, installs it .
	As it is an asychronous call we need to install it explicitly.
	*/
	CActiveScheduler::Install( scheduler );
	
	CSendAs2Example* app = CSendAs2Example::NewL();
	CleanupStack::PushL(app);
	// Calls the main function in which other functions are called.
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