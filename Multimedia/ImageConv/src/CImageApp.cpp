//
// ImageApp.cpp
//
// Copyright (c) 2001-2005 Symbian Software Ltd.  All rights reserved.
//

#include "CImageApp.h"
#include "CImageDocument.h"
#include <eikstart.h>

const TUid KUidImageConv = { 0xE8000064 };

TUid CImageApp::AppDllUid() const
	{
	return KUidImageConv;
	}

CApaDocument* CImageApp::CreateDocumentL()
	{
	return new(ELeave) CImageDocument(*this);
	}

//
// Exported function
//

EXPORT_C CApaApplication* NewApplication()
	{
	return new CImageApp;
	}


GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
