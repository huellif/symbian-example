// embedded.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#include "embedded.h"

const TUid KAppEmbeddable	= { 0xE800008F };

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
	{
	delete iExampleText;
	}

// Standard initialisation for a window-owning control.
void CExampleAppView::ConstructL(const TRect& aRect)
    {
	TPtrC ptr(KExampleText);
	iExampleText = ptr.AllocL();
	CreateWindowL();
	SetRect(aRect);
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
  	fontUsed = iEikonEnv->TitleFont();
	gc.UseFont(fontUsed);
	TInt   baselineOffset=(drawRect.Height() - fontUsed->HeightInPixels())/2;
	gc.DrawText(*iExampleText,drawRect,baselineOffset,CGraphicsContext::ECenter, 0);
	gc.DiscardFont();
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
	case EEikCmdExit:
		Exit();
		break;
		}
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

// Called by the UI framework at application start-up to
// create an instance of the document class.
CApaDocument* CExampleApplication::CreateDocumentL()
	{
	return new (ELeave) CExampleDocument(*this);
	}

// Called by the UI framework to get the application's UID
TUid CExampleApplication::AppDllUid() const
	{
	return KAppEmbeddable;
	}

// Called by the UI framework to get the name of the resource file.
// This returns an empty filename as there is no resource file.
TFileName CExampleApplication::ResourceFileName() const
{
	return TFileName();
}

