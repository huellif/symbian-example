// cardgametimer.H
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef CARDGAMETIMER_H
#define CARDGAMETIMER_H

#include <e32base.h> 

/**
 * CCardGameTimer is an active object that derives from CTimer. It acts as
 * a timer for events on the socket.
 */
class CCardGameBase;

class CCardGameTimer : public CTimer
	{
public:
	static CCardGameTimer* NewL(CCardGameBase& aParent);
	~CCardGameTimer();
	void RunL();
	void StartTimer(TInt aDuration);
	
private:
	CCardGameTimer(CCardGameBase& aParent);
	void ConstructL();

private:
	CCardGameBase& iParent;
	
	};
	
#endif // CARDGAMETIMER_H