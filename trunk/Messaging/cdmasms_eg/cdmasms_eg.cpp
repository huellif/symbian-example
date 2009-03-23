//////////////////////////////////////////////////////////////////////////
//
//	cdmasms_eg.cpp
//
//	Copyright (c) 2004 Symbian Software Ltd.  All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#include <mtclreg.h>
#include <txtrich.h>
#include <e32cons.h>
#include <eikenv.h>
#include <emssoundie.h>
#include <emsanimationie.h>

#include "cdmasms_eg.h"



//=====================================================
//
// CSendOp: 
//	active object signalled when asynchronous messaging operation completes
//
//=====================================================

/**
Constructor.
@param aMsvOperation Operation to monitor
*/
CSendOp::CSendOp(CSMSSession& aSession)
	: CActive(EPriorityStandard), iSmsSession(aSession)
	{
	CActiveScheduler::Add(this);
	}

/**
Start a send operation.
*/
void CSendOp::SendL()
	{	
	iMsvOperation = iSmsSession.SendSMSNowL(iStatus);
	SetActive(); 
	// if the operation completed synchronously, 
	// we need to go ahead and complete ourselves
	if (! iMsvOperation->IsActive())
		{
		TRequestStatus* status = &iStatus ;
		User::RequestComplete(status, KErrNone);
		}	
	}

/**
Destructor.
*/
CSendOp::~CSendOp()
	{
	Cancel();
	delete iMsvOperation;
	}

/** If cancelled, cancel outstanding operation */
void CSendOp::DoCancel()
	{
	iMsvOperation->Cancel();
	TRequestStatus* status = &iStatus ;
	User::RequestComplete(status, KErrCancel);
	}


/** On completion, display progress */
void CSendOp::RunL() 
	{
	// get progress information after send operation completes
	TSmsProgressBuf progressBuf;
	progressBuf.Copy(iMsvOperation -> FinalProgress());
	TSmsProgress progress = progressBuf();

	TBuf<40> msg1;
	TBuf<20> msg2;
	msg1.Format(_L("\nSent %d of %d messages"),progress.iMsgDone, progress.iMsgCount);
	if (progress.iError) msg2.Format(_L("Error: %d"), progress.iError);
	CEikonEnv::Static()->AlertWin(msg1, msg2);
	}

//=====================================================
//
//	CSMSSession
//
//=====================================================


/** Constructor.

@param aSessionObserver Session observer
*/
CSMSSession::CSMSSession(MMsvSessionObserver& aSessionObserver)
	:iSessionObserver(aSessionObserver)
	{}

/** Second phase constructor.

@leave KErrNotFound No SMS service exists
*/
void CSMSSession::ConstructL()
	{
	// Create message server session
	iMsvSession = CMsvSession::OpenSyncL(iSessionObserver);
	// Create client registry object
	iClientMtmRegistry = CClientMtmRegistry::NewL(*iMsvSession);
	// Get SMS client MTM object
	iSmsClientMtm = static_cast<CSmsClientMtm*>(iClientMtmRegistry->NewMtmL(KUidMsgTypeSMS));	

	// check a service exists
	TInt sid = iSmsClientMtm->ServiceId();
	if (!sid)
		{
		// need to create a new entry for the service in the root of the message store
		CMsvEntry* entry = iMsvSession->GetEntryL(KMsvRootIndexEntryId);			
		CleanupStack::PushL(entry);
		// set message idex entry
		TMsvEntry s;
		s.iMtm = KUidMsgTypeSMS;
		s.iType = KUidMsvServiceEntry;
		s.SetVisible(EFalse);
		// create new message entry	
		entry->CreateL(s);
		entry->SetEntryL(s.Id());
		// open store for entry
		CMsvStore* store = entry->EditStoreL();
		CleanupStack::PushL(store);
				
		// load SMS settings into MTM
		iSmsClientMtm -> RestoreServiceAndSettingsL();
		
		/*	could access SMS settings like this...
		CSmsSettings* smsSet = CSmsSettings::NewLC();
		CSmsAccount* account = CSmsAccount::NewLC();
		account->LoadSettingsL(*smsSet);
		// .. make settings as needed here
		// save settings and store
		account->SaveSettingsL(*smsSet);
		CleanupStack::PopAndDestroy(4);
		*/		
		}	
	}

/**
Destructor.
*/
CSMSSession::~CSMSSession()
	{
	delete iSmsClientMtm;
	delete iClientMtmRegistry;
	delete iMsvSession;
	}
	
/** Sends SMS messages that are waiting in the outbox.

@param aStatus Status to complete when the send operation completes.
@return Asynchronous message sending operation 
*/
CMsvOperation* CSMSSession::SendSMSNowL(TRequestStatus& aStatus)
	{
	// get a list of SMS messages in the outbox
	CMsvEntry* entry = iMsvSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId);	
	CleanupStack::PushL(entry);
	CMsvEntrySelection* smsEntries = entry->ChildrenWithMtmL(KUidMsgTypeSMS);
	CleanupStack::PushL(smsEntries);
	// copy to service to send messages
	CMsvOperation* op = NULL;
	if (smsEntries->Count())
		op = entry->CopyL(*smsEntries, iSmsClientMtm->ServiceId(), aStatus);
	else
		op = CMsvCompletedOperation::NewL(*iMsvSession,KUidMsgTypeSMS, KNullDesC8,iSmsClientMtm->ServiceId(),aStatus);		
	CleanupStack::PopAndDestroy(2); //smsEntries, entry
	return op;
	}	

// these are only needed for SimulateIncomingL()
#include <cdmasmsmessage.h>

/** 
Creates a phony incoming SMS message in the inbox to help test
handling of incoming messages.
@param aWEMT True to simulate WEMT message, false for WMT
*/
void CSMSSession::SimulateIncomingL(TBool aWEMT)
	{
	//Set the new message entry
	TMsvSmsEntry newEntry;
	newEntry.iType = KUidMsvMessageEntry;
	newEntry.iDescription.Set(_L(""));
	newEntry.iDetails.Set(_L(""));
	newEntry.SetMessageId(100, ETrue);		
	newEntry.SetAckSummary(ESmsAckTypeDelivery, TMsvSmsEntry::ENoAckSummary);
	newEntry.SetAckSummary(ESmsAckTypeUser,TMsvSmsEntry::ENoAckSummary);
	newEntry.SetAckSummary(ESmsAckTypeRead, TMsvSmsEntry::ENoAckSummary);      
	newEntry.iMtm = KUidMsgTypeSMS;    
	newEntry.iServiceId	= iSmsClientMtm->ServiceId();
	newEntry.iError	= KErrNone;	
	TTime currentTime;
	currentTime.HomeTime();
	newEntry.iDate = currentTime;
				
	//Create a new entry to store the message
	CMsvEntry* entry = iMsvSession->GetEntryL(KMsvGlobalInBoxIndexEntryId);
	CleanupStack::PushL(entry);
	entry->CreateL(newEntry);
	iMsvSession->CleanupEntryPushL(newEntry.Id());
	entry->SetEntryL(newEntry.Id());
	
	// Save the message
	CMsvStore* store = entry->EditStoreL();
	CleanupStack::PushL(store);
	
	CCdmaSmsMessage* sms = CCdmaSmsMessage::NewL(CEikonEnv::Static()->FsSession(),NULL, 
		(aWEMT ? tia637::KTeleserviceWEMT : tia637::KTeleserviceWMT), tia637::KTDeliver);
	
	// set originating address - required to prevent a panic
	CCdmaSmsEndPoint* endpoint = CCdmaSmsEndPoint::NewL(_L("1234"), ETrue, tia637::KNTIsdnUnknown, tia637::KNPUnknown, EFalse);
	CleanupStack::PushL(endpoint);	
	CTpAddress* tpAddress = CTpAddress::NewL(*endpoint, tia637::KOriginatingAddress);
	CleanupStack::PopAndDestroy(endpoint);
	CleanupStack::PushL( tpAddress );																	
	sms->SetTransportParameterL(*tpAddress);
	CleanupStack::PopAndDestroy(tpAddress);
	
	TBdPriorityIndicator prparam;
	prparam.SetPriority(tia637::KBdUrgent);
	sms->SetBearerDataSubParameterL(prparam);
	
	TBdPrivacyIndicator privacy;
	privacy.SetPrivacy(tia637::KBdRestricted);
	sms->SetBearerDataSubParameterL(privacy);

	TBdReplyOption roparam;
	roparam.SetReadAckRequired(ETrue);
	roparam.SetUserAckRequired(ETrue);
	roparam.SetDeliveryAckRequired(ETrue);
	sms->SetBearerDataSubParameterL(roparam);
	
	TBdMessageDepositIndex mdparam;
	mdparam.SetMessageDepositIndex(100);
	sms->SetBearerDataSubParameterL(mdparam);

	TBdAlertOnMessageDelivery adparam;
	adparam.SetAlert(tia637::KBdUseMediumPriorityAlert);
	sms->SetBearerDataSubParameterL(adparam);

	TBdLanguageIndicator lparam;
	lparam.SetLanguage(tia637::KLanguageEnglish);
	sms->SetBearerDataSubParameterL(lparam);
	
	CSmsHeader* smsHeader = CSmsHeader::NewL(sms);
	CleanupStack::PushL(smsHeader);
	
	if (aWEMT)
		{
		CSmsMessage& msg = smsHeader->Message();
		// if simulating an EMS message, add a couple of ems elements
		CEmsPreDefAnimationIE* ani = CEmsPreDefAnimationIE::NewL(CEmsPreDefAnimationIE::EWinking);
		CleanupStack::PushL(ani);			
		msg.AddEMSInformationElementL(*ani);
		CleanupStack::PopAndDestroy(ani);
		CEmsPreDefSoundIE* soundIE = CEmsPreDefSoundIE::NewL(CEmsPreDefSoundIE::EDrum);
		CleanupStack::PushL(soundIE);			
		msg.AddEMSInformationElementL(*soundIE);
		CleanupStack::PopAndDestroy(soundIE);	
		}
	smsHeader->StoreL(*store);
	CleanupStack::PopAndDestroy(smsHeader);
	
	//Create and fill a CRichText object
	CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);
	CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charFormatLayer);
	CRichText* richText = CRichText::NewL(paraFormatLayer,charFormatLayer);
	CleanupStack::PushL(richText);	
	richText->InsertL(0, _L("simulated incoming message"));
	store->StoreBodyTextL(*richText);	
	CleanupStack::PopAndDestroy(3); // richText, charFormatLayer, paraFormatLayer
	store->CommitL();
	CleanupStack::PopAndDestroy(store);
	
	newEntry.SetVisible(ETrue);
	newEntry.SetInPreparation(EFalse);
	entry->ChangeL(newEntry);

	iMsvSession->CleanupEntryPop(); //newEntry
	CleanupStack::PopAndDestroy(entry);			
	}

/** Create a new empty WEMT message.
@param aServiceSettings Service settings to apply to message 
@return ID of message */
TMsvId CSMSSession::CreateWEMTL(const CSmsSettings& aServiceSettings)
	{
	// set up index entry
 	TMsvEntry newEntry;
	newEntry.iType = KUidMsvMessageEntry;
	newEntry.iServiceId = KMsvLocalServiceIndexEntryId;
	newEntry.iMtm = KUidMsgTypeSMS;
	newEntry.SetVisible(EFalse);
	newEntry.SetInPreparation(ETrue);			

	// create empty message in outbox
	CMsvEntry* entry = iMsvSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId);
	CleanupStack::PushL(entry);
	entry->CreateL(newEntry);
	iMsvSession->CleanupEntryPushL(newEntry.Id());
	// switch to created entry
	entry->SetEntryL(newEntry.Id());

	// create a rich text body
	CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);
	CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charFormatLayer);
	CRichText* richText = CRichText::NewL(paraFormatLayer,charFormatLayer);
	CleanupStack::PushL(richText);	
	// ... can set text in richText now

	// create the header object
	CSmsHeader* smsHeader = CSmsHeader::NewL(KSmsTypeSubmit, *richText, tia637::KTeleserviceWEMT);
	CleanupStack::PushL(smsHeader);
	smsHeader->SetDefaultMessageSettingsL(aServiceSettings);
	// .. configure further as required
	
	// set the message header in the entry's store
	CMsvStore* store = entry->EditStoreL();
	CleanupStack::PushL(store);
	smsHeader->StoreL(*store);
	
	// The rich text is empty, but we will need it to
	// properly load the message
	store->StoreBodyTextL(*richText);	
			
	store->CommitL();
	
	CleanupStack::PopAndDestroy(5); // smsHeader, richText, charFormatLayer, paraFormatLayer, store
	iMsvSession->CleanupEntryPop(); //entry
	CleanupStack::PopAndDestroy(); // entry
	return newEntry.Id();
	}
	
//=====================================================
//
//	CMenuHandler
//
//=====================================================

/**
	Constructor.
*/
CMenuHandler::CMenuHandler(CConsoleBase& aConsole, TDes& aInput)
	: iConsole(aConsole), iInput(aInput)
	{
	}

/** Second phase constructor.
	Creates a session and an input editor. */
void CMenuHandler::ConstructL()
	{
	iSmsSession = new (ELeave) CSMSSession(*this);
	iSmsSession -> ConstructL();
	iMtm = & iSmsSession -> SmsClientMtm();
	}

/**
Destructor.
*/
CMenuHandler::~CMenuHandler()
	{
	if (iSendOperation) iSendOperation->Cancel();
	delete iSendOperation;
	delete iSmsSession;
	}
	
/** Writes the main menu */
void CMenuHandler::MenuL()
	{
	_LIT(KMenu,"\n1. Create WMT\n2. Create WEMT\n3. Send SMS messages\n4. Get incoming messages\n5. Quit\n");
	iConsole.Write(KMenu);
	iState=EMenuChoiceMain;
	}
	
/** Processes user input */
void CMenuHandler::ProcessInputL()
	{
	TInt inputNum = 0;
	switch (iState)
		{
	// Handle main menu choice
	case EMenuChoiceMain:
		if (InputOutOfRange(5,inputNum)) MenuL();
		switch (inputNum)
			{
		// create WMT/WEMT
		case 1:
		case 2:		
			iWEMT = (inputNum==2);
			iState = EInputCreate;
			iCreateState = EInit;
			NextCreateInputL();
			break;
			
		// Send SMS messages
		case 3:
			delete iSendOperation;
			iSendOperation = 0;
			iSendOperation = new (ELeave) CSendOp(*iSmsSession);
			iSendOperation -> SendL();
			MenuL();
			break;
			
		// Get incoming messages
		case 4:
			iState = EInputIncoming;
			iIncomingState = EIncomingInit;
			NextIncomingInputL();
			break;
			
		// quit
		case 5:
			CBaActiveScheduler::Exit();
			break;				
			};
		break;
		
	// Handle incoming message menu choices
	case EMenuChoiceIncoming:
		if (InputOutOfRange(4,inputNum)) IncomingMenuL();
		switch (inputNum)
			{
		// Simulate incoming message
		case 1:
		case 2:
			iSmsSession -> SimulateIncomingL((inputNum==2));
			IncomingMenuL();
			break;
		// Cancel waiting for incoming messages
		case 3:
			iGetIncoming = EFalse;
			MenuL();
			break;
		// Quit
		case 4:
			CBaActiveScheduler::Exit();
			break;
		default:
			break;				
			}
		break;

	// creating messages	
	case EInputCreate:
		NextCreateInputL();
		break;

	// setting options for incoming messages
	case EInputIncoming:
		NextIncomingInputL();
		break;

	default:
		break;			
		}
	}

	
/** Writes the main menu */
void CMenuHandler::IncomingMenuL()
	{
	_LIT(KMenu,"\n1. Simulate incoming WMT message\n2. Simulate incoming WEMT message\n3. Cancel waiting for incoming messages\n4. Quit\n");
	iConsole.Write(KMenu);
	iState=EMenuChoiceIncoming;
	}

/**
Gets options for accepting incoming messages.
*/
void CMenuHandler::NextIncomingInputL()
	{
	switch (iIncomingState)
		{
	// Get teleservice to match
	case EIncomingInit:
		iIncomingState++;
		iConsole.Write(_L("\nTeleservice to get: 1. WMT; 2: WEMT\n"));
		break;
	// Set teleservice to match
	case ESetTeleservice:
		{
		TInt teleservice = 0;	
		if (!InputOutOfRange(2,teleservice))
			if (teleservice == 1) 			
				iMatchTeleservice = tia637::KTeleserviceWMT;
			else
				iMatchTeleservice = tia637::KTeleserviceWEMT;
		}
		// Get text to match
		iConsole.Write(_L("\nText to match:\n"));
		iIncomingState++;
		break;
	// Set text match and start listening
	case ESetTextMatch:
		{
		iMatchText = iInput.Left(5);	
		iGetIncoming = ETrue;
		IncomingMenuL();		
		}
		break;
	default:
		break;		
		}	
	}

/**
Gets options for creating an outgoing message.
*/
void CMenuHandler::NextCreateInputL()
	{
	switch (iCreateState)
		{
	// Create new empty message
	case EInit:
		if (!iWEMT)
			{
			// Set context to Outbox folder
			iMtm->SwitchCurrentEntryL(KMsvGlobalOutBoxIndexEntryId);			
			// Create new message entry
			iMtm->CreateMessageL(0);
			}
		else
			{
		 	// because we're going to use client mtm functions
		 	// later on, set the mtm context to this entry
		 	TMsvId newID = iSmsSession -> CreateWEMTL(iMtm->ServiceSettings());
		 	iMtm->SwitchCurrentEntryL(newID);
		 	iMtm->LoadMessageL();
			}

		iCreateState++;					
		iConsole.Write(_L("\nMessage text:\n"));
		break;
		
	// Message text:
	case ESetMsgText:
		if (iInput.Length())
			{
			// set body text
			CRichText& body = iMtm->Body();
			body.InsertL(0,iInput);
			}
		iCreateState++;					
		iConsole.Write(_L("\nRecipient address:\n"));
		break;

	// Message address:
	case ESetAddress:
		{
		if (iInput.Length())
			{
			// set address - could use 
			// iMtm->AddAddresseeL(iInput);
			// or use this to specify ISDN or email address	
			CSmsHeader& header = iMtm->SmsHeader();
			CArrayPtrFlat<CSmsNumber>& recips = header.Recipients();
			CSmsNumber* newRecip = CSmsNumber::NewL();
			CleanupStack::PushL(newRecip);
			newRecip->SetAddressL(iInput, EMsvAddressTypeISDN);
			recips.AppendL(newRecip);
			CleanupStack::Pop(newRecip); //CSmsHeader will delete it
			}
		iCreateState++;					
		iConsole.Write(_L("\nValidity Period (YYYYMMDD:HHMMSS):\n"));
		}
		break;

	// Validity Period - Absolute
	case ESetValidityPeriodAbs:
		{
		TTime time;
		TInt err = time.Set(iInput);
		if (!err)
			{
			// This method is defined in the TMsvMessageSms wrapper
			iMtm->SmsHeader().CdmaMessage().SetValidityPeriodAbsoluteL(time.DateTime());
			}
		/*	for relative period, use SetValidityPeriodRelativeL(), e.g.
		TSmsRelativeTimeUnit timeUnit=ETimeUnitDays;
		TUint32 relativeTime=1;
		iMtm->SmsHeader().CdmaMessage().SetValidityPeriodRelativeL(timeUnit, relativeTime);		
		*/
		iCreateState++;					
		iConsole.Write(_L("\nDeferred delivery time (YYYYMMDD:HHMMSS):\n"));
		}
		break;
				
	// Deferred Delivery Time  - Absolute
	case ESetDeliveryTimeAbs:
		{
		TTime time;
		TInt err = time.Set(iInput);
		if (!err)
			{
			// This method is defined in the TMsvMessageCdma wrapper
			iMtm->SmsHeader().CdmaMessage().SetDeferredDeliveryTimeAbsoluteL(time.DateTime());
			}
	//	for relative period, use SetDeferredDeliveryTimeRelativeL();
		iCreateState++;					
		iConsole.Write(_L("\nPriority (1. Normal, 2. Interactive, 3. Urgent, 4. Emergency):\n"));
		}
		break;

	// Priority Indicator 
	case ESetPriority:
		{
		TInt priority = 0;
		if (!InputOutOfRange(4,priority))
			// This method is defined in the TMsvMessageCdma wrapper
			iMtm->SmsHeader().CdmaMessage().SetPriorityIndicatorL(priority-1); // tia637::TPriorityIndicator values are 0-3
		iCreateState++;							
		iConsole.Write(_L("\nPrivacy (1. Not restricted, 2. Restricted, 3. Confidential, 4. Secret):\n"));
		}
		break;

	// Privacy Indicator 
	case ESetPrivacy:
		{
		TInt privacy = 0;
		if (!InputOutOfRange(4,privacy))
			// This method is defined in the TMsvMessageCdma wrapper
			iMtm->SmsHeader().CdmaMessage().SetPrivacyIndicatorL(privacy-1); // tia637::TPrivacy values are 0-3
		iCreateState++;							
		iConsole.Write(_L("\nAlert on delivery (1. Default; 2. Low; 3. Medium; 4. High):\n"));
		}
		break;

	// Alert on  delivery
	case ESetAlert:
		{
		TInt alert = 0;
		if (!InputOutOfRange(4,alert))
			// This method is defined in the TMsvMessageCdma wrapper
			iMtm->SmsHeader().CdmaMessage().SetAlertOnDeliveryL(alert-1); // tia637::TAlertPriority values are 0-3
		iCreateState++;							
		iConsole.Write(_L("\nLanguage indicator (1. Unknown; 2. English; 3. French; 4. Spanish; \
5. Japanese; 6. Korean; 7. Chinese; 8. Hebrew):\n"));		
		}
		break;

	// Language indicator
	case ESetLang:
		{
		TInt lang = 0;
		if (!InputOutOfRange(8,lang))
			// This method is defined in the TMsvMessageCdma wrapper
			iMtm->SmsHeader().CdmaMessage().SetLanguageIndicatorL(lang-1); // tia637::TLanguageIndicator values are 0-7
		iCreateState++;							
		iConsole.Write(_L("\nCallback number:\n"));
		}
		break;

	// Call Back Number
	case ESetSetCallbackNumber:
		{
		// This method is defined in the TMsvMessageCdma wrapper
		iMtm->SmsHeader().CdmaMessage().SetCallbackNumberL(iInput);

		if (!iWEMT)
			SaveMessageL(); // end of the input menu for WMT
		// the remaining items are for WEMT only
		else
			{		
			iConsole.Write(_L("\nMessage animation (1. flirty; 2. winking; 3. glasses):\n"));
			iCreateState++;
			}					
		}
		break;
	// WEMT message animation
	case EWEMTSetAnimation:
		{
		TInt animation = 0;
		if (!InputOutOfRange(3,animation))
			{
			CEmsPreDefAnimationIE::TAnimType animValue;
			if (animation == 1) animValue=CEmsPreDefAnimationIE::EFlirty;
				else
			if (animation == 2) animValue=CEmsPreDefAnimationIE::EWinking;
				else
			animValue=CEmsPreDefAnimationIE::EGlasses;							

			CEmsPreDefAnimationIE* ani = CEmsPreDefAnimationIE::NewL(animValue);
			CleanupStack::PushL(ani);			
			// This method is defined in the TMsvMessageSms wrapper
			iMtm->SmsHeader().CdmaMessage().AddEMSInformationElementL(*ani);
			CleanupStack::PopAndDestroy(ani);
			}
		iConsole.Write(_L("\nMessage sound (1. chimes; 2. ding; 3. drums):\n"));
		iCreateState++;
		}
		break;
	// WEMT message sound
	case EWEMTSetSound:
		{
		TInt sound = 0;
		if (!InputOutOfRange(3,sound))
			{
			CEmsPreDefSoundIE::TPredefinedSound soundValue;
			if (sound == 1) soundValue=CEmsPreDefSoundIE::EChimesHigh;
				else
			if (sound == 2) soundValue=CEmsPreDefSoundIE::EDing;
				else
			soundValue=CEmsPreDefSoundIE::EDrum;							

			CEmsPreDefSoundIE* soundIE = CEmsPreDefSoundIE::NewL(soundValue);
			CleanupStack::PushL(soundIE);			
			// This method is defined in the TMsvMessageSms wrapper
			iMtm->SmsHeader().CdmaMessage().AddEMSInformationElementL(*soundIE);
			CleanupStack::PopAndDestroy(soundIE);
			}
		SaveMessageL();
		}
		break;
	default:
		break;
		}
	}						
		
/** Set message for later scheduling sending.
The example doesn't call this: it's just to show the method.

@param aId ID of message to schedule
@param aStatus Asynchronous status to signal when scheduling operation completes
@return Messaging scheduling operation
*/
CMsvOperation* CMenuHandler::SaveScheduledL(TMsvId aId,TRequestStatus& aStatus)
	{
	// schedule message
	CMsvEntrySelection* smsEntries = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(smsEntries);
	smsEntries->AppendL(aId);
	TBuf8<1> p;
	CMsvOperation* op = iMtm->InvokeAsyncFunctionL(ESmsMtmCommandScheduleCopy,
		*smsEntries, p, aStatus);
	CleanupStack::PopAndDestroy(); //smsEntries
	return op;
	}
	
/** Commits message to message store. 
*/
void CMenuHandler::SaveMessageL()
	{
	TMsvEntry ent = iMtm->Entry().Entry();
	ent.SetVisible(ETrue);
	ent.SetInPreparation(EFalse);
    ent.SetSendingState(KMsvSendStateWaiting);   
    ent.iDate.HomeTime();
    iMtm->Entry().ChangeL(ent);                       		
	iMtm->SaveMessageL();
	iConsole.Write(_L("\nMessage created.\n"));
	MenuL();
	}
	
/** Tests input is a number less than or equal a certain value.

@param aHigh Upper range of input
@param aVal On return, the input number
@return True if input is a number less than or equal aHigh
*/
TBool CMenuHandler::InputOutOfRange(TInt aHigh,TInt& aVal) const
	{
	TLex lex(iInput);
	TInt err = lex.Val(aVal);
	return (err != KErrNone || aVal > aHigh);
	}
	
/**
Called by CMsvSession when a messaging event has occurred. It is used here to 
find out if any new messages have been created.
@param aEvent Event type
@param aArg1 Event arguments
*/
void CMenuHandler::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* , TAny* )
	{
	CMsvEntrySelection* entries = NULL;

	// check the event type
	switch (aEvent)
		{
	// new messages have been created in the message store
	case EMsvEntriesCreated:
		entries = static_cast<CMsvEntrySelection*>(aArg1);
		break;
	// these cases indicate a problem that can't be recovered from
    case EMsvCloseSession:
    case EMsvServerTerminated:
    case EMsvGeneralError:
    case EMsvServerFailedToStart:
        CActiveScheduler::Stop();
        break;
    default:
        break;        			
		}

	// handle any incoming messages if the user has set the get incoming flag
	if (iGetIncoming && entries)
		{
		TInt count = entries->Count();

		// check each entry to see if we want to process it
		while (count--)
			{
			// get the index entry
			TMsvId serviceId;
			TMsvEntry entry;
			iMtm->Session().GetEntry((*entries)[count], serviceId, entry);
			// if the entry is an SMS message
			if (entry.iMtm == KUidMsgTypeSMS && entry.iType == KUidMsvMessageEntry
				// and if it's an incoming message (as its in the inbox)
				&& entry.Parent() == KMsvGlobalInBoxIndexEntryId)
				{
				// switch the MTM context to the message, so that further checking can be done
				iMtm->SwitchCurrentEntryL((*entries)[count]);
				iMtm->LoadMessageL();
				TBool match = ETrue;
				// Check the message body against any specified match text		
				if (iMatchText.Length())
					{
					CRichText& body = iMtm->Body();
					TPtrC text(body.Read(0, iMatchText.Length()));
					match = (text.Compare(iMatchText) == 0); 
					}
				// Check the message teleservice against the match teleservice
				if (iMatchTeleservice)
					{
					// This method is defined in the TMsvMessageCdma wrapper
					match &= (iMtm->SmsHeader().CdmaMessage().TeleserviceId() == iMatchTeleservice);
					}
				// if the message is for this program
				if (match) 
					{
					// make the entry invisible
					entry.SetVisible(EFalse);
					iMtm->Entry().ChangeL(entry);
					// print its contents
					PrintMessageL();
					}
				}
			}
		}
	}
	
/**
Print details of an incoming message to the console.
*/
void CMenuHandler::PrintMessageL()
	{
	// message text
    CRichText& body = iMtm->Body();
    iConsole.Printf(_L("Message received:"));
    TPtrC text(body.Read(0));
    iConsole.Printf(_L("\n\tText: %S"),&text);
    
    // sender: takes name from Contacts database if an entry is available
    const TMsvEntry& entry = iMtm->Entry().Entry();
    TPtrC originator(entry.iDetails);
    iConsole.Printf(_L("\n\tSender: %S"),&originator);

	// Originating Address
        // This method is defined on the TMsvMessageBio wrapper
	TPtrC originatingAddress = iMtm->SmsHeader().CdmaMessage().OriginatingAddress();
	iConsole.Printf(_L("\n\tOriginating address: %S"), &originatingAddress);

	// Originating SubAddress
        // This method is defined on the TMsvMessageCdma wrapper
	HBufC* originatingSubAddress = iMtm->SmsHeader().CdmaMessage().GetOriginatingSubAddressL();
	if (originatingSubAddress)
		{
		iConsole.Printf(_L("\n\tOriginating Subaddress: %S"), originatingSubAddress);
		}
	delete originatingSubAddress;
	
	// Call Back Number
        // This method is defined on the TMsvMessageCdma wrapper
	HBufC* callbackString = iMtm->SmsHeader().CdmaMessage().GetCallbackNumberL();
	if (callbackString)
		{
		iConsole.Printf(_L("\n\tCallback number: %S"), callbackString);
		}
	delete callbackString;

	// Teleservice
	// This method is defined in the TMsvMessageCdma wrapper
	tia637::TTeleserviceId teleserviceId = iMtm->SmsHeader().CdmaMessage().TeleserviceId();
	const TText* teleValues[8]={_S("IS-91 Extended Protocol Enhanced Services"), _S("Wireless Paging Teleservice"), 
		_S("Wireless Messaging Teleservice"), _S("Voice Mail Notification"), _S("Wireless Application Protocol"), 
		_S("Wireless Enhanced Messaging Teleservice"), _S("Service Category Programming Teleservice"), 
		_S("Card Application Toolkit Protocol Teleservice") };
	if (teleserviceId != tia637::KTeleserviceNotUsed)
		{
		TBuf<50> teleString(teleValues[teleserviceId - tia637::KTeleserviceCMT91]); // ids consecutive from KTeleserviceCMT91
		iConsole.Printf(_L("\n\tTeleservice: %S"), &teleString);		
		}

	// Message Service Center Time Stamp
	TDateTime timeStamp;
	TInt quarterHourUTCOffset;
	
        // This method is defined on the TMsvMessageSms wrapper
	User::LeaveIfError(iMtm->SmsHeader().CdmaMessage().GetMessageServiceCenterTimeStamp(timeStamp,quarterHourUTCOffset));
	iConsole.Printf(_L("\n\tMessage Service Center Time Stamp: "));
	PrintAbsoluteTimeL(timeStamp);	
	iConsole.Printf(_L("\n\t\tTime Zone difference to GMT in quarter hours: %d"),quarterHourUTCOffset);
	
	// Deferred delivery time
	TSmsTimePeriodFormat deliveryTimeFormat = iMtm->SmsHeader().CdmaMessage().DeliveryTimeFormat();
	if (deliveryTimeFormat == ESmsTimePeriodAbsolute)
		{
		// Absolute: A precise date/time (e.g. 2010 May 28 12:00 AM)
		TDateTime deliveryTime;
		// This method is defined in the TMsvMessageCdma wrapper
		iMtm->SmsHeader().CdmaMessage().GetDeferredDeliveryTimeAbsoluteL(deliveryTime);
		iConsole.Printf(_L("\n\tAbsolute delivery time: "));
		PrintAbsoluteTimeL(deliveryTime);
		}
	if (deliveryTimeFormat == ESmsTimePeriodRelative)
		{
		// Relative format
		TSmsRelativeTimeUnit timeUnit;
		TUint32 relativeTime;
		// This method is defined in the TMsvMessageCdma wrapper
		iMtm->SmsHeader().CdmaMessage().GetDeferredDeliveryTimeRelativeL(timeUnit, relativeTime);
		iConsole.Printf(_L("\n\tRelative Delivery Time: "));
		PrintRelativeTime(timeUnit, relativeTime, EFalse);
		}

	// Priority Indicator 
	tia637::TPriorityIndicator priority = 0;
	// This method is defined in the TMsvMessageCdma wrapper
	TInt err = iMtm->SmsHeader().CdmaMessage().GetPriorityIndicatorL(priority);
	if (err == KErrNone)
		{
		const TText* priorityValues[4]={_S("Normal"), _S("Interactive"), _S("Urgent"), _S("Emergency") };
		TBuf<20> priorityString(priorityValues[priority]);
		iConsole.Printf(_L("\n\tPriority: %S"), &priorityString);
		}
		
	// Privacy Indicator 
	tia637::TPrivacy privacy = 0;
	// This method is defined in the TMsvMessageCdma wrapper
	err = iMtm->SmsHeader().CdmaMessage().GetPrivacyIndicatorL(privacy);
	if (err == KErrNone)
		{
		const TText* privacyValues[3]={_S("Not restricted"), _S("Restricted"), _S("Confidential") };
		TBuf<20> privacyString(privacyValues[privacy]);
		iConsole.Printf(_L("\n\tPrivacy: %S"), &privacyString);
		}
					
	// Alert on  delivery
	tia637::TAlertPriority alert;
	// This method is defined in the TMsvMessageCdma wrapper
	err = iMtm->SmsHeader().CdmaMessage().GetAlertOnDeliveryL(alert);
	if (err == KErrNone)
		{
		const TText* alertValues[4]={_S("Default"), _S("Low"), _S("Medium"), _S("High") };
		TBuf<20> alertString(alertValues[alert]);
		iConsole.Printf(_L("\n\tAlert priority: %S"), &alertString);		
		}	

	// Language indicator
	tia637::TLanguageIndicator lang;
	// This method is defined in the TMsvMessageCdma wrapper
	err = iMtm->SmsHeader().CdmaMessage().GetLanguageIndicatorL(lang);
	if (err == KErrNone)
		{
		const TText* langValues[8]={_S("Unknown"), _S("English"), _S("French"), _S("Spanish"),
			 _S("Japanese"), _S("Korean"), _S("Chinese"), _S("Hebrew"),};
		TBuf<20> langString(langValues[lang]);
		iConsole.Printf(_L("\n\tLanguage indicator: %S"), &langString);		
		}	
	
	// Message Deposit Index
	tia637::TMessageDepositIndex index;
	// This method is defined in the TMsvMessageCdma wrapper
	err = iMtm->SmsHeader().CdmaMessage().GetMessageDepositIndexL(index);
	if (err == KErrNone)	
		iConsole.Printf(_L("\n\tMessage deposit index: %d"), index);
	
	// Message Display Mode	 
	tia637::TBdMode display = 0;
	// This method is defined in the TMsvMessageCdma wrapper
	err = iMtm->SmsHeader().CdmaMessage().GetMessageDisplayModeL(display);
	if (err == KErrNone)
		{
		const TText* displayValues[3]={_S("Immediate"), _S("Default"), _S("User Invoke") };
		TBuf<20> displayString(displayValues[display]);
		iConsole.Printf(_L("\n\tDisplay Mode: %S"), &displayString);
		}

	// WEMT-only fields
	if (teleserviceId == tia637::KTeleserviceWEMT)
		{
		// This method is defined in the TMsvMessageSms wrapper
		const RPointerArray<const CEmsInformationElement>& ems = iMtm->SmsHeader().CdmaMessage().GetEMSInformationElementsL();
		TInt numEles = ems.Count();
		if (numEles) iConsole.Printf(_L("\nContains %d EMS elements:"),numEles);
		for (TInt i=0; i < numEles; i++) 
			{
			CSmsInformationElement::TSmsInformationElementIdentifier eleId = ems[i]->Identifier();
			switch (eleId)
				{
			case CSmsInformationElement::ESmsEnhancedTextFormatting: 	
				iConsole.Printf(_L("\n\tText Formatting"));
				break;
			case CSmsInformationElement::ESmsEnhancedPredefinedSound: 	
				iConsole.Printf(_L("\n\tPredefined Sound"));
				break;
			case CSmsInformationElement::ESmsEnhancedUserDefinedSound: 	
				iConsole.Printf(_L("\n\tUser-defined Sound"));
				break;
			case CSmsInformationElement::ESmsEnhancedPredefinedAnimation: 	
				iConsole.Printf(_L("\n\tPredefined Animation"));
				break;
			case CSmsInformationElement::ESmsEnhancedLargeAnimation: 	
				iConsole.Printf(_L("\n\tLarge Animation"));
				break;
			case CSmsInformationElement::ESmsEnhancedSmallAnimation: 	
				iConsole.Printf(_L("\n\tSmall Animation"));
				break;
			case CSmsInformationElement::ESmsEnhancedLargePicture: 	
				iConsole.Printf(_L("\n\tLarge Picture"));
				break;
			case CSmsInformationElement::ESmsEnhancedSmallPicture: 	
				iConsole.Printf(_L("\n\tSmall Picture"));
				break;
			case CSmsInformationElement::ESmsEnhancedVariablePicture: 	
				iConsole.Printf(_L("\n\tVariable Picture"));
				break;
			case CSmsInformationElement::ESmsEnhancedUserPromptIndicator: 	
				iConsole.Printf(_L("\n\tUser Prompt Indicator"));
				break;
			case CSmsInformationElement::ESmsEnhancedODI: 	
				iConsole.Printf(_L("\n\tODI"));
				break;
			default:
				break;				
				};
			}	
		}

	// Delete message once processing is done
    // Can only do this through the parent entry    
    CMsvEntry* parentEntry = CMsvEntry::NewL(iMtm->Session(), entry.Parent(), TMsvSelectionOrdering());
    CleanupStack::PushL(parentEntry);
    parentEntry->DeleteL(entry.Id());
    CleanupStack::PopAndDestroy(parentEntry);
	
	iConsole.Printf(_L("\n"));
	IncomingMenuL();		
	} 
	

/** 
Print acknowledgement status values for current MTM context.
*/
void CMenuHandler::PrintAcknowledgementStatus()
	{
	// summary information
	TMsvSmsEntry entry = static_cast<TMsvSmsEntry>(iMtm->Entry().Entry());
	TMsvSmsEntry::TMsvSmsEntryAckSummary summary = entry.AckSummary(ESmsAckTypeDelivery);
	if (summary == TMsvSmsEntry::EAllSuccessful)
		iConsole.Printf(_L("\nMessage was successfully delivered to all recipients"));
	
	// per recipient information
	const CArrayPtrFlat< CSmsNumber > & recipArray = iMtm->SmsHeader().Recipients();
	for (TInt i=0; i<recipArray.Count(); i++)
		{
		TPtrC address = recipArray[i]->Address();
		iConsole.Printf(_L("\nAcknowledgement status for recipient %S"),&address);
		
		iConsole.Printf(_L("\n\tDelivery: "));
		CSmsNumber::TSmsAckStatus ackStatus;
		ackStatus = recipArray[i]->AckStatus(ESmsAckTypeDelivery);
		PrintAckStatus(ackStatus);
		iConsole.Printf(_L("\n\tUser: "));
		ackStatus = recipArray[i]->AckStatus(ESmsAckTypeUser);
		PrintAckStatus(ackStatus);
		if (ackStatus == CSmsNumber::EAckSuccessful)
			{
			TUint8 userAck = recipArray[i]->UserAck();
			iConsole.Printf(_L("\n\tResponse code: %u"), userAck);		 
			}
			
		iConsole.Printf(_L("\n\tRead: "));
		ackStatus = recipArray[i]->AckStatus(ESmsAckTypeRead);
		PrintAckStatus(ackStatus);
		}
	}

/** Print acknowledgement status value.

@param ackStatus Acknowledgement status value
*/
void CMenuHandler::PrintAckStatus(CSmsNumber::TSmsAckStatus ackStatus)
	{
	switch (ackStatus)
		{
	case CSmsNumber::EPendingAck:
		iConsole.Printf(_L("The delivery status for this recipient has been requested, but the status report has not yet been received."));
		break; 
	case CSmsNumber::EAckSuccessful:
		iConsole.Printf(_L("A successful acknowledgement for this recipient has been received."));
		break;
	case CSmsNumber::EAckError:
		iConsole.Printf(_L("A failed acknowledgement for this recipient has been received."));
		break;
	case CSmsNumber::ENoAckRequested:
		iConsole.Printf(_L("An acknowledgement for this recipient has not been requested."));
		break;
	default:
		break;
		};
	}	



/** Prints a date/time value.

@param absTime Date/time value
*/
void CMenuHandler::PrintAbsoluteTimeL(const TDateTime& absTime)
	{
	TBuf<30> dateString;
	// format: e.g. 02/01/1997 11:59 pm
	_LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
	TTime time(absTime);
	time.FormatL(dateString,KDateString);
    iConsole.Printf(dateString); 		
	}

/** Prints a CDMA relative time value.

@param aTimeUnit Units, e.g. days, weeks.
@param aRelativeTime Value
@param aValidity True if time is a validity period, false if it's a delivery time
*/
void CMenuHandler::PrintRelativeTime(TSmsRelativeTimeUnit aTimeUnit, TUint32 aRelativeTime, TBool aValidity)
	{
	TBuf<30> timeString;
	switch (aTimeUnit)
		{
		case ESmsTimeUnitFiveMinutes:
			timeString.Format(_L("%u minutes"),aRelativeTime*5);				
			break;
		case ESmsTimeUnitHalfHours:
			timeString.Format(_L("%u minutes"),aRelativeTime*30);				
			break;
		case ESmsTimeUnitDays:
			timeString.Format(_L("%u days"),aRelativeTime);				
			break;
		case ESmsTimeUnitWeeks:
			timeString.Format(_L("%u weeks"),aRelativeTime);				
			break;
		case ESmsTimeUnitIndefinite:
			timeString = _L("Indefinite");				
			break;
		case ESmsTimeUnitImmediate:
			timeString = _L("Immediate");				
			break;
		case ESmsTimeUnitValidWhileActive:
			if (aValidity)
				// validity string
				timeString = _L("Until mobile becomes inactive");				
			else // delivery string	
				timeString = _L("When mobile next becomes active");				
			break;
		case ESmsTimeUnitWhileInValidRegistrationArea:
			timeString = _L("Until registration area changes");				
			break;				
		default:
			timeString = _L("not set");				
			break;
		};								
	iConsole.Printf(timeString);
	}
