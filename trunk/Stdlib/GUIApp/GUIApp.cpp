// GUIApp.CPP
//
// Copyright (c) 1997-1999 Symbian Ltd.  All rights reserved.
//

//////////////////////////////////////////////////////////////////////////////
//
// OVERVIEW
// --------
//
// This is a GUI application to use the checksum engine.
//
// DIALOGS
// -------
//
// The two dialogs which this application defines:
// 
// a. Allows the user to select a file for which a checksum is calculated
// b. Displays the list of all checksums calculated 
//
//////////////////////////////////////////////////////////////////////////////

#include "GUIApp.h"
#include <stdlib.h> 
#include <eikstart.h>

CExampleChecksumDialog::CExampleChecksumDialog(TDes* aFileName,TInt aTitleId)
	: iFileName(aFileName),iTitleId(aTitleId)
	{}

void CExampleChecksumDialog::PreLayoutDynInitL()
	{
	CEikFileNameSelector* fileNameSelector = (CEikFileNameSelector*) 
		(Control(EEikCidFileNameSel));
    CEikFolderNameSelector* folderNameSelector = (CEikFolderNameSelector*) 
		(Control(EEikCidFolderNameSel));
    CEikDriveNameSelector* driveNameSelector = (CEikDriveNameSelector*) 
		(Control(EEikCidDriveNameSel));
	folderNameSelector->SetFileSelectionObserver(fileNameSelector);
	driveNameSelector->SetFileSelectionObserver(folderNameSelector);
    User::LeaveIfError(EikFileUtils::Parse(*iFileName));
	TParsePtr parse(*iFileName);
	driveNameSelector->SetFullNameL(parse.FullName());
    if (iTitleId)
        SetTitleL(iTitleId);
	}

TBool CExampleChecksumDialog::OkToExitL(TInt /*aKeycode*/)
	{
	_LIT(KSpace," ");

	CEikFileNameSelector* fileNameSelector = (CEikFileNameSelector*) 
		(Control(EEikCidFileNameSel));
	fileNameSelector->ValidateStateL();
	// get the selected filename
	*iFileName = fileNameSelector->FullName();
	// Calculate the checksum
	TUint32 checksum, filesize;
	const TUint16* fn=iFileName->PtrZ();
	int fd = wopen((const wchar_t*)fn, O_RDONLY, 0); // Open file for reading
	if (fd < 0)
		// Can't open file
		{
		iEikonEnv->InfoMsg(R_EXAMPLE_TEXT_ERROR,iFileName);
		return EFalse;
		}
	TInt err=crc(fd, &checksum, &filesize); // (filesize not used)
	if (err==0)
		{
		// display the checksum in an infomessage
		iEikonEnv->InfoMsg(R_EXAMPLE_TEXT_CKSUM,checksum);
		// Append the filename and checksum to the container's descriptor array
		TDes* fileNameAndCksum=iFileName;
		// Append a space to separate filename from checksum
		fileNameAndCksum->Append(KSpace);
		// Convert the checksum to text then append to the descriptor.
		fileNameAndCksum->AppendNum(checksum,EDecimal);
		}
	else iEikonEnv->InfoMsg(R_EXAMPLE_TEXT_CKSUM_ERROR); 
			// can't calculate checksum
	close(fd); // close file
	return ETrue;
	}

CExampleListBoxDialog::CExampleListBoxDialog(CDesCArrayFlat* aCksumArray)
		: iCksumArray(aCksumArray)
	{
	}

void CExampleListBoxDialog::PreLayoutDynInitL()
    {
	CEikTextListBox* listbox=(CEikTextListBox*)Control(EExampleChecksumListBox);
	CDesCArray* listboxArray=((CDesCArray*)listbox->Model()->ItemTextArray());
	if (iCksumArray!=NULL)
		{
		// append contents of the model's checksum array to listbox for display
		for (TInt i=0; i<iCksumArray->Count(); i++)
			listboxArray->AppendL(iCksumArray->operator[](i));
		}
	CEikScrollBarFrame* scrollbarFrame = listbox->CreateScrollBarFrameL();
	// set horizontal and vertical scroll bars to appear when required
	scrollbarFrame->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto,
		CEikScrollBarFrame::EAuto);
	}

void CExampleListBoxDialog::SetSizeAndPositionL(const TSize& aSize)
	{
	// Resize the dialog - keep aSize's height, double its width
	TSize dialogSize(((aSize.iWidth)*2), aSize.iHeight);
	SetCornerAndSize(EHCenterVCenter,dialogSize);
	}

TBool CExampleListBoxDialog::OkToExitL(TInt /*aKeycode*/)
    {
    return(ETrue);
    }


//////////////////////////////////////////////////////////////////////////////
//
// -----> CExampleContainer (definition)
//
//////////////////////////////////////////////////////////////////////////////
CExampleContainer::CExampleContainer()
	{}

	
CExampleContainer::~CExampleContainer()
	{
	delete iChecksumArray;
	}

void CExampleContainer::ConstructL(const TRect& aRect)
	{
	  // Create the window
	CreateWindowL();

	  // granularity 3, ie space for 3 elements added when buffer is reallocated
	iChecksumArray = new (ELeave) CDesCArrayFlat(3);
	
	  // Set the bounding rectangle of this control (this will result in 
	  // a call to the SizeChangedL() member function 
	SetRect(aRect);

	  // Prepares this control and ALL of its contained controls for drawing 
	ActivateL();	
	}


void CExampleContainer::Draw(const TRect&) const
	{
	// Draw the container control 
	CWindowGc& gc=SystemGc();
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetBrushColor(KRgbWhite);
	gc.DrawRect(Rect());
	}


void CExampleContainer::AppendChecksumL(const TDesC& aFilename)
	{
	// Check that the filename and cksum don't already exist in the array
	// If already there, do nothing
	TInt pos; // not interested in value
	if (iChecksumArray->Find(aFilename,pos,ECmpNormal))
		{
		TRAPD(error,iChecksumArray->AppendL(aFilename));
		if (error)
			{
			iChecksumArray->Reset();
			delete iChecksumArray;
			iChecksumArray=0;
			User::Leave(error);
			}
		}
	}

CDesCArrayFlat* CExampleContainer::GetChecksumArray() const
	{
	return iChecksumArray;
	}

void CExampleContainer::HandleControlEventL(CCoeControl* /*aControl*/,
										TCoeEvent /*aEventType*/
									    )
	{
	}


//////////////////////////////////////////////////////////////////////////////
//
// -----> CExampleAppUi (implementation)
//
//////////////////////////////////////////////////////////////////////////////
void CExampleAppUi::ConstructL()
	{
	// Disable control environment's automatic resource checking
	// If we don't do this when using stdlib, we get a CONE 36 panic
	iCoeEnv->DisableExitChecks(ETrue);

	// Allow base class (CEikAppUi) to perform necessary construction
	BaseConstructL();

	  // Construct the container control which forms the main view
	  // for this application. Construction requires the rectangle
	  // available.
	iContainer = new(ELeave) CExampleContainer;
	iContainer->ConstructL(ClientRect());

	  // The container is added to the control stack (for key event
	  // handling).
	AddToStackL(iContainer);
	}
	

CExampleAppUi::~CExampleAppUi()
	{
	RemoveFromStack(iContainer);
	  // Delete the container (control)
	delete iContainer;
	}

void CExampleAppUi::HandleCommandL(TInt aCommand)
	{
	  // Handle the command generated by:
	  //   1. menu item selection
	  //   2. short-cut key press
	  //   3. tool bar item selection
	switch (aCommand)
		{
	case EExampleCmdChecksum:
		OnCmdChecksumL();
		break;
	case EExampleCmdViewChecksums:
		OnCmdViewChecksumsL();
		break;
		// EXIT comand
	case EEikCmdExit:
		OnCmdExit();
		break;
	default :
		break;
		}
	}

void CExampleAppUi::OnCmdChecksumL()
	{
	// Ensure buffer is long enough to hold the max length filename 
	// and the max length checksum 
	_LIT(KM,"C:\\Documents\\");
	TBuf<KMaxFileName+KMaxChecksumLength> fileName(KM);
	CEikDialog* dialog = new (ELeave) CExampleChecksumDialog(&fileName);
	if (dialog->ExecuteLD(R_EXAMPLE_CHECKSUM_DIALOG))
		{
		// A checksum was calculated, so append the descriptor containing it and 
		// the filename to the container's array
		iContainer->AppendChecksumL(fileName);
		}
	}

void CExampleAppUi::OnCmdViewChecksumsL()
	{
    CEikDialog* dialog = new(ELeave) CExampleListBoxDialog
		(iContainer->GetChecksumArray());
    dialog->ExecuteLD(R_EXAMPLE_CHECKSUM_INFO_DIALOG);
	}

void CExampleAppUi::OnCmdExit()
	{
	CBaActiveScheduler::Exit();
	}

void CExampleAppUi::DrawViewL()
	{
	iContainer->SetRect(ClientRect());
	iContainer->DrawNow();
	}

////////////////////////////////////////////////////////////////////////////
//
// -----> CExampleDocument (implementation)
//
/////////////////////////////////////////////////////////////////////////////
CExampleDocument::CExampleDocument(CEikApplication& aApp)
	 : CEikDocument(aApp)
	{}

CExampleDocument::~CExampleDocument()
	{
	// release all of the STDLIB resources associated with this thread
	// Satisfies the CONE policy that the heap should be balanced across 
	// the lifetime of a CCoeEnv.
	CloseSTDLIB();
	}

CExampleDocument* CExampleDocument::NewL(CEikApplication& aApp)
	{
	CExampleDocument* self=new(ELeave) CExampleDocument(aApp);
	return self;
	}


CEikAppUi* CExampleDocument::CreateAppUiL()
	{
    return(new(ELeave) CExampleAppUi);
	}


//////////////////////////////////////////////////////////////////////////////
//
// -----> CExampleApplication (implementation)
//
//////////////////////////////////////////////////////////////////////////////
TUid CExampleApplication::AppDllUid() const
	{
	return(KUidCksumApp);
	}


CApaDocument* CExampleApplication::CreateDocumentL()
	{
	return CExampleDocument::NewL(*this);
	}

//
// EXPORTed functions
//

EXPORT_C CApaApplication* NewApplication()
	{
	return(new CExampleApplication);
	}


GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
	
