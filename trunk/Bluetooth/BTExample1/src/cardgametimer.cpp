// cardgametimer.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#include <e32base.h>
#include "cardgametimer.h"
#include "cardgamebase.h"
/**
 * Constructor.
 */
CCardGameTimer::CCardGameTimer(CCardGameBase& aParent) 
	: CTimer(EPriorityNormal),
	  iParent(aParent)	 
	{
	CActiveScheduler::Add(this);	
	}

/**
 * NewLC function, calls 2nd phase constructor.
 */
CCardGameTimer* CCardGameTimer::NewL(CCardGameBase& aParent)
    {
    CCardGameTimer* self = new (ELeave) CCardGameTimer(aParent);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/**
 * C'tor, calls the base classes c'tor. 
 */
void CCardGameTimer::ConstructL()
     {
     CTimer::ConstructL();
     }

/**
 * Destructor.
 */
CCardGameTimer::~CCardGameTimer()
	{
	Cancel();// Cancel Active Object
	}

/**
 * Implemention of RunL that make a call back to
 * parent to signal completion
 */	
void CCardGameTimer::RunL()
	{
	iParent.TimerComplete();
	}
	
/**
 * A function that will start a timer for the duration specified
 * 
 * @param aDuration
 * Duration of timer
 */	
void CCardGameTimer::StartTimer(TInt aDuration)
	{
	CTimer::After(aDuration);
	}

