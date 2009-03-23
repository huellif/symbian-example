// Copyright (c) 2000-2005 Symbian Software Ltd.  All rights reserved.
//
//
// EIKECHO.CPP
//
// EIKON interface to ECHOENG

// Definitions of CConsoleControl, CEchoAppUi, 
// CEchoDocument, CEchoApplication

#include "Eikecho.h"
#include <eikstart.h>


CConsoleControl* CConsoleControl::NewL(CEchoEngine* aEchoEngine)
	{
	CConsoleControl* self=new (ELeave) CConsoleControl;
	CleanupStack::PushL(self);
	self->ConstructL(aEchoEngine);
	CleanupStack::Pop();
	return self;
	}

void CConsoleControl::ConstructL(CEchoEngine* aEchoEngine)
	{
	iEchoEngine=aEchoEngine;
    CreateWindowL();
    Window().SetShadowDisabled(ETrue);
    Window().SetBackgroundColor(KRgbGray);
    EnableDragEvents();
    SetExtentToWholeScreen();
	SetBlank();
    iConsole=new(ELeave) CEikConsoleScreen;
	_LIT(KEikEcho,"EikEcho");
	iConsole->ConstructL(KEikEcho,0);
	iConsole->SetHistorySizeL(10,10);
	}

CConsoleControl::~CConsoleControl()
	{
	delete iConsole;
	}
/**
 Mark control ready to draw
*/
void CConsoleControl::ActivateL()
	{
	CCoeControl::ActivateL();
	iConsole->SetKeepCursorInSight(TRUE);
	iConsole->DrawCursor();
	iConsole->ConsoleControl()->SetFocus(ETrue, EDrawNow); 
	}

TKeyResponse CConsoleControl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
// The key event handler
    {
	if (aType!=EEventKey)
		return(EKeyWasConsumed);
    TInt aChar=aKeyEvent.iCode;

	if (aChar == '3')
		// Connect using an IP address
		{
		_LIT(KConnecting1,"Attempting connection to KInetAddr\n");
		iConsole->Printf(KConnecting1);
		iEchoEngine->ConnectTo(KInetAddr);
		}
	else if (aChar == '2')
		{
		// Connect using a hostname, calling DNS first to resolve this to an IP address
		_LIT(KConnecting2,"Attempting connection to KInetHostName\n");
		iConsole->Printf(KConnecting2);
		iEchoEngine->ConnectL(KInetHostName);
		}
	else if (aChar == '1')
		{
		// Connect using an IP address, obtaining the symbolic name for this IP address first.
		_LIT(KConnecting3,"Testing GetHostByAddress and attempting connection\n");
		iConsole->Printf(KConnecting3);
		iEchoEngine->TestGetByAddrL(KInetAddr);
		}
	else iEchoEngine->Write(aChar); // Send input character to echo server
	
    return(EKeyWasConsumed);
    }




// The following functions are to implement MUINotify

void CConsoleControl::PrintNotify(const TDesC& aDes) 
// Print up calls: write information to screen
	{
	iConsole->Printf(aDes);
	};

void CConsoleControl::PrintNotify(TInt aInt)
	{
	iConsole->Printf(KIntFormat,aInt);
	};

void CConsoleControl::ErrorNotifyL(const TDesC& aErrMessage, TInt aErrCode)
// Error up call: inform user and quit
	{
	_LIT(KErrorTitle,"Communications error ");
	TBuf<25> errorTitleCode(KErrorTitle);
	errorTitleCode.AppendNum(aErrCode);
	CEikonEnv::Static()->InfoWinL(errorTitleCode,aErrMessage);
	CBaActiveScheduler::Exit();
	}
/**
CEchoAppUi: user interface command handling
*/
CEchoAppUi::CEchoAppUi(CEchoEngine* aEchoEngine)
		: iEchoEngine(aEchoEngine)
	{
	}

void CEchoAppUi::ConstructL()
// Set up control and engine
    {
    BaseConstructL();
	CreateConsoleL();
	iEchoEngine->ConstructL(iConsoleControl);
	_LIT(KCommands,"\nList of Commands:\n\
3 - Test connecting with IP\n\
2 - Test DNS lookup\n\
1 - Test Get hostname from IP address\n\n");

	iConsoleControl->PrintNotify(KCommands);
    }

void CEchoAppUi::CreateConsoleL()
	{
	iConsoleControl=CConsoleControl::NewL(iEchoEngine);
	AddToStackL(iConsoleControl);
	iConsoleControl->ActivateL();
	}

CEchoAppUi::~CEchoAppUi()
	{
	RemoveFromStack(iConsoleControl);
    delete iConsoleControl;
	}

void CEchoAppUi::HandleCommandL(TInt aCommand)
// Command handling
	{
    switch (aCommand)
        {
	// When Exit chosen, stop engine and quit
	case EEikCmdExit:
		iEchoEngine->Stop();
		Exit();
	default:;
		}
	}

/*
 Two-phase construction
 CEchoDocument: document class, which owns the engine
*/
CEchoDocument::CEchoDocument(CEikApplication& aApp)
	: CEikDocument(aApp) { }

CEchoDocument* CEchoDocument::NewL(CEikApplication& aApp)
	{
	CEchoDocument* self=new (ELeave) CEchoDocument(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CEchoDocument::ConstructL()
	{
	iEchoEngine = new (ELeave) CEchoEngine;
	}

CEchoDocument::~CEchoDocument()
	{
	delete iEchoEngine;
	}

CEikAppUi* CEchoDocument::CreateAppUiL()
	{
    return(new(ELeave) CEchoAppUi(iEchoEngine));
	}


TUid CEchoApplication::AppDllUid() const
	{
	return(KUidEikEchoApp);
	}

CApaDocument* CEchoApplication::CreateDocumentL()
	{
	return CEchoDocument::NewL(*this);
	}

/**
 EXPORTed functions
*/
EXPORT_C CApaApplication* NewApplication()
	{
	return(new CEchoApplication);
	}

GLDEF_C TInt E32Main()						
	{
	return EikStart::RunApplication(NewApplication);
	}

