// BmpAnimGui_Main.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//


#include "BmpAnimGui.h"

// Creates and returns an instance of the CApaApplication-derived class.
static CApaApplication* NewApplication()
	{
	return new CExampleApplication;
	}
	
// Standard entry point function.
TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}






