// CMediaClientView.h
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
//


#ifndef CMEDIACLIENTVIEW
#define CMEDIACLIENTVIEW

#include <coecntrl.h>

class CMediaClientEngine;

/** 
Application view. 

It simply writes a string describing the current state of the application.
 */

class CMediaClientView : public CCoeControl 
    {
public:
/**
Allocates and constructs a new view.
@cat Construction and destruction
@return New view object
@param aRect App UI area
@param aEngine Application engine
*/
    static CMediaClientView* NewL(const TRect& aRect, CMediaClientEngine& aEngine);
/**
Destructor.
@cat Construction and destruction
*/
    ~CMediaClientView();

	// 

/**
Sets the view's text.

@param aDescription View's new text
*/
	void SetDescription(const TDesC& aDescription);

private: 

/**
Redraws the view.

From CCoeControl.
@param aRect Redraw area
*/
    void Draw(const TRect& /*aRect*/) const;

private:
/**
Constructor.
@cat Construction and destruction
@param aEngine Application engine
*/
	CMediaClientView(CMediaClientEngine& aEngine);
/**
Second phase constructor.
@cat Construction and destruction
@param aRect App UI area
*/    void ConstructL(const TRect& aRect);

private:
    /** Application engine*/	
	CMediaClientEngine& iEngine;
	/** Current view text */
	TBufC<25> iDescription;
    };

#endif // CMEDIACLIENTVIEW
