// cardgameplayer.H
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef CARDGAMEPLAYER_H
#define CARDGAMEPLAYER_H

#include "cardgamebase.h"

/**
 * CCardGamePlayer is derived from the CCardGameBase base class. It allows for 
 * a card game player concrete class to be derived from it.
 */

class CCardGamePlayer : public CCardGameBase
	{
public:
	~CCardGamePlayer()
		{
		}
	
protected:
	void ConstructL(RSocket& aSocket)
		{
		CCardGameBase::ConstructL(aSocket);
		}
public:
	// pure virtuals to be implemented by derived classes
	// these are from CCardGameBase but we dont want them
	virtual void SendComplete(TInt aError) = 0;
	virtual void RecvComplete(TInetAddr aRecvAddr,TInt aError) = 0;
	virtual void TimerComplete() = 0;

	};
	
#endif // CARDGAMEPLAYER_H