// TXTMBOX.CPP
//
// Copyright (c) 1999 Symbian Ltd.  All rights reserved.
//

#if !defined(__TXTSPAN_H__)
#include "TXTSPAN.H"
#endif

#if !defined(__MSVUIDS_H__)
#include <MSVUIDS.H>
#endif

#if !defined(__MSVIDS_H__)
#include <MSVIDS.H>
#endif

#include "txtmbox.h"

//
//	CTxtRefreshMBox: refresher class to synchronise real file system and service
//

CTxtRefreshMBox* CTxtRefreshMBox::NewL(RFs& aFs, TFileName& aRelativePath, TMsvId aCurrentRootEntryId, 
									   CMsvServerEntry *aEntry, TMsvId aServiceEntryId,
									   const TMTMTxtSettings& aTxtSettings)
//
//  1. sort files and entries by description.
//
	{
	CTxtRefreshMBox* self = new (ELeave)CTxtRefreshMBox(aFs, aRelativePath, 
		aCurrentRootEntryId, aEntry, aServiceEntryId, aTxtSettings);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}	
	
void CTxtRefreshMBox::ConstructL()
	{
	// Find absolute path of folder to refresh
	TFileName fullPath = iTxtSettings.RootFolder();		
	fullPath.Append(iRelativePath);
	
	// Get directory list
	User::LeaveIfError(iFs.GetDir(fullPath, KEntryAttDir | KEntryAttNormal,ESortByName 
		| EDirsAnyOrder | EAscending, iFilelist));

	// Get sorted list of current entries
	iExistingEntries = new (ELeave) CMsvEntrySelection;
	TMsvSelectionOrdering order(KMsvNoGrouping, EMsvSortByDescription);    
    User::LeaveIfError(iEntry->SetEntry(iCurrentRootEntryId));
	iEntry->SetSort( order );
	iEntry->GetChildren(*iExistingEntries);

	iCurrentFile  = 0;
	iCurrentEntry = 0;
	}

CTxtRefreshMBox::~CTxtRefreshMBox()
	{
	delete iExistingEntries;
	delete iFilelist;
	}

void CTxtRefreshMBox::DeleteEntryL()
// Delete current entry
	{
	__ASSERT_DEBUG(iCurrentEntry < iExistingEntries->Count(), gPanic(ETxtsInvalidEntryIndex));

	User::LeaveIfError(iEntry->SetEntry(iCurrentRootEntryId));
	iEntry->DeleteEntry((*iExistingEntries)[iCurrentEntry]); // deletes recursively
	}

TInt CTxtRefreshMBox::CreateChild(const TDesC& aDescription, const TDesC& aDetails, TUid aMessageType, 
								  const TTime& aDate, const TInt aSize)
//
// Create a child. return its Id.
//
	{
	TMsvEntry newChildEntry;
	newChildEntry.iType= aMessageType;
	newChildEntry.iMtm = KUidMsgTypeText;
	newChildEntry.iDescription.Set(aDescription);
	newChildEntry.iDetails.Set(aDetails);		   
	newChildEntry.iServiceId = iServiceEntryId;
	newChildEntry.iSize = aSize;
	newChildEntry.iDate=aDate;
	iEntry->CreateEntry(newChildEntry);
	return newChildEntry.Id();
	}

TMsvId CTxtRefreshMBox::InsertFileL()
// Insert file in entries list
//
// Return the new id if this is a folder entry, or KMsvNullIndexEntryId if it isn't
//
	{
	__ASSERT_DEBUG(iCurrentFile < iFilelist->Count(), gPanic(ETxtsInvalidEntryIndex));
	User::LeaveIfError(iEntry->SetEntry( iCurrentRootEntryId ));
	TEntry fileEntry = (*iFilelist)[iCurrentFile];
	TTime date;
	TInt size;
	if (fileEntry.IsDir())
		{
		date.HomeTime();
		size=0;
		return CreateChild(fileEntry.iName, fileEntry.iName, KUidMsvFolderEntry, date, size);
		}
	else
		{
		TFileName filename = iTxtSettings.RootFolder();
		filename.Append(iRelativePath);
		filename.Append(fileEntry.iName);
		RFile file;
		User::LeaveIfError(file.Open(iFs,filename,EFileRead));

		file.Modified(date);
		file.Size(size);
		file.Close();
		CreateChild(fileEntry.iName, iRelativePath, KUidMsvMessageEntry, date, size);

		return KMsvNullIndexEntryId;
		}
	}

TBool CTxtRefreshMBox::DoStepL()
// Main method
//
//  2. walk over the list: 
//  3.   if file name smaller than that in current pos in list:
//  4.     if file doesn't exist in list, insert it in list, and move to next file.
//  5.   else if file equals description
//  6.     skip both
//  7.   else delete TMsvEntry on the other side, and go to next in list.
//  =
//  a. at the end of either list: if the file list was finished, delete all in the entry list from 
//     current position. if entry list was finished, insert all files starting at current.
//  b. when skipping a entry, and the entry is a folder, do the folder also.
//
	{
	// If both file list and entry list are done, task is done.
    TInt nrFiles = iFilelist->Count();
    TInt existingEntries = iExistingEntries->Count();
    if (iCurrentFile  == nrFiles && iCurrentEntry == existingEntries)
		return ETrue;

	TMsvId folderId = KMsvNullIndexEntryId;

	// End of the file list: delete all entries following.
	if (iCurrentFile == nrFiles)
		{
		// Delete current entry.
		DeleteEntryL();
		// Step to next entry.
		iCurrentEntry++;
		}
	// End of entries list. Append current file to end of list.
	else if (iCurrentEntry == existingEntries)
		{
		// Insert file in entries list.
		// if file is folder, do the folder recursively
		folderId = InsertFileL();
		// step to next file
		iCurrentFile++;
		}
	else
		{
		// Continue walking
		User::LeaveIfError(iEntry->SetEntry( (*iExistingEntries)[iCurrentEntry] ));

		TInt compare = (*iFilelist)[iCurrentFile].iName.CompareF(iEntry->Entry().iDescription);

		// If current file name smaller than name of current entry, then the file doesn't yet
		// exist in the list, and needs to be added.
		if ( compare < 0 )
			{
			// Insert file in entries list.
			// if file is folder, do the folder recursively
			folderId = InsertFileL();
			// step to next file
			iCurrentFile++;
			}
		// Files are equal. Both should be the same.
		else if (compare == 0)
			{
			// if file is folder, do the folder recursively
			if (iEntry->Entry().iType == KUidMsvFolderEntry)
				folderId = (*iExistingEntries)[iCurrentEntry];
			// Skip both file and entry
			iCurrentFile++;
			iCurrentEntry++;
			}
		// File name is greater than name of current entry, so the current entry 
		// shouldn't be there.
		else
			{
			// Delete entry.
			DeleteEntryL();
			// Step to next entry.
			iCurrentEntry++;
			}
		}

	// If just passing folder, do the folder recursively.
	if (folderId != KMsvNullIndexEntryId)
		{
		User::LeaveIfError(iEntry->SetEntry(folderId));
		
		// Set absolute name
		TFileName subDir = iRelativePath;
		subDir.Append(iEntry->Entry().iDescription);
		subDir.Append(KPathDelimiter);

		CTxtRefreshMBox *folderSynchroniser = CTxtRefreshMBox::NewL(iFs, subDir, folderId, iEntry, iServiceEntryId, iTxtSettings);
		CleanupStack::PushL(folderSynchroniser);
		while (!folderSynchroniser->DoStepL()) ;
		CleanupStack::PopAndDestroy(); //folderSynchroniser
		}
	
	return EFalse;
	}

