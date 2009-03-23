//
//  CImageAppUi
//
// Copyright (c) 2001-2005 Symbian Software Ltd.  All rights reserved.

#include <eikenv.h>
#include <Eikfile.rsg>
#include <eikmenub.h>
#include <eiklabel.h>

#include <imageconv.rsg>
#include "imageconv.hrh"

#include "CImageAppUi.h"
#include "CImageAppView.h"

_LIT(KCancelled, "Cancelled");
_LIT(KLoading,"Loading...");
_LIT(KSaving,"Saving...");
_LIT(KCDrive,"c:\\");
_LIT(KSamplesDir,"c:\\samples\\");
 
void CImageAppUi::ConstructL()
    {
	CEikAppUi::ConstructL();

	// Construct view
    iAppView = new(ELeave) CImageAppView;
    iAppView->ConstructL(ClientRect());
	AddToStackL(iAppView);

	// Bitmap
	iBitmap = new(ELeave) CFbsBitmap();
	iBitmap->Create(TSize(0,0), EColor256);
	iAppView->SetBitmap(iBitmap);

	// Image handler
	iImageHandler = new (ELeave) CImageHandler(*iBitmap, iEikonEnv->FsSession(), *this);
	iImageHandler->ConstructL();

	// Show menu bar
	iEikonEnv->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
	}

CImageAppUi::~CImageAppUi()
	{
	delete iBitmap;
	delete iImageHandler;
	if (iAppView)
		{
		RemoveFromStack(iAppView);
		delete iAppView;
		}
	}

// Handle key events
// ESCAPE cancels loading or saving
TKeyResponse CImageAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
	{
	TKeyResponse ret = EKeyWasConsumed;

	switch (aKeyEvent.iCode)
		{
	case EKeyEscape:
		iImageHandler->Cancel();
		CEikonEnv::Static()->BusyMsgCancel();
		CEikonEnv::Static()->InfoMsg(KCancelled);
		break;
	default:
		ret = EKeyWasNotConsumed;
		break;
		}
	return ret;
	}

// Handle commands
void CImageAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
	// File | Open
	case EImageCmdOpenFile:
		OpenFileL();
		break;
	// File | Save As
	case EImageCmdSaveAs:
		SaveAsL();
		break;
	// Image | Zoom in
	case EImageCmdZoomIn:
		iImageHandler->ZoomFrame(ETrue);
		break;
	// Image | Zoom out
	case EImageCmdZoomOut:
		iImageHandler->ZoomFrame(EFalse);
		break;
	// Image | Rotate clockwise
	case EImageCmdRotateClockwise:
		iImageHandler->FrameRotate(ETrue);
		break;
	// Image | Rotate anti-clockwise
	case EImageCmdRotateAntiClockwise:
		iImageHandler->FrameRotate(EFalse);
		break;
	// Image | Mirror
	case EImageCmdMirror:
		iImageHandler->Mirror();
		break;
	// Image | Flip
	case EImageCmdFlip:
		iImageHandler->Flip();
		break;
	// Image | Refresh
	case EImageCmdRefresh:
		iImageHandler->LoadFileL(iLoadFileName);
		break;
	// Image | Select frame
	case EImageCmdExtractFrame:
		ExtractFrameL();
		break;
	// File | Close
	case EEikCmdExit:
		Exit();
		break;
		}
	}

// Dim out image menu until image loaded
void CImageAppUi::DynInitMenuBarL(TInt /*aResourceId*/, CEikMenuBar* aMenuBar)
	{
	if (iBitmap->SizeInPixels().iWidth == 0)
		{
		// Get title index for Image menu pane
		const CEikMenuBar::CTitleArray& titles = *(aMenuBar->MenuTitleArray());
		for (TInt i=0; i < titles.Count(); i++)
			if (titles[i]->iData.iMenuPaneResourceId == R_IMAGE_VIEW_MENU)
				{
				aMenuBar->SetTitleDimmed(i, ETrue);
				break;
				}
		}
	}

// Open File dialog
void CImageAppUi::OpenFileL()
	{
	if (iLoadFileName.Length() < 4)
		iLoadFileName = KSamplesDir;
	// Select a file
	CEikDialog* dialog = new(ELeave) CEikFileOpenDialog(&iLoadFileName);
	if (!dialog->ExecuteLD(R_EIK_DIALOG_FILE_OPEN)) return;
	CEikonEnv::Static()->BusyMsgL(KLoading);
	// Load the selected image file
	iImageHandler->LoadFileL(iLoadFileName, 0);
	}

// Select frame dialog
void CImageAppUi::ExtractFrameL()
	{
	// Get number of frames
	TInt frameCount = iImageHandler->FrameCount();
	// Select a frame
	TInt selectedFrame = 0;
	CEikDialog* dialog = new(ELeave) CImageCurrentFrameDialog(selectedFrame, frameCount);
	if (!dialog->ExecuteLD(R_IMAGE_FRAME_DIALOG)) return;
	CEikonEnv::Static()->BusyMsgL(KLoading);
	// Load the selected frame
	iImageHandler->LoadFileL(iLoadFileName, selectedFrame);
	}

// Save as... dialog
void CImageAppUi::SaveAsL()
	{
	// Do the dialog
	if (iSaveFileName.Length() < 4)
		iSaveFileName = KCDrive;
	CEikDialog* dialog = new(ELeave) CImageSaveAsDialog(&iSaveFileName, iSaveInfo);
	if (!dialog->ExecuteLD(R_IMAGE_FILE_SAVEAS_DIALOG)) return;
	CEikonEnv::Static()->BusyMsgL(KSaving);
	// Save the image
	iImageHandler->SaveFileL(iSaveFileName, iSaveInfo.iImageType, iSaveInfo.iImageSubType );
	}

// Image operation complete callback
// Leaves will be caught by CONE error handling
void CImageAppUi::ImageOperationComplete(TInt aError)
	{
	CEikonEnv::Static()->BusyMsgCancel();
	User::LeaveIfError(aError);
	iAppView->Clear();
	iAppView->DrawBitmapNow();
	}

//
// CImageCurrentFrameDialog: select frame dialog
//

CImageCurrentFrameDialog::CImageCurrentFrameDialog(TInt& aCurrentFrame,TInt aNumberOfFrames):
	iCurrentFrame(aCurrentFrame),
	iNumberOfFrames(aNumberOfFrames)
	{}

// Initialise dialog with current settings
void CImageCurrentFrameDialog::PreLayoutDynInitL()
	{
	CEikNumberEditor* frames = static_cast<CEikNumberEditor*>(Control(EImageIdNumberOfFrames));
	frames -> SetNumber(0);
	frames -> SetMinimumAndMaximum(0,iNumberOfFrames - 1);
	}

// Store dialog result
TBool CImageCurrentFrameDialog::OkToExitL(TInt /*aButtonId*/)
	{
	CEikNumberEditor* frames = static_cast<CEikNumberEditor*>(Control(EImageIdNumberOfFrames));
	iCurrentFrame = frames -> Number();
	return ETrue;
	}


//
// CImageSaveAsDialog: save as dialog
//

CImageSaveAsDialog::CImageSaveAsDialog(TDes* aFileName, TFileSaveDialogOptions& aSaveInfo):
	CEikFileSaveAsDialog(aFileName),
	iSaveInfo(aSaveInfo)
	{}

// Set dialog with image types
void CImageSaveAsDialog::PreLayoutDynInitL()
	{
	CImageEncoder::GetImageTypesL(iImageTypes);
	CEikChoiceList* types = static_cast<CEikChoiceList*>(Control(EImageIdSaveAsFormat));
	CDesC16ArrayFlat* listItems = new (ELeave) CDesC16ArrayFlat(4);
	for (int i=0; i<iImageTypes.Count(); i++)
		listItems->AppendL(iImageTypes[i]->Description());
	types->SetArrayL(listItems);
	SetImageSubTypes();
	CEikFileSaveAsDialog::PreLayoutDynInitL();
	}

// Set image sub-types list
void CImageSaveAsDialog::SetImageSubTypes()
	{
	// Extract image format type chosen
	CEikChoiceList* subtypes = static_cast<CEikChoiceList*>(Control(EImageIdSubtype)); 
	CEikChoiceList* types = static_cast<CEikChoiceList*>(Control(EImageIdSaveAsFormat));
	TUid type = iImageTypes[types->CurrentItem()]->ImageType();		// Image type
	// Get list of available image sub-types
	TRAPD(err, CImageEncoder::GetImageSubTypesL (type, iImageSubTypes));
	if (err == KErrNotFound) // no sub-types
		{
		subtypes->SetArrayL(R_NO_SUB_TYPES);
		subtypes->SetCurrentItem(0);
		iSubtypeSet = EFalse;
		return;
		}
	User::LeaveIfError(err); // leave for other errors
	// Fill choicelist with sub-types
	CDesC16ArrayFlat* listItems = new (ELeave) CDesC16ArrayFlat(4);
	for (int i=0; i<iImageSubTypes.Count(); i++)
		listItems->AppendL(iImageSubTypes[i]->Description());
	subtypes->SetArrayL(listItems);
	subtypes->SetCurrentItem(0);
	iSubtypeSet = ETrue;
	}

// Update any image sub-types
void CImageSaveAsDialog::HandleControlStateChangeL(TInt aControlId)
	{
	if (aControlId == EImageIdSaveAsFormat) SetImageSubTypes();
	}

// Need to clean up RImageTypeDescriptionArray resources before exiting
CImageSaveAsDialog::~CImageSaveAsDialog()
	{
	iImageSubTypes.ResetAndDestroy();
	iImageSubTypes.Close();
	iImageTypes.ResetAndDestroy();
	iImageTypes.Close();
	}

// Store selected type
TBool CImageSaveAsDialog::OkToExitL(TInt aButtonId)
	{
	// Extract type chosen
	CEikChoiceList* types = static_cast<CEikChoiceList*>(Control(EImageIdSaveAsFormat));
	TInt index = types->CurrentItem();
	iSaveInfo.iImageType = iImageTypes[index]->ImageType();		// Image type
	if (iSubtypeSet)
		{
		CEikChoiceList* subtypes = static_cast<CEikChoiceList*>(Control(EImageIdSubtype));
		iSaveInfo.iImageSubType = iImageSubTypes[subtypes->CurrentItem()]->SubType();// Image sub-type
		}
	else
		iSaveInfo.iImageSubType = KNullUid;

	return CEikFileSaveAsDialog::OkToExitL(aButtonId);
	}
