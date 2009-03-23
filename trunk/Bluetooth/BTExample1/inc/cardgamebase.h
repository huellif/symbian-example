// cardgamebase.H
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef CARDGAMEBASE_H
#define CARDGAMEBASE_H

#include <e32base.h>
#include "cardgamesend.h"
#include "cardgamerecv.h"
#include "cardgametimer.h"

/**
 * CCardGameBase is the base class to which CCardGameDealer, CCardGamePlayer
 * and the user created classes will derive from. It has pointer to active objects that 
 * perform operations on the RSocket that was passed in when the concrete object was created.
 */

class CCardGameBase : public CBase
	{
public:
	~CCardGameBase();
	void BaseSendTo(TDesC8& aDes, TInetAddr aAddr);
	void BaseRecvFrom(TDes8& aDes);
	void BaseCancelSendTo();
	void BaseCancelRecvFrom();
	void BaseCancelAll();
	void StartTimer(TInt aDuration);
	void StopTimer();
	// pure virtuals to be implemented by derived classes,
	// The 'Operations' classes call these
	virtual void SendComplete(TInt aError) = 0;
	virtual void RecvComplete(TInetAddr aRecvAddr,TInt aError) = 0;
	virtual void TimerComplete() = 0;
	
protected:
	void ConstructL(RSocket& aSocket);

private:
	// Operations classes
	CCardGameSend* iGameSender;
	CCardGameRecv* iGameReceiver;
	CCardGameTimer* iGameTimer;
public:
	RSocket iSocket; // Socket passed from derived class
	};
	
#endif // CARDGAMEBASE_H