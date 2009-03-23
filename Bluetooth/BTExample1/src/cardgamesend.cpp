// cardgamesend.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#include <e32base.h>
#include "cardgamesend.h"
#include "cardgamebase.h"
/**
 * Constructor.
 */
CCardGameSend::CCardGameSend(CCardGameBase& aParent, RSocket& aSocket)
	: CActive(EPriorityNormal),
	  iParent(aParent), iSocket(aSocket)
	{
	CActiveScheduler::Add(this);
	}

/**
 * NewLC function, calls 2nd phase constructor.
 */
CCardGameSend* CCardGameSend::NewL(CCardGameBase& aParent, RSocket& aSocket)
    {
    CCardGameSend* self = new (ELeave) CCardGameSend(aParent, aSocket);
    return self;
    }


/**
 * Destructor.
 */
CCardGameSend::~CCardGameSend()
	{
	Cancel();// Cancel Active Object
	}


void CCardGameSend::RunL()
	{
	iParent.SendComplete(iStatus.Int());
	}


void CCardGameSend::SendTo(TDesC8& aDes, TInetAddr aAddr)
	{
	iSocket.SendTo(aDes, aAddr, 0, iStatus);
	SetActive();
	}
	
void CCardGameSend::DoCancel()
     {
     iSocket.CancelSend();
     }
