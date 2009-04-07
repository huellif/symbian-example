// Direct.cpp
//
// Copyright (c) 2005 Symbian Softwares Ltd.  All rights reserved.
//

#include "CLifeEngine.h"
#include "Direct.h"
#include "CDirectDisplayLife.h"

#include <eikenv.h>
#include <eikmenub.h>

#include <eikspane.h>

#include <EikStart.h>

//
// CExampleAppView
//

CExampleAppView::CExampleAppView(CLifeEngine& aLifeEngine) :
	iLifeEngine(aLifeEngine)
	{
	}

void CExampleAppView::ConstructL(const TRect& aRect)
	{
	// Create window
	CreateWindowL();
	SetRect(aRect);

	// Set up direct displayer for life engine
	iDirectDisplayLife = new (ELeave) CDirectDisplayLife(
			iEikonEnv->WsSession(), // Window server session
			Window(), // The window itself
			iLifeEngine);
	iDirectDisplayLife->ConstructL();

	ActivateL();
	}

CExampleAppView::~CExampleAppView()
	{
	delete iDirectDisplayLife;
	}

// Start using the DSA
void CExampleAppView::StartDirectL()
	{
	iDirectDisplayLife->StartL();
	iState = EDirectStarted;
	}

// Pause use of the DSA
void CExampleAppView::PauseDirect()
	{
	iState = EDirectPaused;
	iDirectDisplayLife->Cancel();
	}

// Restart use of the DSA after pausing
void CExampleAppView::RestartDirect()
	{
	iState = EDirectStarted;
	iDirectDisplayLife->Restart(RDirectScreenAccess::ETerminateCancel);
	}

// Gets the view state
TInt CExampleAppView::State() const
	{
	return iState;
	}

void CExampleAppView::Draw(const TRect& /*aRect*/) const
	{
	CWindowGc& gc = SystemGc();
	// gray out whole rectangle
	TRect rect = Rect();
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetBrushColor(KRgbGray);
	gc.DrawRect(rect);
	// border
	rect.Shrink(10, 10);
	gc.SetBrushStyle(CGraphicsContext::ENullBrush);
	gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.DrawRect(rect);
	}

//
// CExampleAppUi
//

void CExampleAppUi::ConstructL()
	{
	BaseConstructL();

	// Construct the view
	iAppView = new (ELeave) CExampleAppView(
			static_cast<CExampleDocument*> (Document())->LifeEngine());
	iAppView->ConstructL(ClientRect());

	// Construct the example overlaying dialog
	iOverlayDialog = new (ELeave) COverlayDialog();
	CActiveScheduler::Add(iOverlayDialog);
	}

CExampleAppUi::~CExampleAppUi()
	{
	delete iAppView;
	iOverlayDialog->Cancel();
	delete iOverlayDialog;
	}

// Handle menu commands
void CExampleAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		// Start command
		case EExampleCmd_Start:
			{
			// Different action required for very first start
			// And subsequent restarts
			if (iAppView->State() == CExampleAppView::EDirectNotStarted)
				{
				iAppView->StartDirectL();
				}
			else
				{
				iAppView->PauseDirect();
				static_cast<CExampleDocument*> (Document())->LifeEngine().Reset();
				iAppView->RestartDirect();
				}
			}
			break;
			
			// Test overlay command
		case EExampleCmd_TestOverlay:
			{
			iOverlayDialog->ShowDialog();
			}
			break;
			// Close command
		case EEikCmdExit:
			{
			Exit();
			}
			break;
		}
	}

//
// CExampleAppUi::COverlayDialog
//

CExampleAppUi::COverlayDialog::COverlayDialog() :
	CActive(EPriorityStandard)
	{
	iNotifier.Connect();
	}

CExampleAppUi::COverlayDialog::~COverlayDialog()
	{
	Cancel();
	iNotifier.Close();
	}

void CExampleAppUi::COverlayDialog::ShowDialog()
	{
	_LIT(KLine1, "Overlaying dialog");
	_LIT(KLine2, "Owned by another thread");
	_LIT(KBut, "OK");

	// Use a notifier to display a dialog from the notifier server thread
	iNotifier.Notify(KLine1, KLine2, KBut, KBut, iR, iStatus);
	SetActive();
	}

void CExampleAppUi::COverlayDialog::RunL()
	{
	// Don't care what the dialog returned
	}

void CExampleAppUi::COverlayDialog::DoCancel()
	{
	}

//
// CExampleDocument
//

CExampleDocument::CExampleDocument(CEikApplication& aApp) :
	CEikDocument(aApp)
	{
	}

CExampleDocument::~CExampleDocument()
	{
	delete iLifeEngine;
	}

CLifeEngine& CExampleDocument::LifeEngine() const
	{
	return *iLifeEngine;
	}

CEikAppUi* CExampleDocument::CreateAppUiL()
	{
	// Get a random seed from the timer
	User::After(1);
	TTime now;
	now.HomeTime();

	// Create engine
	iLifeEngine = new (ELeave) CLifeEngine(now.Int64());
	return new (ELeave) CExampleAppUi;
	}

//
// CExampleApplication
//

TUid CExampleApplication::AppDllUid() const
	{
	return KUidExample;
	}

CApaDocument* CExampleApplication::CreateDocumentL()
	{
	return new (ELeave) CExampleDocument(*this);
	}

//
// DLL interface
//

EXPORT_C CApaApplication* NewApplication()
	{
	return new CExampleApplication;
	}

//	The below section is added to make the code compatible with v9.1
//	This is because only exe files are compatible with v9.1
#if (defined __WINS__ && !defined EKA2)		//	E32Dll used only when WINS defined and EKA2 not defined
GLDEF_C TInt E32Dll(enum TDllReason)
	{
	return KErrNone;
	}
#else										//	else E32Main is used
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
#endif

