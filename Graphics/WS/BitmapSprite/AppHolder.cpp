// AppHolder.cpp
//
// Copyright (c) 2005 Symbian Softwares Ltd.  All rights reserved.
// @ file
//
//////////////////////////////////////////////////////////////////////////////////////


#include "AppHolder.h"
#include "BitmapSprite.h"
#include <EikStart.h>
#include <gdi.h>

//
// EXPORTed functions
//

EXPORT_C CApaApplication* NewApplication()
	{
	return new CAppholderApplication;
	}


/**
 The below section is added to make the code compatible with v9.1
 This is because only exe files are compatible with v9.1
*/

GLDEF_C TInt E32Main()		
	{
	return EikStart::RunApplication(NewApplication);
	}


////////////////////////////////////////////////////////////////
//
// Application class, CAppholderApplication
//
////////////////////////////////////////////////////////////////

TUid CAppholderApplication::AppDllUid() const
	{
	return KUidAppholder;
	}

CApaDocument* CAppholderApplication::CreateDocumentL()
	{
	// Construct the document using its NewL() function, rather 
	// than using new(ELeave), because it requires two-phase
	// construction.
	return new (ELeave) CAppholderDocument(*this);
	}


////////////////////////////////////////////////////////////////
//
// Document class, CAppholderDocument
//
////////////////////////////////////////////////////////////////

// C++ constructor
CAppholderDocument::CAppholderDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{
	}

CEikAppUi* CAppholderDocument::CreateAppUiL()
	{
    return new(ELeave) CAppholderAppUi;
	}

CAppholderDocument::~CAppholderDocument()
	{
	}

////////////////////////////////////////////////////////////////
//
// App UI class, CAppholderAppUi
//
////////////////////////////////////////////////////////////////

void CAppholderAppUi::ConstructL()
    {
    BaseConstructL();
	iClient=CExampleWsClient::NewL(ClientRect());
    }

CAppholderAppUi::~CAppholderAppUi()
	{
	delete iClient;
	}

void CAppholderAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
	case EEikCmdExit: 
		Exit();
		break;
		}
	}
