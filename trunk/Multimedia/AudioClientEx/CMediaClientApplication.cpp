// CMediaClientApplication.cpp
//
// Copyright (c) 2001-2005 Symbian Software Ltd.  All rights reserved.
//

#include "CMediaClientApplication.h"
#include "CMediaClientDocument.h"
#include <eikstart.h>

/** Application UID
@private*/
const TUid KUidMediaClientApp = { 0xE8000062 };


TUid CMediaClientApplication::AppDllUid() const
    {
    return KUidMediaClientApp;
    }

CApaDocument* CMediaClientApplication::CreateDocumentL()
    {
	//User::SetJustInTime(EFalse);
    return CMediaClientDocument::NewL(*this);
    }


//
// EXPORTed functions
//
// Globals the App architecture/OS use to access this application


EXPORT_C CApaApplication* NewApplication()
    {
    return new CMediaClientApplication;
    }

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
