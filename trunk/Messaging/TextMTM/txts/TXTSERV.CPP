// TXTSERV.CPP
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#if !defined(__MSVENTRY_H__)
#include <msventry.h>
#endif

#if !defined(__TXTRICH_H__)
#include <txtrich.h>
#endif

#if !defined(_TXTCMDS_H_)
#include "txtcmds.h"
#endif
#if !defined(__TXUT_H__)
#include "txut.h"    // Current service settings
#endif

#include "txtserv.h"
#include "txtscpmv.h"
#include "txtmbox.h"

//
//  Factory function, constructor, initialisation, and destructor functions
//

EXPORT_C CTextServerMtm* CTextServerMtm::NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aInitialEntry)
// Exported factory function
	{
	CleanupStack::PushL(aInitialEntry);
	CTextServerMtm* self=new (ELeave) CTextServerMtm(aRegisteredMtmDll, aInitialEntry);
	CleanupStack::Pop();

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTextServerMtm::CTextServerMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aInitialEntry)
	: 	CBaseServerMtm(aRegisteredMtmDll, aInitialEntry), 
		iCurrentOperation(ETxtOpIdle), 
		iServiceEntryId(aInitialEntry->Entry().Id())
	{
	}
		
void CTextServerMtm::ConstructL()
	{
	iReportStatus=NULL;
	CActiveScheduler::Add(this);
	User::LeaveIfError(iFs.Connect());
	}

CTextServerMtm::~CTextServerMtm()
	{
	Cancel();
	delete iTxtCopyMove;
	delete iTxtSettings;
	}

//
// Copy and move functions
//

void CTextServerMtm::CopyToLocalL(const CMsvEntrySelection& aSelection, TMsvId aDestination, 
								  TRequestStatus& aStatus)
// Get remote entries (by copying)
	{
	ConnectToServiceL();
	CTxtActiveOper* activeOperation = new (ELeave) CTxtCopyToLocalOp(iFs,*iServerEntry);
	DoOperationL(activeOperation, aSelection,aDestination, aStatus);
	}

void CTextServerMtm::CopyFromLocalL(const CMsvEntrySelection& aSelection, TMsvId aDestination, 
									TRequestStatus& aStatus)
// Transfer to remote (by copying)
	{
	ConnectToServiceL();
	CTxtActiveOper* activeOperation = new (ELeave) CTxtCopyFromLocalOp(iFs,*iServerEntry);
	DoOperationL(activeOperation, aSelection,aDestination, aStatus);
	}

void CTextServerMtm::CopyWithinServiceL(const CMsvEntrySelection& aSelection, TMsvId aDestination, 
										TRequestStatus& aStatus)
// Copy entries within service 
	{
	ConnectToServiceL();
	CTxtActiveOper* activeOperation = new (ELeave) CTxtCopyWithinServiceOp(iFs,*iServerEntry);
	DoOperationL(activeOperation, aSelection,aDestination, aStatus);
	}

void CTextServerMtm::MoveToLocalL(const CMsvEntrySelection& aSelection, TMsvId aDestination, 
								  TRequestStatus& aStatus)
// Get remote entries (by moving)
	{
	ConnectToServiceL();
	CTxtActiveOper* activeOperation = new (ELeave) CTxtMoveToLocalOp(iFs, *iServerEntry);
	DoOperationL(activeOperation, aSelection,aDestination, aStatus);
	}

void CTextServerMtm::MoveFromLocalL(const CMsvEntrySelection& aSelection, TMsvId aDestination, 
									TRequestStatus& aStatus)
// Transfer to remote (by moving)
	{
	ConnectToServiceL();
	CTxtActiveOper* activeOperation = new (ELeave) CTxtMoveFromLocalOp(iFs, *iServerEntry);
	DoOperationL(activeOperation, aSelection,aDestination, aStatus);
	}

void CTextServerMtm::MoveWithinServiceL(const CMsvEntrySelection& aSelection, TMsvId aDestination, 
										TRequestStatus& aStatus)
// Move entries within service 
	{
	ConnectToServiceL();
	CTxtActiveOper* activeOperation = new (ELeave) CTxtMoveWithinServiceOp(iFs,*iServerEntry);
	DoOperationL(activeOperation, aSelection,aDestination, aStatus);
	}

//
// Create, change, delete functions
//

void CTextServerMtm::DeleteAllL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
// Delete entries recursively 
	{
	ConnectToServiceL();
	CTxtActiveOper* activeOperation = new (ELeave) CTxtDeleteOp(iFs, *iServerEntry);
	DoOperationL(activeOperation, aSelection,KMsvNullIndexEntryId, aStatus);
	}

void CTextServerMtm::CreateL(TMsvEntry /*aNewEntry*/, TRequestStatus& aStatus)
// Remote creation (other than by copying) not supported 
//
	{
	TRequestStatus *sP = &aStatus;
	User::RequestComplete(sP, KErrNotSupported);
	}

void CTextServerMtm::ChangeL(TMsvEntry /*aNewEntry*/, TRequestStatus& aStatus)
// Change entry 
//
// This function could change file names in the file system. It would then also have
// to change the details field of all the children of a changed folder name.
// Not supported for now.
//
	{
	TRequestStatus *sP = &aStatus;
	User::RequestComplete(sP, KErrNotSupported);
	}

void CTextServerMtm::ConnectToServiceL()
// Retrieves service settings before doing a "remote" operation
	{
	iServerEntry->SetEntry(iServiceEntryId);
	__ASSERT_DEBUG(iServerEntry->Entry().iType == KUidMsvServiceEntry, gPanic(ETxtsInvalidService));
	// Retrieve settings
	if (iTxtSettings == NULL) iTxtSettings = new (ELeave) TMTMTxtSettings;
	CMTMTxtSettings* settings = CMTMTxtSettings::NewL();
	CleanupStack::PushL(settings);
	settings->LoadSettingsL(iServerEntry->Entry().Id(), *iTxtSettings);
	CleanupStack::PopAndDestroy(); // settings	
	}

void CTextServerMtm::DoOperationL(CTxtActiveOper* aActiveOperation, const CMsvEntrySelection& aSelection,
								  TMsvId aDestination, TRequestStatus& aStatus)
// Do any of the copy/move from or to local, or delete operations, defined by aActiveOperation
	{
	CleanupStack::PushL(aActiveOperation);

	__ASSERT_DEBUG(aActiveOperation,gPanic(ETxtsInvalidNullOperation));
	AssertIdle();
	AssertConnected();

	iReportStatus=&aStatus;
	iCurrentOperation=ETxtOpCopyMoveDel;

	if (aDestination != KMsvNullIndexEntryId)
		{
		// Get folder name for destination
		iServerEntry->SetEntry(aDestination);
		TUid type = iServerEntry->Entry().iType;
		if (type == KUidMsvServiceEntry)
			{
			iParse.Set(KNullDesC,NULL,NULL);
			}
		else if (type == KUidMsvFolderEntry)
			{
			iParse.Set(iServerEntry->Entry().iDetails,NULL,NULL);
			iParse.AddDir(iServerEntry->Entry().iDescription);
			}
		else
			{
			gPanic(ETxtsInvalidDestination);
			}
		}

	CleanupStack::Pop(); //aActiveOperation now owned by iTxtCopyMove
	iTxtCopyMove=CTxtCopyMoveBase::NewL(aActiveOperation,aSelection, *iServerEntry, iServiceEntryId, iParse);
	aStatus=iStatus = KRequestPending;
	iTxtCopyMove->Start(iStatus);
	SetActive();
	}

//
// Command and progress functions
//

void CTextServerMtm::StartCommandL(CMsvEntrySelection& /*aSelection*/, TInt aCommand, 
								   const TDesC8& /*aParameter*/, TRequestStatus& aStatus)
// Run MTM-specific command on selection of entries 
// Only command supported is Refresh
	{
	AssertIdle();

	switch (aCommand)
		{
		case KTXTMTMRefresh:
			{
			ConnectToServiceL();
			DoRefreshL();
			TRequestStatus* sP=&aStatus;
			User::RequestComplete(sP,KErrNone);
			}
			break;
		default:
			TRequestStatus* sP=&aStatus;
			User::RequestComplete(sP, KErrNotSupported);
			break;
		}
	}

TBool CTextServerMtm::CommandExpected()
// Prevent object deletion on command completion
// Allow unloading
	{
	return EFalse;
	}

const TDesC8& CTextServerMtm::Progress()
// Pass-back progress information
	{
	switch (iCurrentOperation)
		{
		case ETxtOpCopyMoveDel:
			iProgressBuf() = iTxtCopyMove->Progress();
			break;
		default:
			break;
		}
	return iProgressBuf;
	}

//
//  Active object completion functions
//

void CTextServerMtm::DoCancel()
// Cancel current operation
	{
	// The only operations that are actually asynchronous are the
	// copy, move, delete. Others complete synchronously.
	switch (iCurrentOperation)
		{
		case ETxtOpCopyMoveDel:
			{			
			delete iTxtCopyMove;
			iTxtCopyMove=NULL;
			}
			break;
		default:
			AssertIdle();
			break;
		}
	iCurrentOperation=ETxtOpIdle;
	TRequestStatus* pS=&iStatus;
	User::RequestComplete(pS, KErrCancel);
	}

void CTextServerMtm::DoComplete(TInt aError)
// Active object complete leave handler
//
// Never expect this to be called
// as CTextServerMtm::DoRunL() does not leave
	{
	User::RequestComplete(iReportStatus,aError);
	iReportStatus=NULL;
	}

void CTextServerMtm::DoRunL()
// Active object completion
//
// Run is used in this object to clean up after operations have finished.
//
	{
	__ASSERT_DEBUG(iReportStatus != NULL,gPanic(ETxtsStrayRequest));
	// Pass on an error code if there was one.

	// Shouldn't have any errors in iStatus, but put them in Progress
	// anyway for safety
	
	if (iStatus!=KErrNone) 
		iProgressBuf().iErrorCode=iStatus.Int();

	// Determine the kind of clean up to do after the active process has stopped
	switch (iCurrentOperation)
		{
		case ETxtOpCopyMoveDel:
			{			
			iProgressBuf() = iTxtCopyMove->Progress();
			delete iTxtCopyMove;
			iTxtCopyMove=NULL;
			}
			break;
		default:
			break;
		}
	iCurrentOperation=ETxtOpIdle;
	// Always return KErrNone; errors are returned in Progress
	User::RequestComplete(iReportStatus,KErrNone);
	iReportStatus=NULL;
	}


//
// Methods needed to refresh the service
//

void CTextServerMtm::DoRefreshL()
// Perform refresh
	{
	AssertConnected();
	TFileName relativePath(KNullDesC);

	CTxtRefreshMBox *folderSynchroniser = CTxtRefreshMBox::NewL(iFs, relativePath, 
		iServiceEntryId, iServerEntry, iServiceEntryId, *iTxtSettings);
	CleanupStack::PushL(folderSynchroniser);
	while (!folderSynchroniser->DoStepL()) ;
	CleanupStack::PopAndDestroy(); //folderSynchroniser
	}

