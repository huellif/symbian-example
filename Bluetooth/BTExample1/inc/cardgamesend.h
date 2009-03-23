// cardgamesend.H
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef CARDGAMESEND_H
#define CARDGAMESEND_H

#include <e32base.h>
#include <in_sock.h>

class CCardGameBase;

/**
 * CCardGameSend is an active object that has a reference to the RSocket of the network.
 * It calls SendTo on the socket and then calls back to the CCardGameBase object when 
 * the send has completed
 */
class CCardGameSend : public CActive
	{
public:
	static CCardGameSend* NewL(CCardGameBase& aParent, RSocket& aSocket);
	~CCardGameSend();
	void DoCancel();
	void RunL();
	void SendTo(TDesC8& aDes, TInetAddr aAddr);
	
private:
	CCardGameSend(CCardGameBase& aParent, RSocket& aSocket);

private:
	CCardGameBase& iParent;
	RSocket& iSocket;
	
	};
	
#endif // CARDGAMESEND_H