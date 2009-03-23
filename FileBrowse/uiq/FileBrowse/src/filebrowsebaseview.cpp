// FileBrowseBaseView.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include <QikViewBase.h>
#include <QikListBox.h>
#include <QikListBoxModel.h>
#include <MQikListBoxData.h>
#include <QikContent.h>

#include "FileBrowseGlobals.h"
#include "FileBrowse.hrh"
#include "FileBrowseBaseView.h"
#include "RFsEngine.h"
#include <filebrowseapp.rsg>
#include <filebrowseapp.mbg>

_LIT(KMbmPath, "*");

CFileBrowseBaseView* CFileBrowseBaseView::NewL(CQikAppUi& aAppUi,CRFsEngine& aEngine)
	{
	CFileBrowseBaseView* self = CFileBrowseBaseView::NewLC(aAppUi,aEngine);
	CleanupStack::Pop(self);
	return (self);
	}

CFileBrowseBaseView* CFileBrowseBaseView::NewLC(CQikAppUi& aAppUi,CRFsEngine& aEngine)
	{
	CFileBrowseBaseView* self = new (ELeave) CFileBrowseBaseView(aAppUi,aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return (self);
	}

CFileBrowseBaseView::CFileBrowseBaseView(CQikAppUi& aAppUi,CRFsEngine& aEngine)
	: CQikViewBase(aAppUi, KNullViewId), iEngine(aEngine), iRoot(ETrue)
	{}

CFileBrowseBaseView::~CFileBrowseBaseView()
	{
	iFocusPos.Close();
	}


void CFileBrowseBaseView::ConstructL()
	{
	BaseConstructL();
	}

void CFileBrowseBaseView::ViewConstructL()
	{
	ViewConstructFromResourceL(R_FILEBROWSE_BASEVIEW_UI_CONFIGURATIONS);

	iListBox = LocateControlByUniqueHandle<CQikListBox>(EFileBrowseListbox);
	User::LeaveIfNull(iListBox);
	iListBox->SetListBoxObserver(this);
	}

void CFileBrowseBaseView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/,
                                        const TUid /*aCustomMessageId*/,
                                        const TDesC8& /*aCustomMessage*/)
	{
	// create intial content
	ShowRootL();
	}


// This adds the C and D drives
void CFileBrowseBaseView::ShowRootL()
	{
	// Get the list box model
	MQikListBoxModel& model(iListBox->Model());
	// clean list
	model.RemoveAllDataL();

	// Notify the list box model that changes will be made after this point.
	model.ModelBeginUpdateLC();
  
	// Create listBoxData items
	MQikListBoxData* listBoxData;

	TBuf<20> driveBuf;
	
	for(TInt drive = EDriveC; drive <= EDriveD; drive++)
		{
		
		listBoxData = model.NewDataL(MQikListBoxModel::EDataUnselectable);
		CleanupClosePushL(*listBoxData);
	
		driveBuf.Zero();
	
		if(drive == EDriveC)
			{
			CCoeEnv::Static()->ReadResource(driveBuf, R_FILEBROWSE_DRIVEC);
			}
		else
			{
			CCoeEnv::Static()->ReadResource(driveBuf, R_FILEBROWSE_DRIVED);
			}
    	
		listBoxData->AddTextL(driveBuf, EQikListBoxSlotText1);

		CQikContent* icon = CQikContent::NewL(this, KMbmPath,
	                                         EMbmFilebrowseappFolder,
	                                         EMbmFilebrowseappMask);
		CleanupStack::PushL(icon);
		listBoxData->AddIconL(icon, EQikListBoxSlotLeftSmallIcon1);
		CleanupStack::Pop(icon);
		icon = NULL;

		CleanupStack::PopAndDestroy(listBoxData);
		}

	// Informs that the update of the list box model has ended
	model.ModelEndUpdateL();
  
	iFilePosition = 2;
	iCurrPos = 0;
	}

void CFileBrowseBaseView::NavigateL(TInt aIndex)
	{
	if (iRoot) // Currently in the root directory, set the correct drive to open
    	{
    	if (aIndex==0)
    		{
    		_LIT(KPathC, "C:\\");
    		iPath = KPathC();
    		}
    	else if (aIndex==1)
    		{
    		_LIT(KPathD, "D:\\");
    		iPath = KPathD();
    		}
    	iRoot = EFalse;
    	}
	else
		{  
		if (aIndex == 0) // going back up the folder list
			{
			// delete last '\\'
			iPath.SetLength(iPath.Length()-1);
			// find and remove last folder name
			const TInt pos = iPath.LocateReverse('\\');
			// check if this was not the last folder
			// if error - we're at the begining, go back to entry point
			if (pos < 2 || pos == KErrNotFound)
				{
				iPath.Zero();
				iRoot = ETrue;
				}
			else 
				{
				iCurrPos = iFocusPos.operator[]( iFocusPos.Count()-1 );
				iFocusPos.Remove(iFocusPos.Count()-1);
				iPath.SetLength(pos+1);
				}
			}
		else if (aIndex < iFilePosition) // going down the folder hierarchy
			{
			iRoot = EFalse;
			MQikListBoxModel& model(iListBox->Model());
			MQikListBoxData* data = model.RetrieveDataL(aIndex);
			data->Text(EQikListBoxSlotText1).Length();

			iPath.Append(data->Text(EQikListBoxSlotText1));
			iPath.Append('\\');
			data->Close();
			iCurrPos = 0;
			iFocusPos.Append(aIndex);
			}
		else
        	{
        	CEikonEnv::Static()->InfoWinL(R_FILEBROWSE_INFONOTE);
        	iCurrPos = aIndex;
        	}
		}
	}

void CFileBrowseBaseView::OpenFolderL()
	{
	if (iRoot) // The root directory view is handled separately
		{
		ShowRootL();
		}
	else
		{
		MQikListBoxModel& model(iListBox->Model());
		// clean list
		model.RemoveAllDataL();
		// start updating
		model.ModelBeginUpdateLC();
		// declare listbox data element
		MQikListBoxData* listBoxData;

		// create first "back" element
		listBoxData = model.NewDataL(MQikListBoxModel::EDataUnselectable);
		CleanupClosePushL(*listBoxData);
		_LIT(KBackString, "/..");
		listBoxData->AddTextL(KBackString, EQikListBoxSlotText1);

		CQikContent* icon = CQikContent::NewL(this, KMbmPath,
												 EMbmFilebrowseappFolder,
												 EMbmFilebrowseappMask);
		CleanupStack::PushL(icon);
		listBoxData->AddIconL(icon, EQikListBoxSlotLeftSmallIcon1);
		CleanupStack::Pop(icon);	
		icon = NULL;

		CleanupStack::PopAndDestroy(listBoxData);
		iFilePosition = 1;
  
		TInt result = iEngine.GetDirectoryAndFileList(iPath);
  
		if (result==KErrNone)
			{
			const TInt dirCount = iEngine.DirectoryCount();
			TInt index = 0;
			for (; index <dirCount; ++index)
				{
				listBoxData = model.NewDataL(MQikListBoxModel::EDataUnselectable);
				CleanupClosePushL(*listBoxData);
				listBoxData->AddTextL(iEngine.DirectoryName(index), EQikListBoxSlotText1);

				icon = CQikContent::NewL(this, KMbmPath,
													 EMbmFilebrowseappFolder,
													 EMbmFilebrowseappMask);
				CleanupStack::PushL(icon);
   				listBoxData->AddIconL(icon, EQikListBoxSlotLeftSmallIcon1);
      			CleanupStack::Pop(icon);
				icon = NULL;

				CleanupStack::PopAndDestroy(listBoxData);
				}
			
			iFilePosition += dirCount;
    
			const TInt fileCount = iEngine.FileCount();
			index = 0;
			for (; index <fileCount; ++index)
				{
				listBoxData = model.NewDataL(MQikListBoxModel::EDataNormal);
				CleanupClosePushL(*listBoxData);
				listBoxData->AddTextL(iEngine.FileName(index), EQikListBoxSlotText1);

				icon = CQikContent::NewL(this, KMbmPath,
														 EMbmFilebrowseappFile,
														 EMbmFilebrowseappMask);
				CleanupStack::PushL(icon);
   				listBoxData->AddIconL(icon, EQikListBoxSlotLeftSmallIcon1);
				CleanupStack::Pop(icon);		
				icon = NULL;

				CleanupStack::PopAndDestroy(listBoxData);
				}
			}

		if (iCurrPos > model.Count() ) 
			{// in case file number got changed in between
			iCurrPos = model.Count()-1;
			}
  
		iListBox->SetCurrentItemIndexL(iCurrPos, ETrue, ENoDrawNow);

		model.ModelEndUpdateL();
		}
	}

void CFileBrowseBaseView::HandleListBoxEventL(CQikListBox* /*aListBox*/, TQikListBoxEvent aEventType,
                                           TInt aItemIndex, TInt /*aSlotId*/)
	{
	switch(aEventType)
		{
		case EEventItemConfirmed:
		case EEventItemTapped:
			{// An item has been chosen and will be opened
			NavigateL(aItemIndex);
		    OpenFolderL();
			}
		    break;
		break;
		default: // Nothing to do
		break;
    };
  }

TVwsViewId CFileBrowseBaseView::ViewId() const
	{
	return TVwsViewId(KUidFileBrowseID, KUidFileBrowseBaseViewID);
	}
