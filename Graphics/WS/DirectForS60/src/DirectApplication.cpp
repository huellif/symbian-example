/*
 ============================================================================
 Name		: DirectApplication.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include "Direct.hrh"
#include "DirectDocument.h"
#include "DirectApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDirectApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CDirectApplication::CreateDocumentL()
	{
	// Create an Direct document, and return a pointer to it
	return CDirectDocument::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CDirectApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CDirectApplication::AppDllUid() const
	{
	// Return the UID for the Direct application
	return KUidDirectApp;
	}

// End of File
