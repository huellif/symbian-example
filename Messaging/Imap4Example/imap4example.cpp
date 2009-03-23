// imap4example.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//
/**
@file

This is a simple code example that demonstrates the use of 
the IMAP4 protocol to retrieve emails. 
The code creates a simple SMTP mail, sends it 
and then retrieves the same using IMAP4.
*/

#include "imap4example.h"
#include "asyncwaiter.h"
#include <e32cons.h>
#include "imapset.h"
#include "impcmtm.h"
#include <mtclbase.h>
#include <IAPPrefs.h>

_LIT(KTitle, "Imap4example");
_LIT(KTextPressAKey, "\n\n Press any key to step through the example");
_LIT(KExit,"\n\n Press any key to exit the application ");
_LIT(KCreateMtm,"\n\n Creating an IMAP4 client MTM");
_LIT(KTxtAccountName, "ImapAccount");
_LIT(KCreateAccounts,"\n\n Creating IMAP and SMTP acoounts");
_LIT(KSmtpServerAddress, "ban-sindhub01.intra");
_LIT(KEmailAlias, "Messaging example");
_LIT(KSmtpEmailAddress, "ban-sindhub01@ban-sindhub01.intra");
_LIT8(KFrom, "ban-sindhub01@ban-sindhub01.intra");
_LIT(KTo, "ban-sindhub01@ban-sindhub01.intra");
_LIT(KSubject, "SimpleEmail");
_LIT(KBodyContents, "This is a very simple mail");
_LIT(KSendMail,"\n\n Sending the mail ... please wait");
_LIT(KImapServer, "ban-sindhub01.intra");
_LIT8(KImapPassword,"ban-sindhub01");
_LIT8(KImapLoginName,"ban-sindhub01");
_LIT(KWait,"\n\n Connecting to the IMAP server");
_LIT(KPopulate,"\n\n Downloading........");
_LIT(KDisconnect,"\n\n Disconnecting from the IMAP server");

/**
Allocates and constructs a CImap4Example object. 
Initialises all member data to their default values.
*/
CImap4Example* CImap4Example::NewL()
	{
	CImap4Example* self= new (ELeave) CImap4Example();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
Constructor
*/
CImap4Example::CImap4Example()
	{}

/**
Second phase constructor
*/
void CImap4Example::ConstructL()
	{
	iConsole = Console::NewL(KTitle,TSize(KConsFullScreen,KConsFullScreen));

	iConsole->Printf ( KTextPressAKey );
	iConsole->Getch ();
	}

/**
Destructor
*/
CImap4Example::~CImap4Example()
	{
	iConsole->Printf(KExit);
	iConsole->Getch();
	delete iMtm;
	delete iClientRegistry;
	delete iSession;
	delete iConsole;
	}

/**
Starts a message server session, creates a client side MTM registry 
and IMAP client MTM.
@leave KErrNoMemory
@leave KErrNotFound
@leave KErrBadLibraryEntryPoint
*/
void CImap4Example::CreateClientRegistryL()
	{
	// Create a message server session
	iSession=CMsvSession::OpenSyncL(*this);
	CleanupStack::PushL(iSession);

	// Create a client-side MTM registry
	iClientRegistry=CClientMtmRegistry::NewL(*iSession,KMsvDefaultTimeoutMicroSeconds32);
	if (iClientRegistry != NULL && iClientRegistry->IsPresent(KUidMsgTypeIMAP4))
		{
		CleanupStack::PushL(iClientRegistry);
		}
	// Create a client-side MTM object for the specified MTM UID.
	iMtm=(CImap4ClientMtm*)iClientRegistry->NewMtmL(KUidMsgTypeIMAP4); 
	iConsole->Printf(KCreateMtm);
	CleanupStack::Pop(2,iSession); // iClientRegistry,iSession
	}

/**
Creates an IMAP account and SMTP account. 
It sets the IMAP server address to system address and sets login name and password.
@leave ... System wide error codes
*/
void CImap4Example::CreateImapAndSmtpAccountL()
	{
	CEmailAccounts* emailAccounts = CEmailAccounts::NewLC();
	CImImap4Settings *imap4Settings = new(ELeave)CImImap4Settings;
	CleanupStack::PushL(imap4Settings);

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

	emailAccounts->PopulateDefaultImapSettingsL(*imap4Settings, *prefs);

	// Set the server address to system address
	imap4Settings->SetServerAddressL(KImapServer);
	imap4Settings->SetLoginNameL(KImapLoginName); 
	imap4Settings->SetPasswordL(KImapPassword);
	imap4Settings->SetPort(143);

	// Create an IMAP account
	iImapAccount = emailAccounts->CreateImapAccountL(KTxtAccountName, *imap4Settings,*prefs,EFalse);

	CImSmtpSettings *smtpSettings = new (ELeave) CImSmtpSettings();
	CleanupStack::PushL(smtpSettings);

	emailAccounts->PopulateDefaultSmtpSettingsL(*smtpSettings, *prefs);

	// Create an SMTP acoount
	iConsole->Printf(KCreateAccounts);
	iSmtpAccount= emailAccounts->CreateSmtpAccountL(iImapAccount, *smtpSettings, *prefs, EFalse);

	emailAccounts->SetDefaultSmtpAccountL(iSmtpAccount);

	smtpSettings->SetServerAddressL(KSmtpServerAddress);
	smtpSettings->SetEmailAliasL(KEmailAlias);
	smtpSettings->SetEmailAddressL(KSmtpEmailAddress);
	smtpSettings->SetReplyToAddressL(KSmtpEmailAddress);
	smtpSettings->SetReceiptAddressL(KSmtpEmailAddress);
	smtpSettings->SetPort(25);

	emailAccounts->GetSmtpAccountL(iSmtpAccount.iSmtpService, iSmtpAccount);
	emailAccounts->SaveSmtpSettingsL(iSmtpAccount,*smtpSettings);
	emailAccounts->SaveSmtpIapSettingsL(iSmtpAccount, *prefs);

	CleanupStack::PopAndDestroy(6,emailAccounts); //smtpSettings, connPrefRecord, dbSession, prefs, imap4Settings, emailAccounts
	}

/**
Creates an SMTP message, stores and then 
commits settings to a specified message store.
@leave KErrNoMemory
@leave KErrNotFound
@leave KErrAccessDenied
*/
void CImap4Example::CreateSmtpMessageL() 
	{
	TMsvId outboxId = KMsvGlobalOutBoxIndexEntryId; 

	// Set the context to the folder in which message has to be created
	CMsvEntry* entry = CMsvEntry::NewL(*iSession,outboxId,TMsvSelectionOrdering());
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

	// Insert the contents of a buffer into the document at specified position
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
Sends the created SMTP mail.
@leave KErrNoMemory
@leave KErrNotFound
*/
void CImap4Example::SendSMTPMessageL()
	{
	CMsvEntry* entry = iSession->GetEntryL(iSmtpAccount.iSmtpService);
	CleanupStack::PushL(entry);

	TMsvId outboxId = KMsvGlobalOutBoxIndexEntryId;

	// Create children selection
	TMsvSelectionOrdering order;
	order.SetShowInvisibleEntries(ETrue);
	entry->SetSortTypeL(order);

	entry->SetEntryL(outboxId);
	// Get the  selection containing the IDs of all the context children
	CMsvEntrySelection* selection = entry->ChildrenL();

	// Fetch the ID of the first entry
	TMsvId entryId = KMsvNullIndexEntryId;
	entryId = (*selection)[0];

	delete selection;

	CMessAsyncWaiter* waiter = CMessAsyncWaiter::NewL();
	CleanupStack::PushL(waiter);

	iConsole->Printf(KSendMail);
	// Create asynchronously copies of children of the context
	// as new entries of targetId (smtpServiceId)
	CMsvOperation* operation = entry->CopyL(entryId,iSmtpAccount.iSmtpService, waiter->iStatus);

	CleanupStack::PushL(operation);
	waiter->StartAndWait();
	User::LeaveIfError(waiter->Result());

	CleanupStack::PopAndDestroy(3, entry); // operation,waiter,entry
	}

/**
Changes the entry on which later actions are performed 
to the entry with the specified TMsvId. 
Connects to the IMAP server. 
Downloads the mail sent using SMTP and disconnects from the service.
@leave KErrNoMemory
@leave KErrNotFound
@leave KErrNotSupported
@leave ... System wide error codes
*/
void CImap4Example::ConnectAndCopyAllMailAndDisconnectL()
	{
	// Change the current context
	iMtm->SwitchCurrentEntryL(iImapAccount.iImapService);
	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);

	// Append the imapServiceId onto the end of the array
	selection->AppendL(iImapAccount.iImapService);

	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = iImapAccount.iImapService+1; // remote inbox
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyText;

	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);

	CMessAsyncWaiter* waiter = CMessAsyncWaiter::NewL();
	CleanupStack::PushL(waiter);

	// Connect to the server
	iConsole->Printf (KWait);
	CMsvOperation* operationConnect = iMtm->InvokeAsyncFunctionL(KIMAP4MTMConnect,*selection,package, waiter->iStatus);
	CleanupStack::PushL(operationConnect);

	// Wait on the status
	waiter->StartAndWait();
	User::LeaveIfError(waiter->Result());

	CMsvOperation* operationSync = iMtm->InvokeAsyncFunctionL(KIMAP4MTMFullSync ,*selection,package, waiter->iStatus);
	CleanupStack::PushL(operationSync);

	// Wait on the status
	waiter->StartAndWait();
	User::LeaveIfError(waiter->Result());

	CMsvEntry* entry = iSession->GetEntryL(iImapAccount.iImapService);
	CMsvEntrySelection* childrenSelection = entry->ChildrenL();
	TMsvId id = (*childrenSelection)[0];
	selection->AppendL(id);

	// Copy the meassage when already connected
	iConsole->Printf (KPopulate);
	CMsvOperation* operationCopy = iMtm->InvokeAsyncFunctionL(KIMAP4MTMPopulateAllMailWhenAlreadyConnected ,*selection,package, waiter->iStatus);
	CleanupStack::PushL(operationCopy);

	// Wait on the status
	waiter->StartAndWait();

	User::LeaveIfError(waiter->Result());

	// Now disconnect from Imap4 service
	iConsole->Printf (KDisconnect);
	CMsvOperation* operationDis = iMtm->InvokeAsyncFunctionL(KIMAP4MTMDisconnect,*selection,package, waiter->iStatus);
	CleanupStack::PushL(operationDis);

	// Wait on the status
	waiter->StartAndWait();
	User::LeaveIfError(waiter->Result());
	delete entry;
	delete childrenSelection;

	CleanupStack::PopAndDestroy(6,selection); // operationDis,operationCopy,operationSync,operationConnect,waiter,selection	
	}

/**
Implements the session observer interface to update the registry 
information when MTM groups are installed or uninstalled. 
Invoked by the active object mechanism.
@param aEvent	Event type 
@param aArg1	Event type-specific argument value (unused)
@param aArg2	Event type-specific argument value (unused)
@param aArg3	Event type-specific argument value (unused)
@leave ...  	System wide error codes
*/
void CImap4Example::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{}

LOCAL_C void MainL()
	{
	// Create an Active Scheduler to handle asychronous calls
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;

	// If Active Scheduler has been created, install it.
	// As it is an asychronous call we need to install it explicitly.
	CActiveScheduler::Install( scheduler );

	CImap4Example* app = CImap4Example::NewL();
	CleanupStack::PushL(app);

	// Create a client registry
	app->CreateClientRegistryL();

	// Create IMAP and SMTP accounts
	app->CreateImapAndSmtpAccountL();

	// Load the physical and logical device drivers.
	// Symbian OS will automatically append .PDD and .LDD
	TInt err;
	err=User::LoadPhysicalDevice(PDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
	err=User::LoadLogicalDevice(LDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);

	// Create an SMTP mail to be sent
	app->CreateSmtpMessageL();

	// Send the mail created.
	app->SendSMTPMessageL();

	// Connect, download and disconnect from the IMAP server
	app->ConnectAndCopyAllMailAndDisconnectL();

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
		User::Panic(_L("Failed to complete"),err);
		}

	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}
