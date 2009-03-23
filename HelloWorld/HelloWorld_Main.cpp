// HelloWorld_Main.cpp
// -------------------
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.
//

////////////////////////////////////////////////////////////////////////
//
// HelloWorld 
// ----------
//
//
// The example is a simple application containing a single view with
// the text "Hello World !" drawn on it.
// 
// The example includes code for displaying a very simple menu.
//
// --------------------------------------------------------------------- 
//
// This source file contains the single exported function required by 
// all UI applications and the E32Dll function which is also required
// but is not used here.
//
////////////////////////////////////////////////////////////////////////


#include "HelloWorld.h"

//             The entry point for the application code. It creates
//             an instance of the CApaApplication derived
//             class, CExampleApplication.
//

#if defined(EKA2)

#include <eikstart.h>
LOCAL_C CApaApplication* NewApplication()
	{
	return new CExampleApplication;
	}
	
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
	
#endif

#if defined(__WINS__) && !defined(EKA2)
//             This function is required by all Symbian OS DLLs. In this 
//             example, it does nothing.

EXPORT_C CApaApplication* NewApplication()
	{
	return new CExampleApplication;
	}

GLDEF_C TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}
	
EXPORT_C TInt WinsMain(TDesC* aCmdLine)
	{
	return EikStart::RunApplication(NewApplication, aCmdLine);
	}
	
#endif
