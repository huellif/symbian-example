// ClockExample_Document.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//


#include "ClockExample.h"


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
