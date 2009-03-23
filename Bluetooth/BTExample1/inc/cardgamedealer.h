// cardgamedealer.H
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef CARDGAMEDEALER_H
#define CARDGAMEDEALER_H

#include "cardgamebase.h"

/**
 * CCardGameDealer is derived from CCardGameBase, the virtual functions 
 * from the base class are not implemented in this class but are implemented
 * in the derived class. When created, an array of addresses is passed in along
 * with an RSocket that is passed to the base class. This class has concrete implementations
 * of functions that shuffle and deal cards.
 */

class CCardGameDealer : public CCardGameBase
	{
public:
	~CCardGameDealer();
	
protected:
	void ConstructL(RArray<TInetAddr>& aRemoteNames, RSocket& aSocket);
		
public:
	void ShuffleCards(TDesC8& aCards);
	void DealCards(TDesC8& aCards);
	
protected:
	// pure virtuals to be implemented by derived classes
	// these are from CCardGameBase but we dont want them
	virtual void SendComplete(TInt aError) = 0;
	virtual void RecvComplete(TInetAddr aRecvAddr,TInt aError) = 0;
	virtual void TimerComplete() = 0;
	
public:
	RArray<TInetAddr> iRemoteNames;
	};
	
#endif // CARDGAMEDEALER_H