/*
 ============================================================================
 Name		: hellosisApplication.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include "hellosis.hrh"
#include "hellosisDocument.h"
#include "hellosisApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChellosisApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* ChellosisApplication::CreateDocumentL()
	{
	// Create an hellosis document, and return a pointer to it
	return ChellosisDocument::NewL(*this);
	}

// -----------------------------------------------------------------------------
// ChellosisApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid ChellosisApplication::AppDllUid() const
	{
	// Return the UID for the hellosis application
	return KUidhellosisApp;
	}

// End of File
