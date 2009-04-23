/*
 ============================================================================
 Name		: hellosisDocument.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : ChellosisDocument implementation
 ============================================================================
 */

// INCLUDE FILES
#include "hellosisAppUi.h"
#include "hellosisDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChellosisDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChellosisDocument* ChellosisDocument::NewL(CEikApplication& aApp)
	{
	ChellosisDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// ChellosisDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChellosisDocument* ChellosisDocument::NewLC(CEikApplication& aApp)
	{
	ChellosisDocument* self = new (ELeave) ChellosisDocument(aApp);

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// ChellosisDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChellosisDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// ChellosisDocument::ChellosisDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
ChellosisDocument::ChellosisDocument(CEikApplication& aApp) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// ChellosisDocument::~ChellosisDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
ChellosisDocument::~ChellosisDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// ChellosisDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* ChellosisDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new (ELeave) ChellosisAppUi;
	}

// End of File
