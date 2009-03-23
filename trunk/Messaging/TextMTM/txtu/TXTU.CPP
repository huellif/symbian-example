// TXTU.CPP
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.


// Standard includes
#include <txtrich.h>//CRichText
#include <EIKRTED.H> //	CEikRichTextEditor
#include <APFFNDR.H>//CApaScanningAppFinder
#include <eikenv.H>//CEikonEnv
#include <eikfsel.h> //CEikFolderNameSelector
#include <eikon.hrh>  //EEikBidCancel, EEikBidTab
#include <eikon.rsg>

// Messaging includes
#include <MSVUIDS.H>
#include <msvids.h>

// Specific includes
#include "txtcmds.h" //commands accepted by the server side mtms
#include "TXCLIENT.H"

#include "TXTU.HRH"
#include "TXTU.h"
#include "TXTUcmds.h"
#include "TXTUpan.h"
#include <TXTU.rsg>
#include "TXTU.HRH"

// resource file locations
#ifdef __WINS__
// on wins, assume built to z:
_LIT(KTxtuMtmUiResourceFile,"txtu.rsc");
#else
_LIT(KTxtuMtmUiResourceFile,"c:\\resource\\messaging\\txtu.rsc");
#endif

//
// CTextMtmUi: User Interface MTM
//

//
//	Construction and destruction 
//

CTextMtmUi* CTextMtmUi::NewL(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll)
	{
	CTextMtmUi* self=new(ELeave) CTextMtmUi(aBaseMtm, aRegisteredMtmDll);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTextMtmUi::CTextMtmUi(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll)
	:	CBaseMtmUi(aBaseMtm, aRegisteredMtmDll)
	{
	}

CTextMtmUi::~CTextMtmUi()
	{
	}

void CTextMtmUi::ConstructL()
	{
	CBaseMtmUi::ConstructL();
	}

CMsvOperation* CTextMtmUi::CancelL(TRequestStatus& /*aStatus*/, const CMsvEntrySelection& /*aSelection*/)
	{
	User::Leave(KErrNotSupported); // no cancelling
	return NULL;
	}

void CTextMtmUi::GetResourceFileName(TFileName& aFileName) const
// Resource file loading 
	{ 
	aFileName=KTxtuMtmUiResourceFile; 
	}

//
//	Entry manipulation 
//

CMsvOperation* CTextMtmUi::OpenL(TRequestStatus& aStatus)
// Open 
	{
	__ASSERT_DEBUG(iBaseMtm.Entry().Entry().iType==KUidMsvMessageEntry, 
		Panic(ETextMtmUiOnlyWorksWithMessageEntries));
	return EditL(aStatus);
	}

CMsvOperation* CTextMtmUi::OpenL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection)
// Open selection
	{
	__ASSERT_DEBUG(aSelection.Count(), Panic(ETextMtmUiEmptySelection));
	iBaseMtm.SwitchCurrentEntryL(aSelection.At(0));
	return OpenL(aStatus);
	}

CMsvOperation* CTextMtmUi::CloseL(TRequestStatus& aStatus)
	{
// Close 
	__ASSERT_DEBUG(iBaseMtm.Entry().Entry().iType==KUidMsvMessageEntry, 
		Panic(ETextMtmUiOnlyWorksWithMessageEntries));

	// Nothing to do for this MTM
	TPckgBuf<TMsvLocalOperationProgress> progress;
	SetProgressSuccess(progress,iBaseMtm.Entry().Entry().Id());

	CMsvCompletedOperation* op=CMsvCompletedOperation::NewL(Session(), Type(), 
		progress, KMsvLocalServiceIndexEntryId,aStatus);
	return op;
	}

CMsvOperation* CTextMtmUi::CloseL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection)
	{
// Close selection
	__ASSERT_DEBUG(aSelection.Count(), Panic(ETextMtmUiEmptySelection));
	iBaseMtm.SwitchCurrentEntryL(aSelection.At(0));
	return CloseL(aStatus);
	}

CMsvOperation* CTextMtmUi::CreateL(const TMsvEntry& aEntry, CMsvEntry& aParent, 
								   TRequestStatus& aStatus)
// Create new entry 
	{
	TMsvId oldId=0;
	if (iBaseMtm.HasContext())
		oldId = iBaseMtm.Entry().Entry().Id();	
	else
		iBaseMtm.SwitchCurrentEntryL(KMsvRootIndexEntryId);
	
	// Create the new entry synchronously
	TMsvEntry newEntry=aEntry;
	if (newEntry.iType.iUid == KUidMsvMessageEntryValue)
		{
		// Some extra info for new messages
		_LIT(KNewText,"New Text");
		newEntry.iServiceId=KMsvLocalServiceIndexEntryId;
		newEntry.iDescription.Set(KNewText);
		}
	CMsvOperationActiveSchedulerWait* wait=CMsvOperationActiveSchedulerWait::NewLC();
	CMsvOperation* op=CBaseMtmUi::CreateL(newEntry, aParent, wait->iStatus);
	CleanupStack::PushL(op);
	wait->Start();

	TPckgBuf<TMsvLocalOperationProgress> progressPack;
    progressPack.Copy(op->ProgressL());
	TMsvLocalOperationProgress progress = progressPack();
	User::LeaveIfError(progress.iError);

	CleanupStack::PopAndDestroy(2);// op, wait

	// Now edit the entry
	iBaseMtm.SwitchCurrentEntryL(progress.iId);
	TRAPD(ret, op=EditL(aStatus));
	if (ret!=KErrNone)
		{// An error has occurred before ownership of entry passed over to editor, 
		//	so try and delete the new entry, and return context to its previous state
		wait = CMsvOperationActiveSchedulerWait::NewLC();
		CMsvEntry* entry=Session().GetEntryL(iBaseMtm.Entry().Entry().Parent());
		CleanupStack::PushL(entry);
		wait->Start();
		entry->DeleteL(progress.iId);
		CleanupStack::PopAndDestroy(2);// wait, entry
		if (oldId)
			iBaseMtm.SwitchCurrentEntryL(oldId);
		User::Leave(ret);
		}
	return op;
	}


CMsvOperation* CTextMtmUi::EditL(TRequestStatus& aStatus)
// Edit 
	{
	TUid type = iBaseMtm.Entry().Entry().iType;
	iServiceId = iBaseMtm.Entry().Entry().iServiceId;

	__ASSERT_DEBUG(type==KUidMsvMessageEntry || type==KUidMsvServiceEntry, 
		Panic(ETextMtmUiWrongEntryType));

	if ( type == KUidMsvMessageEntry )
		return MessageEditL(aStatus);
	else
		return ServiceEditL(aStatus);
	}

CMsvOperation* CTextMtmUi::MessageEditL(TRequestStatus& aStatus)
// Message editing
	{
	CMsvOperation* op=NULL;

	// If the message is already stored locally, edit it.
	if ( iServiceId == KMsvLocalServiceIndexEntryId )
		{
		return LaunchEditorApplicationL(aStatus, iBaseMtm.Entry().Entry().Id(), EFalse);
		}

	// Otherwise ask user if she wants to copy it to local
	CEikDialog* dialog=new(ELeave) CEikDialog;
	if (dialog->ExecuteLD(R_DIALOG_REFUSE_EDIT_REMOTE_MESSAGE))
		{
		TMsvEntry tEntry = iBaseMtm.Entry().Entry();
		CEditRemoteOperation* edop = new (ELeave)CEditRemoteOperation(*this, Session(), 0, aStatus);
		edop->Start(tEntry);
		op = edop;
		}
	else
		{
		TPckgBuf<TMsvLocalOperationProgress> progress;
		SetProgressSuccess(progress,iBaseMtm.Entry().Entry().Id());

		op=CMsvCompletedOperation::NewL(Session(), Type(), progress, iServiceId, aStatus);
		}
		
	__ASSERT_DEBUG(op != NULL, Panic(ETextMtmUiInvalidNullPointer));
	return op;
	}


CMsvOperation* CTextMtmUi::ServiceEditL(TRequestStatus& aStatus)
// Service edit dialog
	{
	TMTMTxtSettings settings;

	// load any existing settings
	CTextMtmClient& mtm = static_cast<CTextMtmClient&>(BaseMtm());
	// leaves if none found, which is acceptable, so ignore leave 
	TRAP_IGNORE(mtm.Settings().LoadSettingsL(iBaseMtm.Entry().Entry().Id(), settings));	

	// run the edit service settings dialog
	CEikDialog* dialog=new(ELeave) CTextMtmEditServiceDialog(settings);
	TPckgBuf<TMsvLocalOperationProgress> progress;
	if (dialog->ExecuteLD(R_DIALOG_SERVICE_SETTINGS))
		{
		mtm.Settings().SaveSettingsL(iBaseMtm.Entry().Entry().Id(), settings);		
		// Update index entry description
		TMsvEntry entry = iBaseMtm.Entry().Entry();
		entry.iDetails.Set(settings.RootFolder());
		iBaseMtm.Entry().ChangeL(entry);
		// Send a refresh service command
		CMsvEntrySelection* selection=new (ELeave) CMsvEntrySelection;
		CleanupStack::PushL(selection);
		selection->AppendL(iBaseMtm.Entry().Entry().Id());
		CleanupStack::Pop(); //selection
		TBuf8<1> scrap;
		InvokeSyncFunctionL(ETxtuCommandRefreshMBox, *selection, scrap);
		delete selection;
		SetProgressSuccess(progress,iBaseMtm.Entry().Entry().Id());
		}
	else
		{
		progress().iTotalNumberOfEntries=1;
		progress().iNumberCompleted=0;
		}

	return CMsvCompletedOperation::NewL(Session(), Type(), progress, iServiceId, aStatus);
	}
	
CMsvOperation* CTextMtmUi::EditL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection)
// Edit selection
	{
	__ASSERT_DEBUG(aSelection.Count(), Panic(ETextMtmUiEmptySelection));
	iBaseMtm.SwitchCurrentEntryL(aSelection.At(0));
	return EditL(aStatus);
	}

CMsvOperation* CTextMtmUi::ViewL(TRequestStatus& aStatus)
// View
	{
	__ASSERT_DEBUG(iBaseMtm.Entry().Entry().iType==KUidMsvMessageEntry, 
		Panic(ETextMtmUiOnlyWorksWithMessageEntries));
	return LaunchEditorApplicationL(aStatus, iBaseMtm.Entry().Entry().Id(), ETrue);
	}

CMsvOperation* CTextMtmUi::ViewL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection)
// View selection
	{
	__ASSERT_DEBUG(aSelection.Count(), Panic(ETextMtmUiEmptySelection));
	iBaseMtm.SwitchCurrentEntryL(aSelection.At(0));
	return ViewL(aStatus);
	}

//
//	Copy/move/delete functionality
//


CMsvOperation* CTextMtmUi::DeleteFromL(const CMsvEntrySelection& aSelection,
									   TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(aSelection.Count(), Panic(ETextMtmUiEmptySelection));

	CheckValidCopyMoveDelSelectionL(BaseMtm().Entry(),aSelection);
	return BaseMtm().Entry().DeleteL(aSelection, aStatus);
	}

CMsvOperation* CTextMtmUi::DeleteServiceL(const TMsvEntry& aService, TRequestStatus& aStatus)
	{
	// remove settings from cenrep
	CTextMtmClient& mtm = static_cast<CTextMtmClient&>(BaseMtm());
	mtm.Settings().DeleteSettingsL(aService.Id());
	return CBaseMtmUi::DeleteServiceL(aService, aStatus);
	}

CMsvOperation* CTextMtmUi::CopyToL(const CMsvEntrySelection& aSelection, 
											TRequestStatus& aStatus)
// Copies entries in selection to the current context
	{
	return DoCopyMoveToL(aSelection, aStatus, ETrue);
	}


CMsvOperation* CTextMtmUi::MoveToL(const CMsvEntrySelection& aSelection, 
											TRequestStatus& aStatus)
// Moves entries in selection to the current context
	{
	return DoCopyMoveToL(aSelection, aStatus, EFalse);
	}


CMsvOperation* CTextMtmUi::CopyFromL(const CMsvEntrySelection& aSelection, 
									 TMsvId aTargetId, TRequestStatus& aStatus)
// Copies entries in selection from current context to aTargetId id	
	{
	return DoCopyMoveFromL(aSelection, aTargetId, aStatus, ETrue);
	}


CMsvOperation* CTextMtmUi::MoveFromL(const CMsvEntrySelection& aSelection, 
									 TMsvId aTargetId, TRequestStatus& aStatus)
// Moves entries in selection from current context to aTargetId id	
	{
	return DoCopyMoveFromL(aSelection, aTargetId, aStatus, EFalse);
	}


CMsvOperation* CTextMtmUi::DoCopyMoveFromL(const CMsvEntrySelection& aSelection, 
									 TMsvId aTargetId, TRequestStatus& aStatus, TBool aAction)
// Does copy/move of entries in selection from current context to aTargetId id
	{
	__ASSERT_DEBUG(aSelection.Count(), Panic(ETextMtmUiEmptySelection));

	const TMsvEntry& context=BaseMtm().Entry().Entry();
	__ASSERT_ALWAYS(context.iType.iUid==KUidMsvServiceEntryValue || 
		context.iType.iUid==KUidMsvFolderEntryValue, Panic(ETextMtmUiWrongEntryType));

	CheckValidCopyMoveDelSelectionL(BaseMtm().Entry(),aSelection);

	// Call the CMsvEntry copy/move functions: this will in turn call the Message Server
	CMsvOperation* op=NULL;
	if (aAction)
		op=BaseMtm().Entry().CopyL(aSelection, aTargetId, aStatus);
	else
		op=BaseMtm().Entry().MoveL(aSelection, aTargetId, aStatus);

	return op;
	}


CMsvOperation* CTextMtmUi::DoCopyMoveToL(const CMsvEntrySelection& aSelection, 
											TRequestStatus& aStatus, TBool aAction)
// Perform a copy/move (depending on value of aAction)
	{
	__ASSERT_DEBUG(aSelection.Count(), Panic(ETextMtmUiEmptySelection));

	const TMsvEntry& context=BaseMtm().Entry().Entry();
	__ASSERT_ALWAYS(context.iType.iUid==KUidMsvServiceEntryValue || 
		context.iType.iUid==KUidMsvFolderEntryValue, Panic(ETextMtmUiWrongEntryType));

	// Find selection parent
	CMsvEntry* sourceEntry=Session().GetEntryL(aSelection.At(0));
	CleanupStack::PushL(sourceEntry);

	TMsvId parent=sourceEntry->Entry().Parent();
	sourceEntry->SetEntryL(parent);

	CheckValidCopyMoveDelSelectionL(*sourceEntry,aSelection);

	// Call the CMsvEntry copy/move function: this will in turn call the Message Server
	CMsvOperation* op=NULL;
	if (aAction)
		op=sourceEntry->CopyL(aSelection, context.Id(), aStatus);
	else
		op=sourceEntry->MoveL(aSelection, context.Id(), aStatus);

	CleanupStack::PopAndDestroy();// sourceEntry

	return op;
	}

void CTextMtmUi::CheckValidCopyMoveDelSelectionL(const CMsvEntry& aParent,
												 const CMsvEntrySelection& aSelection)
// Check selection and confirm it contains only messages
// As Server-side doesn't handle folders
	{
	// We have a ready-made way of getting a list of the message children to check against
	CMsvEntrySelection* msgChildren=aParent.ChildrenWithTypeL(KUidMsvMessageEntry);
	CleanupStack::PushL(msgChildren);
	
	// Check each aSelection entry is a child message
	TInt nChildren=aSelection.Count();
	for (TInt i=0; i<nChildren; i++)
		if ( msgChildren->Find(aSelection[i] ) == KErrNotFound )
			User::Leave(KErrNotSupported);
	CleanupStack::PopAndDestroy();// msgChildren
	}


//
//	MTM-specific functionality
//

void CTextMtmUi::InvokeSyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, 
									 TDes8& aParameter)
// Call MTM-specific function synchronously
	{
	if (aFunctionId==ETxtuCommandExportText)
		{
		__ASSERT_DEBUG(aSelection.Count()==1, Panic(ETextMtmUiSelectionIsNotOneMessage));
		DoExportTextFromMessageL(aSelection.At(0));// take the first message in selection
		}
	else if (aFunctionId==ETxtuCommandRefreshMBox)
		{
		CBaseMtmUi::InvokeSyncFunctionL(KTXTMTMRefresh, aSelection, aParameter);
		}
	else
		{
		CBaseMtmUi::InvokeSyncFunctionL(aFunctionId, aSelection, aParameter);
		}
	}

CMsvOperation* CTextMtmUi::InvokeAsyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, 
												TRequestStatus& aCompletionStatus, TDes8& aParameter)
// Call MTM-specific function asynchronously
	{
	return CBaseMtmUi::InvokeAsyncFunctionL(aFunctionId, aSelection, aCompletionStatus, aParameter);
	}

//
//	Message responding
//

CMsvOperation* CTextMtmUi::ReplyL(TMsvId /*aDestination*/, TMsvPartList /*aPartlist*/, TRequestStatus& /*aCompletionStatus*/)
// Reply to message - no UI support
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CMsvOperation* CTextMtmUi::ForwardL(TMsvId /*aDestination*/, TMsvPartList /*aPartlist*/, TRequestStatus& /*aCompletionStatus*/)
// Forwarded message - no UI support
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

//
//	Progress information
//

TInt CTextMtmUi::GetProgress(const TDesC8& aProgress, TBuf<EProgressStringMaxLen>& aReturnString, 
		TInt& aTotalEntryCount, TInt& aEntriesDone,TInt& aCurrentEntrySize, TInt& aCurrentBytesTrans) const
// Get progress information 
	{
	TTxtProgressBuf paramPack;
	if (aProgress.Length()!=paramPack.Length())
		return KErrNone;

	// Only the server-side copy/move/del fill in the progress
	// A good idea would be to add a flag to the progress info to show which was happening
	paramPack.Copy(aProgress);	
	TTxtProgress progress=paramPack();
	aReturnString.Zero();
	_LIT(KWorking,"Working");
	_LIT(KError,"Error: ");

	// Progress information contains any error returned from the server-side
	// It's handling here is very simple
	if (progress.iErrorCode==KErrNone)
		aReturnString.Append(KWorking);
	else
		{
		aReturnString.Append(KError);
		aReturnString.AppendNum(progress.iErrorCode);
		}

	// Copy info from progress
	aTotalEntryCount=progress.iTotalMsgs;
	aEntriesDone=progress.iMsgsProcessed;

	// TTxtProgress doesn't hold anything about these
	aCurrentEntrySize=0;
	aCurrentBytesTrans=0;

	return KErrNone;
	}

//
// Specific to CTextMtmUi
//

void CTextMtmUi::DoExportTextFromMessageL(TMsvId anId)
// Export message text to file
	{
	TFileName filename(_S("C:\\"));
	HBufC* title=iCoeEnv->AllocReadResourceLC(R_TEXT_MTMUI_EXPORT_DIALOG_TITLE);
	CEikDialog* dialog=new(ELeave) CTextMtmExportToFileDialog(&filename, title);
	if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_SAVEAS))
		{
		// Get the entry to export the text from
		CMsvEntry* messageEntry=GetMessageEntryLC(anId);
		
		// Get the store for the message
		CMsvStore* store=messageEntry->ReadStoreL();
		CleanupStack::PushL(store);

		// Now get the rich text from that message
		CRichText* text=GetTextFromMessageStoreL(store);

		// No longer need entry or store, so discard them
		CleanupStack::PopAndDestroy(2);// messageEntry, store
		
		// Now export the text to the given file
		CleanupStack::PushL(text);
		text->ExportAsTextL(filename, CPlainText::EOrganiseByParagraph, 80);
		CleanupStack::PopAndDestroy();// text
		}
	CleanupStack::PopAndDestroy();// title
	}

CMsvEntry* CTextMtmUi::GetMessageEntryLC(TMsvId anId) const
	{
	CMsvEntry* messageEntry=Session().GetEntryL(anId);
	CleanupStack::PushL(messageEntry);
	return messageEntry;
	}

CRichText* CTextMtmUi::GetTextFromMessageStoreL(CMsvStore* aStore) const
	{
	CRichText* text=CRichText::NewL(iEikonEnv->SystemParaFormatLayerL(), 
		iEikonEnv->SystemCharFormatLayerL());
	CleanupStack::PushL(text);
	aStore->RestoreBodyTextL(*text);
	CleanupStack::Pop();// text
	return text;
	}

CMsvOperation* CTextMtmUi::LaunchEditorApplicationL(TRequestStatus& aStatus, TMsvId aId, 
													TBool aReadOnly)
	{
// In a real MTM, would launch the appropriate editor/viewer, and return a controlling CMsvOperation
// Here we just pretend...
	if (aReadOnly)
		iEikonEnv->InfoMsg(R_TEXT_MTMUI_VIEWER_MESSAGE);
	else
		{
		// For editing, append a bit of text to the message
		iEikonEnv->InfoMsg(R_TEXT_MTMUI_EDIT_MESSAGE);

		_LIT(KEdited,"Edited document\n");
		iBaseMtm.SwitchCurrentEntryL(aId);
		CreateBodyIfNoneL();
		iBaseMtm.LoadMessageL();
		TInt docEnd=iBaseMtm.Body().DocumentLength();
		iBaseMtm.Body().InsertL(docEnd,KEdited);
		iBaseMtm.SaveMessageL();
		}

	TPckgBuf<TMsvLocalOperationProgress> progress;
	SetProgressSuccess(progress,aId);

	return CMsvCompletedOperation::NewL(Session(), Type(), progress, iServiceId, aStatus);
	}

void CTextMtmUi::SetProgressSuccess(TPckgBuf<TMsvLocalOperationProgress>& aProgress,TMsvId aId)
// Little utility function to set progress info
	{
	aProgress().iTotalNumberOfEntries=1;
	aProgress().iNumberCompleted=1;
	aProgress().iId=aId;
	}

void CTextMtmUi::CreateBodyIfNoneL()
// Creates body text stream for context if doesn't have one already (i.e. is newly created)
	{
	__ASSERT_DEBUG(iBaseMtm.Entry().Entry().iType.iUid == KUidMsvMessageEntryValue, 
		Panic(ETextMtmUiOnlyWorksWithMessageEntries));

	CMsvStore* store=iBaseMtm.Entry().EditStoreL();
	CleanupStack::PushL(store);
	
	if (!store->HasBodyTextL())
		{
		// Create CRichText object
		CRichText* body = CRichText::NewL(iEikonEnv->SystemParaFormatLayerL(), 
			iEikonEnv->SystemCharFormatLayerL());
		CleanupStack::PushL(body);

		// Store CRichText in body
		store->StoreBodyTextL(*body);
		store->CommitL();

		CleanupStack::PopAndDestroy(); // body
		}
	CleanupStack::PopAndDestroy(); // store
	}

//
//	CEditRemoteOperation: remote edit operation
//

CEditRemoteOperation::CEditRemoteOperation(CTextMtmUi& aMtmUi, CMsvSession& aMsvSession, 
										   TInt aPriority, TRequestStatus& aObserverRequestStatus)
	: CMsvOperation(aMsvSession, aPriority, aObserverRequestStatus), iMtmUi(aMtmUi)
	{
	CActiveScheduler::Add(this);
	}

CEditRemoteOperation::~CEditRemoteOperation()
	{
	Cancel();
	delete iCopyOperation;
	delete iEditOperation;
	}

const TDesC8& CEditRemoteOperation::ProgressL() 
	{
	return progress;
	}

void CEditRemoteOperation::DoCancel()
	{
	switch (iState)
		{
		case EStateCopying:
			iCopyOperation->Cancel();
			break;
		case EStateEditing:
			iEditOperation->Cancel();
			break;
		default:
			break;
		}
	}

void CEditRemoteOperation::Start(TMsvEntry& aSourceEntry)
	{
	CMsvEntry* cEntry=iMsvSession.GetEntryL(aSourceEntry.Parent());
	CleanupStack::PushL(cEntry);
	iObserverRequestStatus = KRequestPending;
	SetActive();
	iCopyOperation=cEntry->CopyL(aSourceEntry.Id(), KMsvGlobalInBoxIndexEntryId, iStatus);
	CleanupStack::PopAndDestroy();// cEntry
	iState = EStateCopying;
	}

void CEditRemoteOperation::RunL()
	{
	switch (iState)
		{
		case EStateCopying:
			{
			TTxtProgressBuf package;
			package.Copy(iCopyOperation->ProgressL());
			TMsvId newid = package().iNewId;

			iStatus = KRequestPending;
			SetActive();
			iEditOperation = iMtmUi.LaunchEditorApplicationL(iStatus, newid, EFalse);
			iState = EStateEditing;
			}
			break;
		case EStateEditing:
			{
			iState = EStateFinished;
			TRequestStatus *sP = &iObserverRequestStatus;
			User::RequestComplete(sP,iStatus.Int());
			}
			break;
		default:
			break;
		}
	}

//
//	CTextMtmExportToFileDialog: export to file dialog
//

CTextMtmExportToFileDialog::CTextMtmExportToFileDialog(TDes* aFileName, const TDesC* aTitle)
	:	CEikFileSaveAsDialog(aFileName, aTitle, NULL, EFalse),
		iTitleBuf(*aTitle) // iTitle is private in CEikFileSaveAsDialog
	{
	}

void CTextMtmExportToFileDialog::PreLayoutDynInitL()
	{
	CEikFileSaveAsDialog::PreLayoutDynInitL();
	SetTitleL(iTitleBuf);
	}

//
//	CTextMtmEditServiceDialog: service setup dialog
//

CTextMtmEditServiceDialog::CTextMtmEditServiceDialog(TMTMTxtSettings& aTxtSettings)
	: iTxtSettings(aTxtSettings)
	{
	}

void CTextMtmEditServiceDialog::PreLayoutDynInitL()
	{
	TParse parse;
	_LIT(KCRoot,"C:\\");
	TFileName defaultName(KCRoot);
	parse.Set(iTxtSettings.RootFolder(),&defaultName,&defaultName);
	CEikFolderNameSelector* fsel = (CEikFolderNameSelector*)Control(ETxtEditServiceFolder);
	TFileName fullName = parse.FullName();
	fsel->SetFullNameL(fullName);	
	}

TBool CTextMtmEditServiceDialog::OkToExitL(TInt /*aButtonId*/)
	{
	CEikFolderNameSelector* fsel = (CEikFolderNameSelector*)Control(ETxtEditServiceFolder);
	iTxtSettings.SetRootFolder(fsel->FullName());
	return ETrue;
	}

