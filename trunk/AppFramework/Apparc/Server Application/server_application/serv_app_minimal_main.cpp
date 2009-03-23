// serv_app_minimal_main.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#include "serv_app_minimal.h"
#include <eikstart.h>

// Creates and returns an instance of the CApaApplication-derived class.
static CApaApplication* NewApplication()
	{
	return new CExampleApplication;
	}

// Standard entry point function.
TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}


