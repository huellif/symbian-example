/*
 ============================================================================
 Name		: DirectAppUi.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : CDirectAppUi implementation
 ============================================================================
 */

// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>

#include <Direct.rsg>

#ifdef _HELP_AVAILABLE_
#include "Direct_0xE4698408.hlp.hrh"
#endif
#include "Direct.hrh"
#include "Direct.pan"
#include "DirectApplication.h"
#include "DirectAppUi.h"
#include "DirectAppView.h"
#include "DirectDocument.h"
#include "CLifeEngine.h"

_LIT( KFileName, "C:\\private\\E4698408\\Direct.txt" );
_LIT( KText, "Hello World!");

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CDirectAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDirectAppUi::ConstructL()
	{
	// Initialise app UI with standard value.
	BaseConstructL(CAknAppUi::EAknEnableSkin);

	// Construct the view
	// Create view object
	CDirectAppView *view = new(ELeave)CDirectAppView(
			static_cast<CDirectDocument*> (Document())->LifeEngine());
	CleanupStack::PushL(view);
	view->ConstructL(ClientRect());
	CleanupStack::Pop(view);
	iAppView = view;

	// Create a file to write the text to
	TInt err = CCoeEnv::Static()->FsSession().MkDirAll(KFileName);
	if ((KErrNone != err) && (KErrAlreadyExists != err))
		{
		return;
		}

	RFile file;
	err = file.Replace(CCoeEnv::Static()->FsSession(), KFileName, EFileWrite);
	CleanupClosePushL(file);
	if (KErrNone != err)
		{
		CleanupStack::PopAndDestroy(1); // file
		return;
		}

	RFileWriteStream outputFileStream(file);
	CleanupClosePushL(outputFileStream);
	outputFileStream << KText;

	CleanupStack::PopAndDestroy(2); // outputFileStream, file

	// Construct the Direct overlaying dialog
	iOverlayDialog = new (ELeave) COverlayDialog();
	CActiveScheduler::Add(iOverlayDialog);
	}

// -----------------------------------------------------------------------------
// CDirectAppUi::CDirectAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDirectAppUi::CDirectAppUi()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CDirectAppUi::~CDirectAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CDirectAppUi::~CDirectAppUi()
	{
	if (iAppView)
		{
		delete iAppView;
		iAppView = NULL;
		}
	if (iOverlayDialog)
		{
		delete iOverlayDialog;
		}
	}

// -----------------------------------------------------------------------------
// CDirectAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CDirectAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		case ECommand1:
			{

			// Load a string from the resource file and display it
			HBufC* textResource = StringLoader::LoadLC(R_COMMAND1_TEXT);
			CAknInformationNote* informationNote;

			informationNote = new (ELeave) CAknInformationNote;

			// Show the information Note with
			// textResource loaded with StringLoader.
			informationNote->ExecuteLD(*textResource);

			// Pop HBuf from CleanUpStack and Destroy it.
			CleanupStack::PopAndDestroy(textResource);
			}
			break;
		case ECommand2:
			{
			RFile rFile;

			//Open file where the stream text is
			User::LeaveIfError(rFile.Open(CCoeEnv::Static()->FsSession(),
					KFileName, EFileStreamText));//EFileShareReadersOnly));// EFileStreamText));
			CleanupClosePushL(rFile);

			// copy stream from file to RFileStream object
			RFileReadStream inputFileStream(rFile);
			CleanupClosePushL(inputFileStream);

			// HBufC descriptor is created from the RFileStream object.
			HBufC* fileData = HBufC::NewLC(inputFileStream, 32);

			CAknInformationNote* informationNote;

			informationNote = new (ELeave) CAknInformationNote;
			// Show the information Note
			informationNote->ExecuteLD(*fileData);

			// Pop loaded resources from the cleanup stack
			CleanupStack::PopAndDestroy(3); // filedata, inputFileStream, rFile
			}
			break;
		case EHelp:
			{

			CArrayFix<TCoeHelpContext>* buf = CCoeAppUi::AppHelpContextL();
			HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
			}
			break;
		case EAbout:
			{

			CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog();
			dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
			HBufC* title = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
			dlg->QueryHeading()->SetTextL(*title);
			CleanupStack::PopAndDestroy(); //title
			HBufC* msg = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
			dlg->SetMessageTextL(*msg);
			CleanupStack::PopAndDestroy(); //msg
			dlg->RunLD();
			}
			break;

			// Start command
		case EStart:
			{
			// Different action required for very first start
			// And subsequent restarts
			if (iAppView->State() == CDirectAppView::EDirectNotStarted)
				{
				iAppView->StartDirectL();
				}
			else
				{
				iAppView->PauseDirect();
				static_cast<CDirectDocument*> (Document())->LifeEngine().Reset();
				iAppView->RestartDirect();
				}
			}
			break;

			// Test overlay command
		case ETestOverlay:
			{
			iOverlayDialog->ShowDialog();
			}
			break;

		default:
			Panic(EDirectUi);
			break;
		}
	}
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CDirectAppUi::HandleStatusPaneSizeChange()
	{
	iAppView->SetRect(ClientRect());
	}

CArrayFix<TCoeHelpContext>* CDirectAppUi::HelpContextL() const
	{
#warning "Please see comment about help and UID3..."
	// Note: Help will not work if the application uid3 is not in the
	// protected range.  The default uid3 range for projects created
	// from this template (0xE0000000 - 0xEFFFFFFF) are not in the protected range so that they
	// can be self signed and installed on the device during testing.
	// Once you get your official uid3 from Symbian Ltd. and find/replace
	// all occurrences of uid3 in your project, the context help will
	// work. Alternatively, a patch now exists for the versions of 
	// HTML help compiler in SDKs and can be found here along with an FAQ:
	// http://www3.symbian.com/faq.nsf/AllByDate/E9DF3257FD565A658025733900805EA2?OpenDocument
#ifdef _HELP_AVAILABLE_
	CArrayFixFlat<TCoeHelpContext>* array = new(ELeave)CArrayFixFlat<TCoeHelpContext>(1);
	CleanupStack::PushL(array);
	array->AppendL(TCoeHelpContext(KUidDirectApp, KGeneral_Information));
	CleanupStack::Pop(array);
	return array;
#else
	return NULL;
#endif
	}

//
// CDirectAppUi::COverlayDialog
//

CDirectAppUi::COverlayDialog::COverlayDialog() :
    CActive(EPriorityStandard)
    {
    iNotifier.Connect();
    }

CDirectAppUi::COverlayDialog::~COverlayDialog()
    {
    Cancel();
    iNotifier.Close();
    }

void CDirectAppUi::COverlayDialog::ShowDialog()
    {
    _LIT(KLine1, "Overlaying dialog");
    _LIT(KLine2, "Owned by another thread");
    _LIT(KBut, "OK");

    // Use a notifier to display a dialog from the notifier server thread
    iNotifier.Notify(KLine1, KLine2, KBut, KBut, iR, iStatus);
    SetActive();
    }

void CDirectAppUi::COverlayDialog::RunL()
    {
    // Don't care what the dialog returned
    }

void CDirectAppUi::COverlayDialog::DoCancel()
    {
    }

