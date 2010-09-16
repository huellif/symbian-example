/*
 ============================================================================
 Name		: DirectDocument.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : CDirectDocument implementation
 ============================================================================
 */

// INCLUDE FILES
#include "DirectAppUi.h"

#include "CLifeEngine.h"

#include "DirectDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDirectDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDirectDocument* CDirectDocument::NewL(CEikApplication& aApp)
	{
	CDirectDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CDirectDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDirectDocument* CDirectDocument::NewLC(CEikApplication& aApp)
	{
	CDirectDocument* self = new (ELeave) CDirectDocument(aApp);

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CDirectDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDirectDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CDirectDocument::CDirectDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDirectDocument::CDirectDocument(CEikApplication& aApp) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CDirectDocument::~CDirectDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CDirectDocument::~CDirectDocument()
	{
	delete iLifeEngine;
	}

CLifeEngine& CDirectDocument::LifeEngine() const
    {
    return *iLifeEngine;
    }

// ---------------------------------------------------------------------------
// CDirectDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CDirectDocument::CreateAppUiL()
	{
    // Get a random seed from the timer
    User::After(1);
    TTime now;
    now.HomeTime();

    // Create engine
    iLifeEngine = new (ELeave) CLifeEngine(now.Int64());

    // Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new (ELeave) CDirectAppUi;
	}

// End of File
