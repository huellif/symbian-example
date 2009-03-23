// cardgamerecv.H
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef CARDGAMERECV_H
#define CARDGAMERECV_H

#include <e32base.h>
#include <in_sock.h>

class CCardGameBase;

/**
 * CCardGameRecv is an active object that has a reference to the RSocket of the network.
 * It calls RecvFrom on the socket and then calls back to the CCardGameBase object when 
 * the recv has completed
 */
class CCardGameRecv : public CActive
	{
public:
	static CCardGameRecv* NewL(CCardGameBase& aParent, RSocket& aSocket);
	~CCardGameRecv();
	void DoCancel();
	void RunL();
	void RecvFrom(TDes8& aDes);
	
private:
	CCardGameRecv(CCardGameBase& aParent, RSocket& aSocket);

private:
	CCardGameBase& iParent;
	RSocket& iSocket;
	TInetAddr iRecvAddr;
	
	};
	
#endif // CARDGAMERECV_H