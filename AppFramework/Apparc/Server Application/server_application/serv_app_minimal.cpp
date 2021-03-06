// serv_app_minimal.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//


#include "serv_app_minimal.h"

const TUid KUidServAppMinimal = { 0xE1111556 };


// Called by the UI framework to get the application's UID
TUid CExampleApplication::AppDllUid() const
	{
	return KUidServAppMinimal;
	}

// Called by the UI framework at application start-up to
// create an instance of the document class.
CApaDocument* CExampleApplication::CreateDocumentL()
	{
	return new (ELeave) CExampleDocument(*this);
	}

// Called by the UI framework to get the name of the resource file.
// This returns an empty filename as there is no resource file.
TFileName CExampleApplication::ResourceFileName() const
	{
	return TFileName();
	}
	
// Called by the UI framework to create an instance of the
// CApaAppServer server derived class	
void CExampleApplication::NewAppServerL(CApaAppServer*& aAppServer)
	{
	aAppServer = new (ELeave) CServAppServer;
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
	BaseConstructL(ENoAppResourceFile | ENoScreenFurniture);
	iAppView = CExampleAppView::NewL(ClientRect());
	}

// The application UI class owns one view, and is responsible
// for destroying it.
CExampleAppUi::~CExampleAppUi()
	{
	delete iAppView;
	}

// Called by the UI framework when a command has been issued.
// Minimally needs to handle the exit command.
void CExampleAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		// Exit the application.
		case EEikCmdExit:
			Exit();
			break;
		}
	}

CExampleAppView::CExampleAppView()
	{
	}

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
	{
	}

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
	// Use drawing parameters from the system graphics context.
	CWindowGc& gc = SystemGc();
	gc.Clear();
	}


CMinimalSession::CMinimalSession()
{
}

CMinimalSession::~CMinimalSession()
{
} 

// Implements the pure virtual function of the base class
// It just evokes a notifier window displaying a message
void CMinimalSession::Display(const TDesC & aMessage)
{
	CEikonEnv::Static()->AlertWin(aMessage);
}
