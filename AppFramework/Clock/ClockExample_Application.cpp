// ClockExample_Application.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#include "ClockExample.h"

const TUid KUidClockExample = { 0xE8000098 }; 

// Called by the UI framework to get the application's UID 
TUid CExampleApplication::AppDllUid() const
	{
	return KUidClockExample;
	}

// Called by the UI framework at application start-up to
// create an instance of the document class.
CApaDocument* CExampleApplication::CreateDocumentL()
	{
	return new (ELeave) CExampleDocument(*this);
	}