//
// Copyright (c) 2000-2005 Symbian Software Ltd.  All rights reserved.
//
//
// SecureSockets.CPP
// Definitions of CConsoleControl, CSecAppUi, 
// CSecDocument, CSecApplication

 
#include <eikmenup.h>
#include <eikmfne.h>
#include <eikconso.h>

#include "SecureSockets.h"
#include "SecureSockets.hrh"
	
#include <eikstart.h>




CConsoleControl* CConsoleControl::NewL(const TRect& aClientRect)
	{
	CConsoleControl* self=new (ELeave) CConsoleControl();
	CleanupStack::PushL(self);
	self->ConstructL(aClientRect);
	CleanupStack::Pop();
	return self;
	}

void CConsoleControl::ConstructL(const TRect& aClientRect)
	{
    iConsole=new(ELeave) CEikConsoleScreen;
	_LIT(KEikSec,"EikSec");
	iConsole->ConstructL(KEikSec,aClientRect.iTl, aClientRect.Size(),0,EEikConsWinInPixels);	
	}


CConsoleControl::~CConsoleControl()
	{
	delete iConsole;
	}

CConsoleBase& CConsoleControl::Console()
	{
	return *iConsole;
	}

void CConsoleControl::ActivateL()
// Mark control ready to draw
	{
	CCoeControl::ActivateL();
	iConsole->SetKeepCursorInSight(EFalse);
	iConsole->DrawCursor();
	iConsole->ConsoleControl()->SetFocus(ETrue, EDrawNow); 
	}

// CSettingsDialog: settings dialog
TBool CSettingsDialog::RunDlgLD(TConnectSettings& aConnectSettings)
	{
	CEikDialog* dialog=new(ELeave) CSettingsDialog(aConnectSettings);
	return (dialog->ExecuteLD(R_SECS_SETTINGS));	
	}

CSettingsDialog::CSettingsDialog(TConnectSettings& aConnectSettings)
: iConnectSettings(aConnectSettings)
	{
	}

void CSettingsDialog::PreLayoutDynInitL()
	{
	// Populate dialog with initial settings
	CEikEdwin* name=(CEikEdwin*)Control(EServerName);
	name->SetTextL(&iConnectSettings.iAddress);
	CEikEdwin* page=(CEikEdwin*)Control(EPage);
	TBuf<KSettingFieldWidth> temp;
	temp.Copy(iConnectSettings.iPage);
	page->SetTextL(&temp);
	CEikNumberEditor* port = (CEikNumberEditor*)Control(EPort);
	port->SetNumber(iConnectSettings.iPortNum);
	}

TBool CSettingsDialog::OkToExitL(TInt /*aButtonId*/)
	{
	// Store dialog input
	CEikEdwin* name=(CEikEdwin*)Control(EServerName);
	name->GetText(iConnectSettings.iAddress);
	CEikEdwin* page=(CEikEdwin*)Control(EPage);
	TBuf<KSettingFieldWidth> temp;
	page->GetText(temp);
	iConnectSettings.iPage.Copy(temp);
	CEikNumberEditor* port = (CEikNumberEditor*)Control(EPort);
	iConnectSettings.iPortNum = port->Number();

	return ETrue;
	}

//
// CSecAppUi: user interface command handling
//

// Construction functions
CSecAppUi::CSecAppUi(CSecEngine* aSecEngine)
		: iSecEngine(aSecEngine)
	{
	_LIT(KFile,"c:\\secsocout.txt");
	iOutputFileName = KFile;
	iConnectSettings.iPortNum = 443;
	}

void CSecAppUi::ConstructL()
// Set up control and engine
    {
    BaseConstructL();

	iConsoleControl=CConsoleControl::NewL(ClientRect());
	AddToStackL(iConsoleControl);
	iConsoleControl->ActivateL();

	iSecEngine -> SetConsole( iConsoleControl->Console() );
	_LIT(KStart,"\nChoose Connect from the menu to get a secure web page\n");
	iConsoleControl->Console().Printf(KStart);
    }


CSecAppUi::~CSecAppUi()
	{
	RemoveFromStack(iConsoleControl);
    delete iConsoleControl;
	}
	
void CSecAppUi::HandleCommandL(TInt aCommand)
// Command handling
	{
    switch (aCommand)
        {
	// Start
	case ECmdStart:
		if (CSettingsDialog::RunDlgLD(iConnectSettings))
			{
			User::LeaveIfError(iOutputFile.Replace(iEikonEnv->FsSession(),
				iOutputFileName,
				EFileWrite));
			iSecEngine -> SetOutputFile(iOutputFile);
			iSecEngine -> ConnectL(iConnectSettings);
			}
// This may be helpful for testing - www.fortify.net SSL test
		else
			{
			
			iConnectSettings.iAddress=_L(" 203.124.140.161");
			iConnectSettings.iPage=_L8("https://www.mail.mphasis.com");
//cgi-bin/ssl_2
			User::LeaveIfError(iOutputFile.Replace(iEikonEnv->FsSession(),
				iOutputFileName,
				EFileWrite));
			iSecEngine -> SetOutputFile(iOutputFile);
			iSecEngine -> ConnectL(iConnectSettings);
			}
		break;
	// Cancel
	case ECmdCancel:
		iSecEngine -> Cancel();
		break;
	// When Exit chosen, stop engine and quit
	case EEikCmdExit:
		iSecEngine -> Cancel();
		Exit();
	default:;
		}
	}

void CSecAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
	{
	if (aResourceId == R_CONS_FILE_MENU)
		{
		// Dim items appropriately according to whether in engine started or not
		aMenuPane->SetItemDimmed(ECmdStart,iSecEngine->InUse());
		aMenuPane->SetItemDimmed(ECmdCancel,!(iSecEngine->InUse()));
		}
	}

CSecDocument::CSecDocument(CEikApplication& aApp)
	: CEikDocument(aApp) { }

CSecDocument* CSecDocument::NewL(CEikApplication& aApp)
	{
	CSecDocument* self=new (ELeave) CSecDocument(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CSecDocument::ConstructL()
	{
	iSecEngine = CSecEngine::NewL();
	}
CSecDocument::~CSecDocument()
	{
	delete iSecEngine;
	}

CEikAppUi* CSecDocument::CreateAppUiL()
	{
    return(new(ELeave) CSecAppUi(iSecEngine));
	}

TUid CSecApplication::AppDllUid() const
	{
	return(KUidSecApp);
	}

CApaDocument* CSecApplication::CreateDocumentL()
	{
	return CSecDocument::NewL(*this);
	}

EXPORT_C CApaApplication* NewApplication()
	{
	return(new CSecApplication);
	}

GLDEF_C TInt E32Main()						
	{
	return EikStart::RunApplication(NewApplication);
	}

