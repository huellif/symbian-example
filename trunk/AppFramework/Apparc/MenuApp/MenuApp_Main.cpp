// MenuApp_Main.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#include "MenuApp.h"
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


