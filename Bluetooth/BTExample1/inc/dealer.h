// dealer.H
//
// Copyright (c) Symbian Software Ltd 2006.  All rights reserved.
//

#ifndef DEALER_H
#define DEALER_H


#include <e32base.h>
#include <in_sock.h>
#include "gameconstants.h"
#include "cardgamedealer.h"




/**
 * The CScabbyQueenDealer is the concrete implementation of the came game architecture,
 * it derives from CCardGameDealer which inturn derives from CCardGameBase. A dealer is
 * the PAN-Gn, therefore when created the RSocket and array of device addresses are passed.
 */

class CScabbyQueenDealer : public CCardGameDealer
	{
public:
	static CScabbyQueenDealer* NewL(CConsoleBase& aConsole, 
									RArray<TInetAddr>& aRemoteNames, 
									RSocket& aSocket);
	~CScabbyQueenDealer();
	// implementations of virtual functions here
	virtual void SendComplete(TInt aError);
	virtual void RecvComplete(TInetAddr aRecvAddr,TInt aError);
	virtual void TimerComplete();
	void StartTheGame();
	void DealDeckL();
	void ShowDeckL();
	void SendPlayerNumbers();
		 
private:
	void InformReceivingPlayer();
	void InformSendingPlayer();
	void InformStaticPlayers();
	void RecvOfCardComplete();
	void RecvOfFinishNotify();
	void CheckPlayerStatus();
	void DealWithGameStatus(TGameStatus aGameStatus);
	void RecvCard();
	void ReceivingPlayerInformed(TInt aError);
	void StaticPlayersInformed(TInt aError);
	void CardSent();
	void SendReadyInquiry();
	void SendPlayerUpdate();
	void PlayerUpdateSent(TInt aError);
	void SendOfCheckStatusComplete(TInt aError);
	void ReadyInquirySent(TInt aError);
	void DealWithStatusFromRecv();
	void DealWithStatusFromSender();
	void ReceiveCardNum();
	void SendHandSize();
	void SendCardNumber();
	void RecvHandSize(TInt aError);
	void SendCompleted(TInt aError);
	void RecvOfAckComplete();
	void TimerCompleted();
	void SentGameOverMessage();
	void InformNextGameOver();
	void InformStaticPlayersProcess();

	
	
protected:
	CScabbyQueenDealer(CConsoleBase& aConsole, RArray<TInetAddr>& aRemoteNames, RSocket& aSocket);	
	void ConstructL();
			
public:
	TBool iGameOver;
	TPckgBuf<TGameStatus> iGameStatus;
	TPckgBuf<TInt> iPlayerNum;
	TInt iCount;
	TInt iPlayerToken;
	TInt iSender;
	TInt iPlayerCount;
	TInt iResendCount;
	TBuf8<20> iPlayerUpdate;
	TBuf8<20> iOldPlayerUpdate;
	TPckgBuf<TInt> iUpdateSize;
	TPckgBuf<TInt> iSenderHandSize;
	TPckgBuf<TInt> iCardNum;
	TPckgBuf<TInt> iAck;
	TPckgBuf<TInt> iGameOverBuffer;
	RArray<TInt> iLostPlayers;
	
private:
	// Dealer send operation enumeration	
	enum TDealerSendMode
		{
		ESendPlayerNum,
		ESendRecvToken,
		ESendSendToken,
		ESendWaitToken,
		ESendCard,
		ESendCheckRecvStatus,
		ESendCheckSenderStatus,
		ESendReadyInquiry,
		ESendPlayerUpdate,
		ESendHandSize,
		ESendPlayerCardNumber,
		ESendGameOver
		};
	// Dealer receive operation enumeration	
	enum TDealerRecvMode
		{
		ERecvSuccess,
		ERecvCardNotification,
		EDealerRecvCard,
		ERecvStatusRecv,
		ERecvStatusSender,
		ERecvReadyResult,
		ERecvUpdateResult,
		EDealerRecvHandSize,
		EDealerRecvCardNum,
		EDealerRecvAck
		};
		
	CConsoleBase& iConsole;
	TDealerRecvMode iRecvMode;
	TDealerSendMode iSendMode;
	HBufC8* iFullDeck;
	RBuf8 iCheckSize;
	TUint iRandom;
	TBuf8<KDeckBufferLength> iTempBuffer;
	TPckgBuf<TPlayerStatus> iStatusBuffer;
	TPckgBuf<TInt> iSuccess;

	TInetAddr iSenderAddr;
	RArray<TInt> iFinishedPlayers;
	RArray<TInt> iCurrentPlayers;
	TInt iResponseCount;
	TInt iWaitingPlayerCount;
	TBuf8<KCardLength> iReceivedCard;
	};
	
	
#endif // DEALER_H