// Anim_Application.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include "AnimExample.h"

const TUid KUidAnimation = { 0xE80000A5 }; 

// Called by the UI framework to get the application's UID 
TUid CAnimationApplication::AppDllUid() const
	{
	return KUidAnimation;
	}

// Called by the UI framework at application start-up to
// create an instance of the document class.
CApaDocument* CAnimationApplication::CreateDocumentL()
	{
	return new (ELeave) CAnimationDocument(*this);
	}
