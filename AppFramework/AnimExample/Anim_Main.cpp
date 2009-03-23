// Anim_Main.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include "AnimExample.h"

// The entry point for the application code. 
// It creates an instance of the CApaApplication derived class, CAnimationApplication.
static CApaApplication* NewApplication()
	{
	return new CAnimationApplication;
	}

TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}
	





