// Pop3Example.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//
/** 
@file
This is a simple example code that demonstrates the usage of the POP3 protocol to retrieve emails.
The code creates a simple SMTP mail, sends it and then retrieves the same using POP. 
*/
#include "pop3Example.h"
#include "MAsyncWaiter.h"

_LIT(KTextPressAKey, "\n\n Press any key to step through the example");
_LIT(KExit,"\n\n Press any key to exit the application ");
_LIT(KCreateMtm,"\n\n Creating a Pop3 client MTM");
_LIT(KKeyPress,"\n\n Press any key");
_LIT(KTxtAccountName, "PopAccount");
_LIT(KCreatePopAccount,"\n\n Creating a Pop account");
_LIT(KCreateSmtpAccount,"\n\n Creating an Smtp account");
_LIT(KSmtpServerAddress, "ban-sindhub01.intra");
_LIT(KEmailAlias, "Messaging example");
_LIT(KSmtpEmailAddress, "ban-sindhub01@ban-sindhub01.intra");
_LIT8(KFrom, "ban-sindhub01@ban-sindhub01.intra");
_LIT(KTo, "ban-sindhub01@ban-sindhub01.intra");
_LIT(KSubject, "SimpleEmail");
_LIT(KBodyContents, "This is a very simple mail");
_LIT(KSendMail,"\n\n Sending the mail... please wait");
_LIT(KPopServer, "ban-sindhub01.intra");	
_LIT8(KPopPassword,"ban-sindhub01");
_LIT8(KPopLoginName,"ban-sindhub01");
_LIT(KWait,"\n\n Connecting to pop3 server");
_LIT(KDownload,"\n\n Downloading mail from the pop3 server");
_LIT(KDisconnect,"\n\n Press any key to disconnect");
	
/**
Allocates and constructs a CPop3Example object.
Initialises all member data to their default values.
*/	
CPop3Example* CPop3Example::NewL()
	{
	CPop3Example* self = new (ELeave) CPop3Example();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
/**
Constructor
*/
CPop3Example::CPop3Example()
	{

	}
	
void CPop3Example::ConstructL()
	{
	iConsole = Console::NewL(KTitle,TSize(KConsFullScreen,KConsFullScreen));
	iConsole->Printf ( KTextPressAKey );
	iConsole->Getch ();
	}

/**	
Destructor
*/
CPop3Example::~CPop3Example()
	{
	iConsole->Printf(KExit);
	iConsole->Getch();
	delete iMtm;
	iMtm = NULL;
	delete iClientRegistry;
	iClientRegistry  = NULL;
	delete iSession;		
	iSession = NULL;
	delete iConsole;
	}

/**
Starts a message server session, creates a client side mtm registry
and pop client mtm.
@leave KErrNoMemory, KErrNotFound, KErrBadLibraryEntryPoint
*/
void CPop3Example::CreateClientRegistryL()
	{
	// Create a message server session.
	iSession = CMsvSession::OpenSyncL(*this);
	CleanupStack::PushL(iSession);
	iSession->InstallMtmGroup(KDataComponentFileName);
	
	// Create a client-side MTM registry.
	iClientRegistry = CClientMtmRegistry::NewL(*iSession,KMsvDefaultTimeoutMicroSeconds32);
	if (iClientRegistry != NULL && iClientRegistry->IsPresent(KUidMsgTypePOP3))
		{
		CleanupStack::PushL(iClientRegistry);
		}
	// Create a Client-side MTM object for the specified MTM UID.
	iMtm = (CPop3ClientMtm*)iClientRegistry->NewMtmL(KUidMsgTypePOP3); 
	iConsole->Printf(KCreateMtm);
	iConsole->Printf(KKeyPress);
	iConsole->Getch();
	CleanupStack::Pop(2,iSession); //iClientRegistry,iSession
	 
    }

/**
Creates a pop account and an smtp account.
Sets the pop server address to system address and sets login name and password
@leave system wide error codes
*/
void CPop3Example::CreatePopAndSmtpAccountL()
	{
	CEmailAccounts* emailAccounts = CEmailAccounts::NewLC();
	CImPop3Settings* settings = new(ELeave) CImPop3Settings();
	CleanupStack::PushL(settings);
	
	CImIAPPreferences* popIAP = CImIAPPreferences::NewLC();
	
	// Set the server address to system address
	settings->SetServerAddressL(KPopServer);
	settings->SetLoginNameL(KPopLoginName); 
	settings->SetPasswordL(KPopPassword);
	settings->SetPort(110);
	
	// Create a Pop account
	iConsole->Printf(KCreatePopAccount);
	iPopAccount = emailAccounts->CreatePopAccountL(KTxtAccountName, *settings,*popIAP,EFalse);
	
	CImSmtpSettings *smtpSettings = new (ELeave) CImSmtpSettings();
	CleanupStack::PushL(smtpSettings);

	emailAccounts->PopulateDefaultSmtpSettingsL(*smtpSettings, *popIAP);
 	
 	// Create an Smtp acoount
	iConsole->Printf(KCreateSmtpAccount);
	iSmtpAccount = emailAccounts->CreateSmtpAccountL(iPopAccount, *smtpSettings, *popIAP, EFalse);
	
	emailAccounts->SetDefaultSmtpAccountL(iSmtpAccount);

	smtpSettings->SetServerAddressL(KSmtpServerAddress);
	smtpSettings->SetEmailAliasL(KEmailAlias);
	smtpSettings->SetEmailAddressL(KSmtpEmailAddress);
	smtpSettings->SetReplyToAddressL(KSmtpEmailAddress);
	smtpSettings->SetReceiptAddressL(KSmtpEmailAddress);
	smtpSettings->SetPort(25);
		
	// Add IAP to the IAP preferences
	CImIAPPreferences* prefs = CImIAPPreferences::NewLC();
	TImIAPChoice iap;
	TInt iapID = 0;
	CMDBSession* dbSession = CMDBSession::NewL(CMDBSession::LatestVersion());
	CleanupStack::PushL(dbSession);
	CCDConnectionPrefsRecord *connPrefRecord = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
	CleanupStack::PushL(connPrefRecord);
	
	// Set the direction of connection
	connPrefRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	connPrefRecord->iRanking = 1;
	if(!connPrefRecord->FindL(*dbSession))
		{
		User::Leave(KErrNotFound);		
		}			
	iapID = connPrefRecord->iDefaultIAP;
	iap.iIAP = iapID;
	iap.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	prefs->AddIAPL(iap);
	
	emailAccounts->GetSmtpAccountL(iSmtpAccount.iSmtpService, iSmtpAccount);
	emailAccounts->SaveSmtpSettingsL(iSmtpAccount,*smtpSettings);
	emailAccounts->SaveSmtpIapSettingsL(iSmtpAccount, *prefs);
	
	CleanupStack::PopAndDestroy(7,emailAccounts); //connPrefRecord,dbSession,prefs,smtpSettings,popIAP,settings,emailAccounts
	}
	
/**
Creates an smtp message,stores and then commits settings to a specified message store.
@leave KErrNoMemory,KErrNotFound,KErrAccessDenied,Standard
*/
void CPop3Example::CreateSmtpMessage() 
	{
	 
	TMsvId outboxId = KMsvGlobalOutBoxIndexEntryId; 
		
	// Set the context to the folder in which message has to be created
	CMsvEntry*	entry = CMsvEntry::NewL(*iSession,outboxId,TMsvSelectionOrdering());
	CleanupStack::PushL(entry);
	entry->SetEntryL(outboxId);
	
	CMessAsyncWaiter* waiter = CMessAsyncWaiter::NewL();
	CleanupStack::PushL(waiter);
	
	TMsvEmailTypeList msvEmailTypeList = 0;
	TMsvPartList partList = (KMsvMessagePartBody | KMsvMessagePartAttachments);
	
	CImEmailOperation* emailOperation = CImEmailOperation::CreateNewL(waiter->iStatus, *iSession,KMsvGlobalOutBoxIndexEntryId, partList, msvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);
	waiter->StartAndWait();
	
	TMsvId temp;
	TPckgC<TMsvId> paramPack(temp);
	const TDesC8& progBuf = emailOperation->ProgressL();
	paramPack.Set(progBuf);
	TMsvId newMessageId;
	newMessageId = paramPack();

	entry->SetEntryL(newMessageId);

	CMsvStore* store = entry->EditStoreL();
	CleanupStack::PushL(store);
	CImHeader* emailEntry = CImHeader::NewLC();
	emailEntry->RestoreL(*store);
	emailEntry->SetFromL((TDesC8&)KFrom);
	emailEntry->SetSubjectL((TDesC&)KSubject);
	emailEntry->ToRecipients().AppendL((TDesC&)KTo);
	
	// Paragraph format layer for the rich text object 
	CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);
	// Character format layer for the rich text object
	CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL(); 
	CleanupStack::PushL(charFormatLayer);

	CRichText* bodyText = CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(bodyText);

	// Inserts the contents of a buffer into the document at specified position
	bodyText->InsertL(0, KBodyContents);
	store->StoreBodyTextL(*bodyText);
	emailEntry->StoreL(*store);
	// Store the changes permanently
	store->CommitL();
	
	CleanupStack::PopAndDestroy(8,entry); // bodyText,charFormatLayer,paraFormatLayer,emailEntry,store,emailOperation,waiter,entry

	}
	
/**
Accesses the entry with the specified ID.
Sets the context to the specified entry.
Sends the created smtp mail.
@leave KErrNoMemory,KErrNotFound
*/
void CPop3Example::SendSMTPMessage()
	{
	CMsvEntry* entry = iSession->GetEntryL(iSmtpAccount.iSmtpService);
	CleanupStack::PushL(entry);

	TMsvId outboxId = KMsvGlobalOutBoxIndexEntryId;
	
	// creating ChildrenSelection
	TMsvSelectionOrdering order;
	order.SetShowInvisibleEntries(ETrue);
	entry->SetSortTypeL(order);
	
	entry->SetEntryL(outboxId);
	// Get the  selection containing the IDs of all the context children
	CMsvEntrySelection* selection = entry->ChildrenL();

	// Fetch the Id of the first entry
	TMsvId entryId = KMsvNullIndexEntryId;
	entryId = (*selection)[0];
	
	delete selection;

	CMessAsyncWaiter* waiter = CMessAsyncWaiter::NewL();
	CleanupStack::PushL(waiter);
	
	iConsole->Printf(KSendMail);
	// Create asynchronously, copies of children of the context 
	// as new entries of targetId (smtpServiceId)
	CMsvOperation*	operation = entry->CopyL(entryId,iSmtpAccount.iSmtpService, waiter->iStatus);

	CleanupStack::PushL(operation);
	waiter->StartAndWait();
	User::LeaveIfError(waiter->Result());

	CleanupStack::PopAndDestroy(3, entry); //operation,waiter,entry
	}
	
/**
Changes the entry on which later actions are performed to the entry with the specified TMsvId.
Connects to the pop server.
Downloads the mail sent using smtp and disconnects from the service.
@leave KErrNoMemory,KErrNotFound,KErrNotSupported,system wide error codes
*/
void CPop3Example::ConnectDownloadAndDisconnectPOPServerL()
	{
	
	// Change the current context
	iMtm->SwitchCurrentEntryL(iPopAccount.iPopService);

	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);
	
	// Append the popServiceId onto the end of the array
	selection->AppendL(iPopAccount.iPopService);
		
	TBuf8<1> param;
	
	CMessAsyncWaiter* waiter = CMessAsyncWaiter::NewL();
	CleanupStack::PushL(waiter);
	
	// Attempts to connect to the Pop3 Service 
	iConsole->Printf(KWait);
	CMsvOperation* connectOperation = iMtm->InvokeAsyncFunctionL(KPOP3MTMConnect,*selection,param, waiter->iStatus);
	CleanupStack::PushL(connectOperation);
	
	// Wait on the status
	waiter->StartAndWait();
	User::LeaveIfError(waiter->Result());
	
	// Populate the message
	iConsole->Printf(KDownload);
	CMsvOperation* operationPop = iMtm->InvokeAsyncFunctionL(KPOP3MTMPopulate,*selection,param, waiter->iStatus);
	CleanupStack::PushL(operationPop);
	
	// Wait on the status
	waiter->StartAndWait();
	User::LeaveIfError(waiter->Result());
	
	// Now disconnect from Pop3 service
	CMsvOperation* operationDis = iMtm->InvokeAsyncFunctionL(KPOP3MTMDisconnect,*selection,param, waiter->iStatus);
	CleanupStack::PushL(operationDis);
	
	// Wait on the status
	waiter->StartAndWait();
	User::LeaveIfError(waiter->Result());
	iConsole->Printf(KDisconnect);
	iConsole->Getch ();
	
	CleanupStack::PopAndDestroy(5,selection); //operationDis,operationPop,connectOperation,waiter,selection
	}
	
/**
Implements the session observer interface to update the registry information when MTM groups are installed or uninstalled.
Invoked by the active object mechanism.
@param aEvent	Event type 
@param aArg1	Event type-specific argument value (unused)
@param aArg2	Event type-specific argument value (unused)
@param aArg3	Event type-specific argument value (unused)
@leave		System wide error codes
 */
void CPop3Example::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{}

LOCAL_C void MainL()
	{

	// Create an Active Scheduler to handle asychronous calls
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	/*
	If Active Scheduler has been created, install it .
	As it is an asychronous call we need to install it explicitly.
	*/
	CActiveScheduler::Install( scheduler );

	CPop3Example* app = CPop3Example::NewL();
	CleanupStack::PushL(app);
	
	// Create a client registry.
	app->CreateClientRegistryL();
	
	// Create pop and smtp accounts.
	app->CreatePopAndSmtpAccountL();

	// Load the physical and logical device drivers.
	// Symbian OS will automatically append .PDD and .LDD
	
	TInt err;
	err=User::LoadPhysicalDevice(PDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
	err=User::LoadLogicalDevice(LDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
		
	// Create an Smtp mail to be sent.	
	app->CreateSmtpMessage();
	 
	// Send the mail created.
	app->SendSMTPMessage();
	
	// Connect to the pop3 server for downloading the mail and disconnect.
	app->ConnectDownloadAndDisconnectPOPServerL();
	
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
		_LIT(KUserPanic,"Failed to complete");	
		User::Panic(KUserPanic, err);
		}

    delete cleanup;
    __UHEAP_MARKEND;
    return KErrNone;
	}
