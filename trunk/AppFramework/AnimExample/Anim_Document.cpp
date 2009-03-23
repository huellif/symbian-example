// Anim_Document.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//


#include "AnimExample.h"

CAnimationDocument::CAnimationDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{}

// Called by the UI framework to construct the application UI class. 
// The app UI's ConstructL() is called by the UI framework.
CEikAppUi* CAnimationDocument::CreateAppUiL()
	{
    return new(ELeave) CAnimationAppUi;
	}
	
