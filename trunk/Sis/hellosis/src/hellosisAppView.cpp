/*
 ============================================================================
 Name		: hellosisAppView.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include "hellosisAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChellosisAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChellosisAppView* ChellosisAppView::NewL(const TRect& aRect)
	{
	ChellosisAppView* self = ChellosisAppView::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// ChellosisAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChellosisAppView* ChellosisAppView::NewLC(const TRect& aRect)
	{
	ChellosisAppView* self = new (ELeave) ChellosisAppView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// ChellosisAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChellosisAppView::ConstructL(const TRect& aRect)
	{
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect(aRect);

	// Activate the window, which makes it ready to be drawn
	ActivateL();
	}

// -----------------------------------------------------------------------------
// ChellosisAppView::ChellosisAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
ChellosisAppView::ChellosisAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// ChellosisAppView::~ChellosisAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
ChellosisAppView::~ChellosisAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// ChellosisAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void ChellosisAppView::Draw(const TRect& /*aRect*/) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	// Gets the control's extent
	TRect drawRect(Rect());

	// Clears the screen
	gc.Clear(drawRect);

	}

// -----------------------------------------------------------------------------
// ChellosisAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void ChellosisAppView::SizeChanged()
	{
	DrawNow();
	}

// -----------------------------------------------------------------------------
// ChellosisAppView::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void ChellosisAppView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{

	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

// End of File
