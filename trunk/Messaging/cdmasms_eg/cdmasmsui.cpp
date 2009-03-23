//
//	cdmasmsui.cpp
//
//	Copyright (c) 2004 Symbian Software Ltd.  All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#include "cdmasmsui.h"
#include "cdmasms_eg.h"

//
// CConsoleControl: console-type control
//

// Two phase construction
CConsoleControl* CConsoleControl::NewL()
	{
	CConsoleControl* self=new (ELeave) CConsoleControl;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// Set up console control
void CConsoleControl::ConstructL()
	{
    CreateWindowL();
    Window().SetShadowDisabled(ETrue);
    Window().SetBackgroundColor(KRgbGray);
    EnableDragEvents();
    SetExtentToWholeScreen();
	SetBlank();
    iConsole=new(ELeave) CEikConsoleScreen;
	_LIT(KMsgExApp,"MsgExApp");
	iConsole->ConstructL(KMsgExApp,0);
	iConsole->SetHistorySizeL(10,10);
	(iCoeEnv->WsSession()).SetAutoFlush(ETrue);
	iMenuHandler = new (ELeave) CMenuHandler(*iConsole,iBuffer);
	iMenuHandler->ConstructL();
	}


CConsoleControl::~CConsoleControl()
	{
	delete iMenuHandler;
	delete iConsole;
	}


void CConsoleControl::ActivateL()
	{
	CCoeControl::ActivateL();
	iConsole->SetKeepCursorInSight(TRUE);
	iConsole->DrawCursor();
	iConsole->ConsoleControl()->SetFocus(ETrue, EDrawNow); 
	}


TKeyResponse CConsoleControl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (aType!=EEventKey)
		return(EKeyWasConsumed);

	TInt code = aKeyEvent.iCode;
	if (code == EKeyEnter)
		{
		iMenuHandler->ProcessInputL();
		iBuffer.Zero(); 
    	return(EKeyWasConsumed);
    	}
		
	if (iBuffer.Length() == iBuffer.MaxLength()) return EKeyWasConsumed;
	TBuf<1> ch;
	ch.SetLength(1);
	ch[0] = (TText)code;
	iBuffer.Append(ch);
	iConsole->Write(ch);
	
    return(EKeyWasConsumed);
	}
	
void CConsoleControl::StartL()
	{
	iMenuHandler->MenuL();
	}


//
// CSmsEgAppUi: user interface command handling
//

// Set up control
void CSmsEgAppUi::ConstructL()
    {
    BaseConstructL();
	CreateConsoleL();
    }

void CSmsEgAppUi::CreateConsoleL()
	{
	iConsoleControl=CConsoleControl::NewL();
	AddToStackL(iConsoleControl);
	iConsoleControl->ActivateL();
	iConsoleControl->StartL();
	}

CSmsEgAppUi::~CSmsEgAppUi()
	{
	RemoveFromStack(iConsoleControl);
	delete(iConsoleControl);
	}

// Command handling
void CSmsEgAppUi::HandleCommandL(TInt aCommand)
	{
    switch (aCommand)
        {
	// When Exit chosen, quit
	case EEikCmdExit:
		Exit();
	default:;
		}
	}

//
// CSmsEgDocument: document class
//

// Two-phase construction
CSmsEgDocument::CSmsEgDocument(CEikApplication& aApp)
	: CEikDocument(aApp) { }

CSmsEgDocument* CSmsEgDocument::NewL(CEikApplication& aApp)
	{
	CSmsEgDocument* self=new (ELeave) CSmsEgDocument(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CSmsEgDocument::ConstructL()
	{
	}

CSmsEgDocument::~CSmsEgDocument()
	{
	}

CEikAppUi* CSmsEgDocument::CreateAppUiL()
	{
	return new (ELeave) CSmsEgAppUi();
	}

//
// CSmsEgApplication
//

TUid CSmsEgApplication::AppDllUid() const
	{
	return(KUidSmsEgApp);
	}

CApaDocument* CSmsEgApplication::CreateDocumentL()
	{
	return CSmsEgDocument::NewL(*this);
	}

//
// Entry functions
//

#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
	{
	return new CSmsEgApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}

