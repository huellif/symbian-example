/*
 ============================================================================
 Name		: DirectAppView.h
 Author	  : 
 Copyright   : Your copyright notice
 Description : Declares view class for application.
 ============================================================================
 */

#ifndef __DIRECTAPPVIEW_h__
#define __DIRECTAPPVIEW_h__

// INCLUDES
#include <coecntrl.h>

class CLifeEngine;
class CDirectDisplayLife;

// CLASS DECLARATION
class CDirectAppView : public CCoeControl
	{
public:
    // View state
    enum
        {
        EDirectNotStarted = 0, // DSA not used
        EDirectStarted, // DSA in use
        EDirectPaused
        // Use of DSA temporarily halted
        };

public:
	/**
	 * CDirectAppView.
	 * C++ default constructor.
	 */
	CDirectAppView(CLifeEngine& aLifeEngine);

	/**
	 * ~CDirectAppView
	 * Virtual Destructor.
	 */
	virtual ~CDirectAppView();

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 * Perform the second phase construction of a
	 * CDirectAppView object.
	 * @param aRect The rectangle this view will be drawn to.
	 */
	void ConstructL(const TRect& aRect);

    // Start using the DSA
    void StartDirectL();
    // Pause use of the DSA
    void PauseDirect();
    // Restart use of the DSA after pausing
    void RestartDirect();

    // Gets the view state
    TInt State() const;

private:
    // The object that handles the DSA
    CDirectDisplayLife* iDirectDisplayLife;
    // The data the view displays
    CLifeEngine& iLifeEngine;
    // View state
    TInt iState;
    
public:
	// Functions from base classes

	/**
	 * From CCoeControl, Draw
	 * Draw this CDirectAppView to the screen.
	 * @param aRect the rectangle of this view that needs updating
	 */
	void Draw(const TRect& aRect) const;

	/**
	 * From CoeControl, SizeChanged.
	 * Called by framework when the view size is changed.
	 */
	virtual void SizeChanged();

	/**
	 * From CoeControl, HandlePointerEventL.
	 * Called by framework when a pointer touch event occurs.
	 * Note: although this method is compatible with earlier SDKs, 
	 * it will not be called in SDKs without Touch support.
	 * @param aPointerEvent the information about this event
	 */
	virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
private:
	RWindowGroup iTopWindowGroup;
	};

#endif // __DIRECTAPPVIEW_h__
// End of File
