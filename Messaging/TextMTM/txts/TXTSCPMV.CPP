// TXTSCPMV.CPP
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__TXTETEXT_H__)
#include <txtetext.h>
#endif

#if !defined(__TXTRICH_H__)
#include <txtrich.h>
#endif

#if !defined(__APPARC_H__)
#include <apparc.h>
#endif

#if !defined(__MSVUIDS_H__)
#include <MSVUIDS.H>
#endif

#if !defined(__MSVIDS_H__)
#include <MSVIDS.H>
#endif

#include "txtserv.h"
#include "txtscpmv.h" 
#include "txtspan.h" 


//
// CTxtActiveOper: base class for move, copy and delete operations 
//

CTxtActiveOper::CTxtActiveOper(RFs& aFs, CMsvServerEntry& aServerEntry)
	: CActive(0), iServerEntry(aServerEntry), iFs(aFs)
	{ 
	CActiveScheduler::Add(this); 
	iFs.Connect(); 
	}

CTxtActiveOper::~CTxtActiveOper()				
	{ 
	Cancel(); 
	}

void CTxtActiveOper::Start(TMsvId& aSourceId, TMsvId& aDestId, TRequestStatus& aStatus)
//
// Start asynchronous transfer.
//
	{ 
	iSourceId = aSourceId;
	iDestId   = aDestId;
	
	iStatus = KRequestPending;
	iReportStatus = &aStatus; 
	SetActive(); 
	aStatus = KRequestPending; 
	TRequestStatus *sP=&iStatus;
	User::RequestComplete(sP, KErrNone); 
	}

void CTxtActiveOper::DoCancel() 
	{ 
	}


void CTxtActiveOper::RunL()	
//
// Default operation: do nothing
//
	{ 
	TRAPD(errorCode, DoRunL());
	// Within this class, a leave is an external (service) error
	// This basically means the DoRunL implementation is not allowed to leave with an error 
	// after calling User::RequestComplete
	if (errorCode)
		{
		iCurrentCaller->Progress().iErrorCode = errorCode;
		User::RequestComplete(iReportStatus, KErrNone);
		}
	}

//
// CTxtCopyMoveBase: walks over all the items in aSource, and starts aActiveOperation
// for each one of them, waiting for completion.
//

CTxtCopyMoveBase* CTxtCopyMoveBase::NewL(CTxtActiveOper* aActiveOperation, const CMsvEntrySelection& aSource, 
									 CMsvServerEntry& aDestination, TMsvId& aServiceEntryId, TParse& aParse)
	{
	CTxtCopyMoveBase* self = new (ELeave) CTxtCopyMoveBase(aActiveOperation,aSource,
		aDestination, aServiceEntryId, aParse);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTxtCopyMoveBase* CTxtCopyMoveBase::CopyConstructL(const CMsvEntrySelection& aSource)
	{
	CTxtActiveOper* newActive = iActiveOperation->CopyConstructL();
	return NewL(newActive, aSource, iDestination, iServiceEntryId, iServiceDestinationpath);
	}

CTxtCopyMoveBase::CTxtCopyMoveBase(CTxtActiveOper* aActiveOperation, const CMsvEntrySelection& aSource, 
								   CMsvServerEntry& aDestination, TMsvId& aServiceEntryId, TParse& aParse)
	: CActive( KMsgTxtRefreshMailboxPriority ),
	iActiveOperation(aActiveOperation), iSource(aSource), iDestination(aDestination), 
	iServiceEntryId(aServiceEntryId), iServiceDestinationpath(aParse)
	{
	}

void CTxtCopyMoveBase::ConstructL()
	{
	iDestId = iDestination.Entry().Id();
	iActiveOperation->SetCurrentCaller(this);
	if (iActiveOperation->MoveIsToService())	
		{
		__ASSERT_ALWAYS(iDestination.Entry().iServiceId == iServiceEntryId, gPanic(ETxtsInvalidService));
		}
	CActiveScheduler::Add(this);	  // Add CTxtCopyMoveBase to scheduler's queue
	}

CTxtCopyMoveBase::~CTxtCopyMoveBase()
	{
	Cancel();
	delete iActiveOperation; 
	iActiveOperation = NULL;
	}

void CTxtCopyMoveBase::DoCancel()
//
// Cancel any current operation
//
	{
	delete iActiveOperation; 
	iActiveOperation = NULL;
	iTxtCopyMoveState =	ETxtFinished;
	}

void CTxtCopyMoveBase::Start(TRequestStatus& aStatus)
//
// Start me up
//
	{
	iReportStatus = &aStatus;

	iMsgCounter = -1;
	// set up progress object
	iProgress.iTotalMsgs=iSource.Count();
	iProgress.iMsgsProcessed=0;
	iProgress.iErrorCode=KErrNone; 

	if(iSource.Count())
		{
		iTxtCopyMoveState=ETxtRetrievingMessage;
		}
	else
		{
		iTxtCopyMoveState=ETxtFinished;
		}
	aStatus = KRequestPending; 
	SetActive(); 
	TRequestStatus *sP=&iStatus;
	User::RequestComplete(sP, KErrNone); 
	}


void CTxtCopyMoveBase::RunL()
	{
	TRAPD(errorCode, DoRunL());
	// In this class a leave is an internal error
	// No leave is allowed after calling User::RequestComplete in DoRunL because 
	// of this implementation
	if (errorCode)
		{
		User::RequestComplete(iReportStatus, errorCode); 
		}
	}


void CTxtCopyMoveBase::CheckDeleteSource()
// Delete previous source message if necessary
	{
	if (iMsgCounter>=0)
		{
		if (iActiveOperation->DeleteSourceAfterwards())	
			{
			//delete the previous source
			iDestination.SetEntry(iSource[iMsgCounter]);
			TMsvId parent = iDestination.Entry().Parent();
			iDestination.SetEntry(parent);
			iDestination.DeleteEntry(iSource[iMsgCounter]);
			}
		}
	}


void CTxtCopyMoveBase::SetupNewEntryL(TMsvId& aSourceMsgId, TMsvId& aNewDestMsvId)
// Create new entry index details based on source
	{
	// Get the destination path, and put it in the details 
	// section of the message
	iDestination.SetEntry(aSourceMsgId);
	TMsvEntry entry;
	entry.iServiceId       = iDestination.Entry().iServiceId;
	entry.iRelatedId       = iDestination.Entry().iRelatedId;
	entry.iType            = iDestination.Entry().iType;
	entry.iMtm             = iDestination.Entry().iMtm;
	entry.iDate            = iDestination.Entry().iDate;
	entry.iSize            = iDestination.Entry().iSize;
	entry.iError           = iDestination.Entry().iError;
//	entry.iWdpPortNumber   = iDestination.Entry().iWdpPortNumber;
	entry.iMtmData1        = iDestination.Entry().iMtmData1;
	entry.iMtmData2        = iDestination.Entry().iMtmData2;
	entry.iMtmData3        = iDestination.Entry().iMtmData3;
	// Copy the details and description for protection
	TFileName name=iDestination.Entry().iDescription;
	TFileName path=iDestination.Entry().iDetails;
	entry.iDescription.Set(name);
	entry.iDetails.Set(path);

	// If the move is to the service, make sure the entry is valid
	if (iActiveOperation->MoveIsToService())	
		{
		__ASSERT_DEBUG(name.Length()>0,gPanic(ETxtsInvalidFileName));
		__ASSERT_DEBUG(name.Locate(KPathDelimiter)<0,gPanic(ETxtsInvalidFileName));
		
		if (name.Length()==0)
			User::Leave(KErrArgument);
		if (name.Locate(KPathDelimiter)>=0)
			User::Leave(KErrArgument);
		
		entry.iDetails.Set(iServiceDestinationpath.FullName());
		entry.iServiceId = iServiceEntryId;
		}
	else
		{
		entry.iServiceId = KMsvLocalServiceIndexEntryId;
		}

	// Create the new entry
	iDestination.SetEntry(iDestId);
	iDestination.CreateEntry(entry);
	
	aNewDestMsvId=entry.Id();
	iProgress.iNewId = aNewDestMsvId;
	}

void CTxtCopyMoveBase::DoRunL()
    {
	// Check if error occurred on last stage
	User::LeaveIfError(iStatus.Int());		// Leave if internal error has occurred. 
	if (iProgress.iErrorCode != KErrNone)	// External error occurred
		{
		User::RequestComplete(iReportStatus,KErrNone);
		return;
		}

	// Delete previous source message if necessary
	CheckDeleteSource();

	switch (iTxtCopyMoveState)
		{
		case ETxtRetrievingMessage:
		// Operate on the next message
			{
			iMsgCounter++;

			// For copy/move ops, make a new entry on the other side.
			TMsvId newSourceMsgId=iSource[iMsgCounter];
			TMsvId newDestMsvId=KMsvNullIndexEntryId;
			if (iActiveOperation->CopiedHeader()) SetupNewEntryL(newSourceMsgId,newDestMsvId);
			
			iProgress.iMsgsProcessed++;	// Update progress info
			// Check if finished
			if(iMsgCounter == iSource.Count()-1)
				{
				iTxtCopyMoveState = ETxtFinished;
				}
			// Do the operation-specific part
			iActiveOperation->Start(newSourceMsgId,newDestMsvId,iStatus);
			SetActive();
			}
			break;
		case ETxtFinished:
		// Have now finished this operation, so pass the completion up
			__ASSERT_DEBUG(iReportStatus != NULL, gPanic(ETxtsStrayRequest));
			User::RequestComplete(iReportStatus,KErrNone);
			break;
		}
	}


TTxtProgress& CTxtCopyMoveBase::Progress()
//
// Report the refreshing news back to the UI
//
	{
	return iProgress;
	}


//
// Implementation of the several body-moving classes
//

void CTxtCopyToLocalOp::DoRunL()
//
// Copy one remote entry to local, getting the source text and storing
// it in the associated CMsvStore
//
	{
	User::LeaveIfError(iStatus.Int());

	User::LeaveIfError(iServerEntry.SetEntry(iSourceId));
	TMsvEntry entry = iServerEntry.Entry();

	// Build a new source file file name
	User::LeaveIfError(iServerEntry.SetEntry( iDestId ));	
	TxtUtils::GetEntryFileNameL(iFileName, entry);		
	
	DoMessageCopyToLocalL();
	}

void CTxtCopyToLocalOp::DoMessageCopyToLocalL()
// Copy remote message to local: i.e. read file contents into message store
	{
	// Description of the body is the original file
	CMsvStore* wstore=NULL;
	wstore=iServerEntry.EditStoreL();
	CleanupStack::PushL(wstore);
	User::LeaveIfNull(wstore);

	// Read file text into rich text object
	CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraLayer);
	CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charLayer);
	CRichText* body = CRichText::NewL(paraLayer,charLayer, CEditableText::EFlatStorage, 70 );
	CleanupStack::PushL(body);
	body->ImportTextFileL(0,iFileName,CPlainText::EOrganiseByLine);

	// Store in body 
	wstore->StoreBodyTextL(*body);
	wstore->CommitL();

	// Clean up
	CleanupStack::PopAndDestroy(4); //body, charLayer, paraLayer, wstore
	TInt err=KErrNone;
	if (DeleteSourceAfterwards())
		err=iFs.Delete(iFileName);
	User::RequestComplete(iReportStatus, err);
	}


void CTxtCopyFromLocalOp::DoRunL()
//
// Copy entries from local to remote, storing the plain text contents of
// the associated rich text in a newly created file.
//
	{
	// Error on previous stages
	User::LeaveIfError(iStatus.Int());

	// Get details of destination entry
	User::LeaveIfError(iServerEntry.SetEntry( iDestId ));
	TMsvEntry entry = iServerEntry.Entry();

	// Find file from entry details
	TxtUtils::GetEntryFileNameL(iFileName, entry);		

	// Check if file already exists.
	RFile file;
	if (file.Open(iFs,iFileName,EFileRead) == KErrNone)
		{
		file.Close();
		// If so, create a new unique filename and update entry
		User::LeaveIfError(CApaApplication::GenerateFileName(iFs,iFileName));	
		TParse newName;
		newName.Set(iFileName,NULL,NULL);
		entry.iDescription.Set(newName.NameAndExt());		
		User::LeaveIfError(iServerEntry.ChangeEntry(entry));
		}

	// Set the context back
	User::LeaveIfError(iServerEntry.SetEntry( iSourceId ));

	DoMessageCopyFromLocalL(entry);
	}


void CTxtCopyFromLocalOp::DoMessageCopyFromLocalL(const TMsvEntry& aEntry)
// Copy local message to remote service: i.e. create a file
	{
	CMsvStore* rstore=iServerEntry.ReadStoreL();
	CleanupStack::PushL(rstore);

	if (rstore->HasBodyTextL())
		{
		// Get message text and write it to file
		CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
		CleanupStack::PushL(paraLayer);
		CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
		CleanupStack::PushL(charLayer);
		CRichText* body = CRichText::NewL(paraLayer,charLayer, CEditableText::EFlatStorage, 70);
		CleanupStack::PushL(body);
		rstore->RestoreBodyTextL(*body);
		//User::LeaveIfError(iFs.MkDir(iFileName)); 
		body->ExportAsTextL(iFileName,CPlainText::EOrganiseByLine,70);

		// Change date modified to reflect what's in the TMsvEntry
		RFile outFile;
		if (outFile.Open(iFs, iFileName, EFileWrite) == KErrNone)
			{
			outFile.SetModified(aEntry.iDate);
			outFile.Close();
			}
		CleanupStack::PopAndDestroy(3); //paraLayer, charLayer, body
		}
	
	CleanupStack::PopAndDestroy(); //rstore

	User::RequestComplete(iReportStatus, KErrNone);
	}
	

void CTxtCopyWithinServiceOp::DoRunL()
//
// Copy files over within the service.
//
	{
	TInt err=KErrNone;
	User::LeaveIfError(iStatus.Int());

	User::LeaveIfError(iServerEntry.SetEntry(iSourceId));
	TMsvEntry sourceEntry = iServerEntry.Entry();

	// Source settings
	TFileName sourceFileName;
	TxtUtils::GetEntryFileNameL(sourceFileName, sourceEntry);		
	iFileName = sourceFileName;

	// Destination settings
	err = iServerEntry.SetEntry(iDestId);
	if (err)
		{
		User::RequestComplete(iReportStatus, err);
		return;
		}

	DoMessageCopyWithinServiceL();
	}


void CTxtCopyWithinServiceOp::DoMessageCopyWithinServiceL()
// Copy remote message to local: i.e. read file contents into message store
	{ 
	TMsvEntry destEntry = iServerEntry.Entry();
	TFileName destFileName;
	TxtUtils::GetEntryFileNameL(destFileName, destEntry);		

	iFs.MkDir(destFileName);

	RFile outFile;
	// Test if destination file already exists
	if (outFile.Open(iFs, destFileName, EFileRead) == KErrNone)
		{
		outFile.Close();
		User::RequestComplete(iReportStatus, KErrAlreadyExists);
		return;
		}
	// Create destination file
	TInt err = outFile.Create(iFs, destFileName, EFileWrite);
	if (err)
		{
		User::RequestComplete(iReportStatus, err);
		return;
		}
	// Open input file
	RFile inFile;
	err = inFile.Open(iFs, iFileName, EFileRead);
	if (err)
		{
		inFile.Close();
		User::RequestComplete(iReportStatus, err);
		return;
		}
	
	// Copy the file over
	TFileText inTextFile;
	TFileText outTextFile;
	inTextFile.Set(inFile);
	outTextFile.Set(outFile);
	TBuf<201> buf;
	TInt len = 200;
	while (len>0)
		{
		buf.FillZ();
		inTextFile.Read(buf);
		len=buf.Length();
		outTextFile.Write(buf);
		}
	// Change date modified to reflect what's in the TMsvEntry
	outFile.SetModified(destEntry.iDate);

	// Clean up
	inFile. Close();
	outFile.Close();
	if (DeleteSourceAfterwards())
		err=iFs.Delete(iFileName);
	User::RequestComplete(iReportStatus, KErrNone);
	}


void CTxtDeleteOp::DoRunL()	
//
// Deleting files.
//
	{ 
	TInt err=KErrNone;
	User::LeaveIfError(iStatus.Int());
	//
	User::LeaveIfError(iServerEntry.SetEntry(iSourceId));
	TMsvEntry entry = iServerEntry.Entry();
	TxtUtils::GetEntryFileNameL(iFileName, entry);		
		
	err=iFs.Delete(iFileName);
	User::RequestComplete(iReportStatus, err);	
	}

//
// These CopyConstructL methods allow other classes to get an exact copy
// of the class derived from CTxtActiveOper. Needed to recursively perform an 
// operation on a set of entries.
//

// CTxtCopyToLocalOp

CTxtActiveOper* CTxtCopyToLocalOp::CopyConstructL()
	{
	return new (ELeave) CTxtCopyToLocalOp(iFs, iServerEntry);
	}

TBool CTxtCopyToLocalOp::CopiedHeader() const    
	{
	return ETrue; 
	}
TBool CTxtCopyToLocalOp::DeleteSourceAfterwards() const    
	{
	return EFalse; 
	}

TBool CTxtCopyToLocalOp::MoveIsToService() const
	{
	return EFalse; 
	}

// CTxtCopyFromLocalOp

CTxtActiveOper* CTxtCopyFromLocalOp::CopyConstructL()
	{
	return new (ELeave) CTxtCopyFromLocalOp(iFs, iServerEntry);
	}

TBool CTxtCopyFromLocalOp::CopiedHeader() const    
	{
	return ETrue; 
	}

TBool CTxtCopyFromLocalOp::DeleteSourceAfterwards() const    
	{
	return EFalse; 
	}

TBool CTxtCopyFromLocalOp::MoveIsToService() const 
	{
	return ETrue; 
	}

// CTxtCopyWithinServiceOp

CTxtActiveOper* CTxtCopyWithinServiceOp::CopyConstructL()
	{
	return new (ELeave) CTxtCopyWithinServiceOp(iFs, iServerEntry);
	}

TBool CTxtCopyWithinServiceOp::CopiedHeader() const    
	{
	return ETrue; 
	}

TBool CTxtCopyWithinServiceOp::DeleteSourceAfterwards() const    
	{
	return EFalse; 
	}

TBool CTxtCopyWithinServiceOp::MoveIsToService() const 
	{
	return ETrue; 
	}

// CTxtMoveToLocalOp

CTxtActiveOper* CTxtMoveToLocalOp::CopyConstructL()
	{
	return new (ELeave) CTxtMoveToLocalOp(iFs, iServerEntry);
	}

TBool CTxtMoveToLocalOp::DeleteSourceAfterwards() const    
	{
	return ETrue; 
	}

// CTxtMoveFromLocalOp

CTxtActiveOper* CTxtMoveFromLocalOp::CopyConstructL()
	{
	return new (ELeave) CTxtMoveFromLocalOp(iFs, iServerEntry);
	}

TBool CTxtMoveFromLocalOp::DeleteSourceAfterwards() const 
	{
	return ETrue; 
	}

// CTxtMoveWithinServiceOp

CTxtActiveOper* CTxtMoveWithinServiceOp::CopyConstructL()
	{
	return new (ELeave) CTxtMoveWithinServiceOp(iFs, iServerEntry);
	}

TBool CTxtMoveWithinServiceOp::DeleteSourceAfterwards() const    
	{
	return ETrue; 
	}

// CTxtDeleteOp

CTxtActiveOper* CTxtDeleteOp::CopyConstructL()
	{
	return new (ELeave) CTxtDeleteOp(iFs, iServerEntry);
	}

TBool CTxtDeleteOp::CopiedHeader() const    
	{
	return EFalse; 
	}

TBool CTxtDeleteOp::DeleteSourceAfterwards() const    
	{
	return ETrue; 
	}

TBool CTxtDeleteOp::MoveIsToService() const 
	{
	return EFalse; 
	}


