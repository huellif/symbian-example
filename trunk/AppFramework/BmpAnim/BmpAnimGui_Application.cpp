// BmpAnimGui_Application.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include "BmpAnimGui.h"

const TUid KUidBmpAnim = { 0xE80000A4 }; 

// Called by the UI framework to get the application's UID 
TUid CExampleApplication::AppDllUid() const
	{
	return KUidBmpAnim;
	}

// Called by the UI framework at application start-up to
// create an instance of the document class.
CApaDocument* CExampleApplication::CreateDocumentL()
	{
	return new (ELeave) CExampleDocument(*this);
	}
