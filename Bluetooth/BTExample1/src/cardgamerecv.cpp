// cardgamerecv.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#include <e32base.h>
#include "cardgamerecv.h"
#include "cardgamebase.h"


/**
 * Constructor.
 */
CCardGameRecv::CCardGameRecv(CCardGameBase& aParent, RSocket& aSocket)
	: CActive(EPriorityNormal),
	  iParent(aParent),
	  iSocket(aSocket)
	{
	CActiveScheduler::Add(this);
	}

/**
 * NewLC function, calls 2nd phase constructor.
 */
CCardGameRecv* CCardGameRecv::NewL(CCardGameBase& aParent, RSocket& aSocket)
    {
    CCardGameRecv* self = new (ELeave) CCardGameRecv(aParent, aSocket);
    return self;
    }


/**
 * Destructor.
 */
CCardGameRecv::~CCardGameRecv()
	{
	Cancel();// Cancel Active Object
	}

/**
 * RunL, calls back to parent.
 */
void CCardGameRecv::RunL()
	{
	iParent.RecvComplete(iRecvAddr, iStatus.Int());
	}

/**
 * Function that makes a call to RSocket::RecvFrom
 */
void CCardGameRecv::RecvFrom(TDes8& aDes)
	{
	iSocket.RecvFrom(aDes, iRecvAddr, 0, iStatus);
	SetActive();
	}
/**
 * Implementation of DOCancel
 */	
void CCardGameRecv::DoCancel()
     {
     iSocket.CancelRecv();  
     }
