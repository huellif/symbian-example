/*
 ============================================================================
 Name		: Direct.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include <eikstart.h>
#include "DirectApplication.h"

LOCAL_C CApaApplication* NewApplication()
	{
	return new CDirectApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}

