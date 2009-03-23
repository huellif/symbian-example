// TXCLIENT.CPP
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__TXTRICH_H__)
#include <txtrich.h>
#endif

#if !defined(__MSVREG_H__)
#include <msvreg.h>	
#endif
#if !defined(__MSVSTORE_H__)
#include <msvstore.h>
#endif	// CMsvStore
#if !defined(__MSVIDS_H__)
#include <MSVIDS.H>	
#endif	// KMsvLocalServiceIndexEntryId
#if !defined(__MTMDEF_H__)
#include <MTMDEF.H>	
#endif	// KMsvMessagePartBody
#if !defined(__MSVUIDS_H__)
#include <MSVUIDS.H>
#endif	// KUidMsvMessageEntry

#if !defined(__TXUT_H__)
#include "txut.h"
#endif
#if !defined(_TXTCMDS_H_)
#include "txtcmds.h"	// Commands that can be passed on to the server side.
#endif

#if !defined(__TXTCPAN_H__)
#include "txtcpan.h"
#endif	// Panics
#if !defined(__TXCLIENT_H__)
#include "txclient.h"
#endif


EXPORT_C CTextMtmClient* CTextMtmClient::NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession)
// Factory function
	{
	CTextMtmClient* self = new (ELeave) CTextMtmClient(aRegisteredMtmDll,aMsvSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTextMtmClient::CTextMtmClient(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession)
: CBaseMtm(aRegisteredMtmDll,aMsvSession)
	{
	__ASSERT_DEBUG(Type()==KUidMsgTypeText,gPanic(ETxtcBadMtmTypeUid));
	}

void CTextMtmClient::ConstructL()
	{
	iSettings = CMTMTxtSettings::NewL();
	SwitchCurrentEntryL(KMsvRootIndexEntryId);
	}

CTextMtmClient::~CTextMtmClient()
	{
	delete iSettings;
	}

void CTextMtmClient::SaveMessageL()
// Store entry data 
// If it is a service entry, store the current settings. Otherwise it has to be
// a message. 
//
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(ETxtcNoCMsvEntrySet));
	// get an editable message store
	CMsvStore* store = iMsvEntry->EditStoreL();
	CleanupStack::PushL(store);

	switch (iMsvEntry->Entry().iType.iUid)
		{
		case KUidMsvServiceEntryValue:
			iSettings->SaveSettingsL(iMsvEntry->Entry().Id(), iTxtSettings);
			break;
		case KUidMsvMessageEntryValue:
			StoreBodyL(*store);	
			break;
		default:
			__ASSERT_DEBUG(EFalse, gPanic(ETxtcEntryTypeNotSupported));
			break;
		}

	store->CommitL();
	CleanupStack::PopAndDestroy(); // store
	}

void CTextMtmClient::LoadMessageL()
// Restore entry data 
// Operation inverse to StoreL
//
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(ETxtcNoCMsvEntrySet));
	// get read-only message store
	CMsvStore* store = iMsvEntry->ReadStoreL();
	CleanupStack::PushL(store);

	switch (iMsvEntry->Entry().iType.iUid)
		{
		case KUidMsvServiceEntryValue:
			iSettings->LoadSettingsL(iMsvEntry->Entry().Id(), iTxtSettings);
			break;
		case KUidMsvMessageEntryValue:
			RestoreBodyL(*store);	
			break;
		default:
			__ASSERT_DEBUG(EFalse, gPanic(ETxtcEntryTypeNotSupported));
			break;
		}

	CleanupStack::PopAndDestroy(); // store
	}


CMsvOperation* CTextMtmClient::ReplyL (TMsvId /*aReplyEntryId*/, TMsvPartList /*aPartlist*/, 
									   TRequestStatus& /*aCompletionStatus*/)
// Create reply message 
// Reply is not defined
//
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}


CMsvOperation* CTextMtmClient::ForwardL(TMsvId aForwardEntryId, TMsvPartList aPartList, 
										TRequestStatus& aCompletionStatus)
// Create forwarded message 
// Destination folder is aForwardEntryL
//
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(ETxtcNoCMsvEntrySet));
	__ASSERT_DEBUG(iMsvEntry->Entry().iType.iUid == KUidMsvMessageEntryValue, gPanic(ETxtcEntryTypeNotSupported));
	__ASSERT_DEBUG(iMsvEntry->Entry().iServiceId == KMsvLocalServiceIndexEntryId, gPanic(ETxtcInvalidServiceId));
	
	// Create the forwarded index entry
	TMsvEntry forwardEntry;
	forwardEntry.iMtm = KUidMsgTypeText;
	forwardEntry.iServiceId = Entry().Entry().iServiceId;
	forwardEntry.iType = KUidMsvMessageEntry;
	forwardEntry.iDetails.Set(iMsvEntry->Entry().iDetails);				
	forwardEntry.iSize = iMsvEntry->Entry().iSize;	
	if(aPartList&KMsvMessagePartDate)
		forwardEntry.iDate.HomeTime();
	if(aPartList&KMsvMessagePartDescription)
		forwardEntry.iDescription.Set(iMsvEntry->Entry().iDescription);		

	// Get CMsvEntry for destination (parent)
	CMsvEntry* cEntry = CMsvEntry::NewL(Session(), aForwardEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(cEntry);
	// Synchronously create new child
	CMsvOperationActiveSchedulerWait* wait = CMsvOperationActiveSchedulerWait::NewLC();
	CMsvOperation* opert = cEntry->CreateL(forwardEntry, wait->iStatus);
	CleanupStack::PushL(opert);
	wait->Start();
	User::LeaveIfError(opert->iStatus.Int());
	// Check result
	TPckgBuf<TMsvLocalOperationProgress> progressPack;
    progressPack.Copy(opert->ProgressL());
	TMsvLocalOperationProgress progress = progressPack();
	User::LeaveIfError(progress.iError);
	CleanupStack::PopAndDestroy(2);	// opert, wait

	// Get CMsvEntry for new entry
	TMsvId forwardId=progress.iId;
	cEntry->SetEntryL(forwardId);
	
	// Populate new forwarded message with Body text
	if(aPartList&KMsvMessagePartBody)
		{
		CMsvStore* store=cEntry->EditStoreL();
		CleanupStack::PushL(store);
		StoreBodyL(*store);				// Current context is original message
		store->CommitL();
		CleanupStack::PopAndDestroy();	// store
		}

	CleanupStack::PopAndDestroy(); // cEntry
		
	// Request was performed synchronously, so return a completed operation object
	return CMsvCompletedOperation::NewL(Session(), KUidMsgTypeText, progressPack, 
		KMsvNullIndexEntryId, aCompletionStatus);
	}


TMsvPartList CTextMtmClient::ValidateMessage(TUint aPartList)
// Message validation
//
// The only thing about the message that can be invalid
// is the iDescription, which should be an acceptable file name. So, length isn't 
// allowed to be zero, and backslashes are not allowed in iDescription.
//
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL,gPanic(ETxtcNoCMsvEntrySet));
	TInt retVal=0;
	if (aPartList & KMsvMessagePartDescription)
		{
		if (iMsvEntry->Entry().iDescription.Length() == 0)
			{
			retVal |= KMsvMessagePartDescription;
			}
		else if (iMsvEntry->Entry().iDescription.Locate( TChar('\\')) >= 0)
			{
			retVal |= KMsvMessagePartDescription;
			}
		}
	return retVal;
	}


TMsvPartList CTextMtmClient::Find(const TDesC& aTextToFind, TMsvPartList aPartList)
// Find text in entry 
	{
	__ASSERT_DEBUG(iMsvEntry!=NULL, gPanic(ETxtcNoCMsvEntrySet));
	TBool found=EFalse;
	if (aPartList & KMsvMessagePartBody)
		{
		CRichText *body = &Body();
		if (body != NULL)
			{
			TInt textSize = body->DocumentLength();
			// Get searchable text

			if (textSize>0)  // Symbian OS doesn't handle allocing zero byte arrays too
				{               
				TText *bodyText = new TText[textSize];
				if (bodyText != NULL)
					{
					// search the rich text
					TPtr p(bodyText,textSize,textSize);
					body->Extract(p);
					if (p.FindF( aTextToFind ) >= 0)
						{
						found = ETrue;
						}
					}
				delete bodyText;
				}
			}
		}

	if (aPartList & KMsvMessagePartOriginator)
		{
		if (iMsvEntry->Entry().iDetails.FindF( aTextToFind ) >= 0)
			{
			found=ETrue;
			}
		}
	
	if (aPartList & KMsvMessagePartDescription)
		{
		if (iMsvEntry->Entry().iDescription.FindF( aTextToFind ) >= 0)
			{
			found=ETrue;
			}
		}
	
	// Never searched - KMsvMessagePartAttachments, KMsvMessagePartRecipient 
	// and KMsvMessagePartDate
	return found;
	}

TInt CTextMtmClient::QueryCapability(TUid aCapability, TInt& aResponse)
// Query for capability 
	{
	TInt error = KErrNone;
	switch (aCapability.iUid)
		{
		case KUidMtmQueryMaxBodySizeValue:
		case KUidMtmQueryMaxTotalMsgSizeValue:
			aResponse = KMaxTextMessageSize;
			break;
		case KUidMtmQuerySupportedBodyValue:
			aResponse = KMtm8BitBody + KMtm7BitBody;
			break;
		case KUidMtmQueryOffLineAllowedValue:
		case KUidMtmQueryCanSendMsgValue:
		case KUidMtmQueryCanReceiveMsgValue:
			aResponse = ETrue;
			break;
		case KUidMtmQuerySupportAttachmentsValue:
		default:
			return KErrNotSupported;
		}
	return error;
	}

void CTextMtmClient::InvokeSyncFunctionL(TInt aFunctionId,const CMsvEntrySelection& aSelection, 
										 TDes8& aParameter)
// Call MTM-specific operation synchronously
	{
	TInt error = KErrNone;
	CMsvOperationActiveSchedulerWait* wait=CMsvOperationActiveSchedulerWait::NewLC();
	CMsvOperation* operation = InvokeAsyncFunctionL(aFunctionId,aSelection, aParameter, wait->iStatus);
	if (operation != NULL)
		{
		wait->Start();
		error = wait->iStatus.Int();
		delete operation;
		}
	else
		{
		error=KErrNotSupported;
		}
	CleanupStack::PopAndDestroy(); // wait
	User::LeaveIfError(error);
	}


CMsvOperation* CTextMtmClient::InvokeAsyncFunctionL(TInt aFunctionId,
													const CMsvEntrySelection& aSelection, 
													TDes8& aParameter, 
													TRequestStatus& aCompletionStatus)
// Call MTM-specific operation asynchronously
	{
	CMsvOperation* operation = NULL;
	switch (aFunctionId)
		{
		case KTXTMTMRefresh:
            {
            operation = (Entry().Session().TransferCommandL(aSelection, aFunctionId, aParameter, aCompletionStatus));
            }
			break;
		default:
			__ASSERT_DEBUG(EFalse,gPanic(ETxtcCommandNotSupported));
			break;
		}
	return operation;
	}


void CTextMtmClient::ContextEntrySwitched() 
// Context change notification 
//
// No need to know entry changes
//
	{
	}

// attachments are not supported for this MTM
void CTextMtmClient::CreateAttachmentL(const TDesC& , const TDesC8& , TRequestStatus& )
	{
	User::Leave(KErrNotSupported);
	}
	
void CTextMtmClient::CreateAttachmentL(RFile& , const TDesC8& , TRequestStatus& )
	{
	User::Leave(KErrNotSupported);
	}

void CTextMtmClient::CreateLinkedAttachmentL(const TDesC& , const TDesC8& , TRequestStatus& )
	{
	User::Leave(KErrNotSupported);
	}

void CTextMtmClient::CreateMessageAttachmentL(TMsvId , TRequestStatus& )
	{
	User::Leave(KErrNotSupported);
	}

// delegate default service storage to CMTMTxtSettings
TMsvId CTextMtmClient::DefaultServiceL() const
	{
	return iSettings->DefaultServiceL();
	}
	
void CTextMtmClient::RemoveDefaultServiceL()
	{
	iSettings->DeleteDefaultServiceSettingL();
	}
	
void CTextMtmClient::ChangeDefaultServiceL(const TMsvId& aService)
	{
	iSettings->SetDefaultServiceL(aService);	
	}

