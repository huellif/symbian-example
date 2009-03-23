// ControlFrameworkApplication.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include <eikstart.h> 

#include "ControlFrameworkApplication.h"
#include "ControlFrameworkGlobals.h"
#include "ControlFrameworkDocument.h"


//Standard DLL entry point function.
TInt E32Main()
	{
	return EikStart::RunApplication( CControlFrameworkApplication::NewApplication );
	}

CControlFrameworkApplication::CControlFrameworkApplication()
	{
	}


CControlFrameworkApplication::~CControlFrameworkApplication()
	{
	}

TUid CControlFrameworkApplication::AppDllUid() const
	{
	return KUidControlFrameworkAppUid;
	}


//Creates and returns an instance of the CApaApplication-derived class.
CApaApplication* CControlFrameworkApplication::NewApplication()
	{
	return new CControlFrameworkApplication();
	}

CApaDocument* CControlFrameworkApplication::CreateDocumentL()
	{
	return CControlFrameworkDocument::NewL(*this);
	}
