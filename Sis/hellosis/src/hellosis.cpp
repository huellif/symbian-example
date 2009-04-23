/*
 ============================================================================
 Name		: hellosis.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include <eikstart.h>
#include "hellosisApplication.h"

LOCAL_C CApaApplication* NewApplication()
	{
	return new ChellosisApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}

