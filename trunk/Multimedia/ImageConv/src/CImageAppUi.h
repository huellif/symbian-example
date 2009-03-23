//
// CImageAppUi.h
//
// Copyright (c) 2001-2005 Symbian Software Ltd.  All rights reserved.
//

#ifndef CIMAGEAPPUI
#define CIMAGEAPPUI

#include <fbs.h>
#include <eikappui.h>
#include <eikdialg.h>
#include <eikcfdlg.h>
#include <eikchlst.h>
#include <eikmfne.h>

#include "CImageHandler.h"


// TFileSaveDialogOptions
// Options that specify how an image should be saved
class TFileSaveDialogOptions
	{
public:
	TUid iImageType;		// Image type
	TUid iImageSubType;		// Image sub-type
	};

class CImageAppView;

// CImageAppUi
// Handles commands, and invokes ICL functions appropriately
class CImageAppUi : public CEikAppUi, public MImageHandlerCallback
    {
public:
    void ConstructL();
	~CImageAppUi();

private:
	// Image operation complete, so need to refresh view
	void ImageOperationComplete(TInt aError);

	// From CCoeAppUi
	TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	
	// From CEikAppUi
	void HandleCommandL(TInt aCommand);
	void DynInitMenuBarL(TInt aResourceId, CEikMenuBar* aMenuBar);
	
	// File command handling functions
	void OpenFileL();
	void SaveAsL();
	void ExtractFrameL();

private:
	CImageHandler* iImageHandler;
    CImageAppView* iAppView;			// App view		
	CFbsBitmap* iBitmap;				// Bitmap
	TFileName iLoadFileName;			// File name to load
	TFileName iSaveFileName;			// File name to save
	TFileSaveDialogOptions iSaveInfo;	// Save options
	};


// CImageCurrentFrameDialog
// Selects frame to extract
class CImageCurrentFrameDialog : public CEikDialog
	{
public:
	CImageCurrentFrameDialog(TInt& aCurrentFrame,TInt aNumberOfFrames);

protected:
	// From CEikDialog
	virtual void PreLayoutDynInitL();
	virtual TBool OkToExitL(TInt aButtonId);

private:
	TInt& iCurrentFrame;
	TInt iNumberOfFrames;
	};

// CImageSaveAsDialog
// Save As dialog
class CImageSaveAsDialog : public CEikFileSaveAsDialog
	{
public:
	CImageSaveAsDialog(TDes* aFileName, TFileSaveDialogOptions& aSaveInfo);
	~CImageSaveAsDialog();

private:
	// from CEikDialog
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
	void HandleControlStateChangeL(TInt aControlId);
	void SetImageSubTypes();

private:
	// dialog settings returned
	TFileSaveDialogOptions& iSaveInfo;
	// stores dynamic list of image types available
	RImageTypeDescriptionArray iImageTypes;
	// stores dynamic list of image subtypes available
	RImageTypeDescriptionArray iImageSubTypes;
	// flag set if sub-type set
	TBool iSubtypeSet;
	};

#endif //CIMAGEAPPUI
