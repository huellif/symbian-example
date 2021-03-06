/*
 ============================================================================
 Name		: DirectAppView.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include <eikenv.h>

#include "CLifeEngine.h"
#include "CDirectDisplayLife.h"

#include "DirectAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDirectAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDirectAppView::ConstructL(const TRect& aRect)
	{
	RWindowGroup TopWindowGroup;
	RWsSession& session = CCoeEnv::Static()->WsSession();

	//Creates an initialised window group handle within a our session
	TopWindowGroup = RWindowGroup(session);

	//Completes construction of a window group
	TInt res = TopWindowGroup.Construct((TUint32) &TopWindowGroup, EFalse);
	if (res != KErrNone)
		{
		}
	//Sets the ordinal position of a window.
	TopWindowGroup.SetOrdinalPosition(0, /*ECoeWinPriorityAlwaysAtFront*/0x80000000);
	iTopWindowGroup = TopWindowGroup;
	
	// Create a window for this application view
	CreateWindowL(TopWindowGroup);

	// Set the windows size
	TRect clientRect(aRect);
	clientRect.Shrink(aRect.Width() / 4, aRect.Height() / 4);
	SetRect(clientRect);

    // Set up direct displayer for life engine
    iDirectDisplayLife = new (ELeave) CDirectDisplayLife(
            CEikonEnv::Static()->WsSession(), // Window server session
            Window(), // The window itself
            iLifeEngine);
    iDirectDisplayLife->ConstructL();

    // Activate the window, which makes it ready to be drawn
	ActivateL();
	}

// -----------------------------------------------------------------------------
// CDirectAppView::CDirectAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDirectAppView::CDirectAppView(CLifeEngine& aLifeEngine) :
	    iLifeEngine(aLifeEngine)
	{
	}

// -----------------------------------------------------------------------------
// CDirectAppView::~CDirectAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CDirectAppView::~CDirectAppView()
	{
    delete iDirectDisplayLife;
    iTopWindowGroup.Close();
	}

// Start using the DSA
void CDirectAppView::StartDirectL()
    {
    iDirectDisplayLife->StartL();
    iState = EDirectStarted;
    }

// Pause use of the DSA
void CDirectAppView::PauseDirect()
    {
    iState = EDirectPaused;
    iDirectDisplayLife->Cancel();
    }

// Restart use of the DSA after pausing
void CDirectAppView::RestartDirect()
    {
    iState = EDirectStarted;
    iDirectDisplayLife->Restart(RDirectScreenAccess::ETerminateCancel);
    }

// Gets the view state
TInt CDirectAppView::State() const
    {
    return iState;
    }

// -----------------------------------------------------------------------------
// CDirectAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CDirectAppView::Draw(const TRect& /*aRect*/) const
	{
    CWindowGc& gc = SystemGc();
    // gray out whole rectangle
    TRect rect = Rect();
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.SetBrushColor(KRgbGray);
    gc.DrawRect(rect);
    // border
    rect.Shrink(10, 10);
    gc.SetBrushStyle(CGraphicsContext::ENullBrush);
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
    gc.DrawRect(rect);
    }

// -----------------------------------------------------------------------------
// CDirectAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CDirectAppView::SizeChanged()
	{
	DrawNow();
	}

// -----------------------------------------------------------------------------
// CDirectAppView::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CDirectAppView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{

	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

// End of File
