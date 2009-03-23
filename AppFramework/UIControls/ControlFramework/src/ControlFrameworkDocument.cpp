// ControlFrameworkDocument.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include <eikapp.h>

#include "ControlFrameworkDocument.h"
#include "ControlFrameworkAppUi.h"


CControlFrameworkDocument::CControlFrameworkDocument(CEikApplication& aApp) : CEikDocument(aApp)
	{
	}

CControlFrameworkDocument::~CControlFrameworkDocument()
	{
	}

CControlFrameworkDocument* CControlFrameworkDocument::NewL(CEikApplication& aApp)
	{
	return new(ELeave) CControlFrameworkDocument(aApp);
	}

//Called by the UI framework to construct the application UI class. 
//Note that the app UI's ConstructL() is called by the UI framework.
CEikAppUi* CControlFrameworkDocument::CreateAppUiL()
	{
	return new(ELeave) CControlFrameworkAppUi();
	}
