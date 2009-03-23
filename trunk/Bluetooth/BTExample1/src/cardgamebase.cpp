// cardgamebase.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#include "cardgamebase.h"
#include "cardgamesend.h"
#include "cardgamerecv.h"
#include "cardgametimer.h" 


/**
 * Destructor.
 */
CCardGameBase::~CCardGameBase()
	{
	delete iGameSender;
	delete iGameReceiver;
	delete iGameTimer;
	}
/**
 * Construction
 */
void CCardGameBase::ConstructL(RSocket& aSocket)
     {
     iSocket = aSocket;
     iGameSender = CCardGameSend::NewL(*this, iSocket);
     iGameReceiver = CCardGameRecv::NewL(*this, iSocket);
     iGameTimer = CCardGameTimer::NewL(*this);
     }
/**
 * Concrete function to send a descriptor to specified address.
 * Calls back to SendComplete
 * 
 * @param aDes
 * Reference to descriptor to send 
 * @param aAddr
 * Address to send to
 */
void CCardGameBase::BaseSendTo(TDesC8& aDes, TInetAddr aAddr)
     {
     iGameSender->SendTo(aDes, aAddr);
     }
/**
 * Concrete function to recv a descriptor of specified length.
 * Calls back to RecvComplete.
 * 
 * @param aLength
 * Length of descriptor to recv
 */
void CCardGameBase::BaseRecvFrom(TDes8& aDes)
     {
     iGameReceiver->RecvFrom(aDes);
     }

/**
 * Function that cancels send operation
 */	
void CCardGameBase::BaseCancelSendTo()
      {
      iGameSender->DoCancel();
      }
/**
 * Function that cancels a recv operation.
 */
void CCardGameBase::BaseCancelRecvFrom()
     {
     iGameReceiver->DoCancel();
     }
/**
 * Function to cancel all outstanding active objects.
 */
void CCardGameBase::BaseCancelAll()
     {
     iGameSender->DoCancel();
     iGameReceiver->DoCancel();
     iGameTimer->Cancel();
     }
/**
 * Concrete function that takes a time in milliseconds
 * and starts a timer that will result in a call back to
 * TimerComplete.
 * 
 * @param aDuration
 * Duration of timer
 */
void CCardGameBase::StartTimer(TInt aDuration)
	{
	iGameTimer->StartTimer(aDuration);
	}
	
void CCardGameBase::StopTimer()
	{
    iGameTimer->Cancel();
    }
    
