// MenuApp.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//


#include "MenuApp.h"


const TUid KUidMenuApp = { 0xE800008C };

// Called by the UI framework to get the application's UID
TUid CExampleApplication::AppDllUid() const
	{
	return KUidMenuApp;
	}

// Called by the UI framework at application start-up to
// create an instance of the document class.
CApaDocument* CExampleApplication::CreateDocumentL()
	{
	return new (ELeave) CExampleDocument(*this);
	}

CExampleDocument::CExampleDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{}

// Called by the UI framework to construct
// the application UI class. Note that the app UI's
// ConstructL() is called by the UI framework.
CEikAppUi* CExampleDocument::CreateAppUiL()
	{
	return new(ELeave) CExampleAppUi;
	}

// Second phase constructor of the application UI class.
// It creates and owns a single view.
void CExampleAppUi::ConstructL()
	{
	BaseConstructL();
	iAppView = CExampleAppView::NewL(ClientRect());
	// Connect to the application architecture server
	User::LeaveIfError(iApaLsSession.Connect());
	}

// The application UI class owns one view, and is responsible
// for destroying it. 
CExampleAppUi::~CExampleAppUi()
	{
	// Close the session with the apparc server
	iApaLsSession.Close();
	delete iAppView;
	}

// Called by the UI framework when a command has been issued.
void CExampleAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		// Handles the exit command
		case EEikCmdExit:
			Exit();
			break;

		// any other command is the UID of a selected application
		default:
			TUid appUid;
			// Get the application Uid
			appUid.iUid = aCommand;
			TApaAppInfo info;
			
			// Get the application info based on the application UID
			TInt ret = iApaLsSession.GetAppInfo(info,appUid);
			if(ret==KErrNone)
				{
				CApaCommandLine* cmdLn = CApaCommandLine::NewLC();
				// Launch the application
				cmdLn->SetExecutableNameL(info.iFullName);
				User::LeaveIfError(iApaLsSession.StartApp(*cmdLn));
				CleanupStack::PopAndDestroy(cmdLn);
				}
		}
	}
	
// Called by the UI framework to initialise the menu pane dynamically
void CExampleAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
	{
	if(aResourceId == R_EXAMPLE_FIRST_MENU)
		{	
		CEikMenuPaneItem::SData extraItem;
		TRequestStatus status;
		// Notify when the application list is fully populated
		iApaLsSession.RegisterListPopulationCompleteObserver(status);
		User::WaitForRequest(status);

		if (status == KErrNone)		
			{	
			// Get the list of applications present on the device
			TInt ret = iApaLsSession.GetAllApps();
			if(ret==KErrNone)
				{			
		
				TApaAppInfo appInfo;
				// Retrieve the next application in the application list
				while ((iApaLsSession.GetNextApp(appInfo)) == KErrNone)
					{
					// Set the menu item flags
					extraItem.iCascadeId = 0;
					extraItem.iFlags = EEikMenuItemSymbolIndeterminate|EEikMenuItemSymbolOn;
					// Set the command Id with the application Uid value
					extraItem.iCommandId = appInfo.iUid.iUid;
					// Set the application name in the menu pane item
					extraItem.iText = appInfo.iCaption;
					// Create the application menu item in the menu pane
					aMenuPane->AddMenuItemL(extraItem);
					}
				}
			}
		// Set the exit command Id
 		extraItem.iCommandId = EEikCmdExit;
		_LIT(KText,"Close");
		extraItem.iText = KText;
		// Create the close menu item in the menu pane	
		aMenuPane->AddMenuItemL(extraItem);
		}
	}


CExampleAppView::CExampleAppView()
	{}

// Static function wraps up two-phase construction for the view.
CExampleAppView* CExampleAppView::NewL(const TRect& aRect)
	{
	CExampleAppView* self = new(ELeave) CExampleAppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}

CExampleAppView::~CExampleAppView()
	{}

// Standard initialisation for a window-owning control.
void CExampleAppView::ConstructL(const TRect& aRect)
    {
	// Create the window owned by the view.
	CreateWindowL();
	// Set the view's size and position.
	SetRect(aRect);
	// Activate the view.
	ActivateL();
	}
	
// Draws the view with a simple outline rectangle and then
// draws the welcome text centred.
void CExampleAppView::Draw(const TRect& /*aRect*/) const
	{
	CWindowGc& gc = SystemGc();
	TRect      drawRect = Rect();
	const CFont*     fontUsed;
	gc.Clear();
	drawRect.Shrink(10,10);
	gc.DrawRect(drawRect);
	fontUsed = iEikonEnv->TitleFont();
	gc.UseFont(fontUsed);
	TInt   baselineOffset=(drawRect.Height())/2;
	_LIT(KText,"Welcome to the menu application example");
	gc.DrawText(KText,drawRect,baselineOffset,CGraphicsContext::ECenter, 0);
	gc.DiscardFont();
	}