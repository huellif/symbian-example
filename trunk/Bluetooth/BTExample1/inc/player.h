// player.H
//
// Copyright (c)Symbian Software Ltd 2006.  All rights reserved.
//

#ifndef PLAYER_H
#define PLAYER_H


#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>
#include "gameconstants.h"
#include "cardgameplayer.h"


/**
 * CScabbyQueenPlayer is the concrete class of the card game architecture. It
 * derives from CCardGamePlayer which inturn derives from CCardGameBase. When created
 * a reference to a console is taken along with the RSocket that the player will 
 * communicate on. 
 * 
 */

class CScabbyQueenPlayer : public CCardGamePlayer
	{
public:
	static CScabbyQueenPlayer* NewL(CConsoleBase& aConsole, RSocket& aSocket);
	~CScabbyQueenPlayer();
	// implementations of virtual functions here
	virtual void SendComplete(TInt aError);
	virtual void RecvComplete(TInetAddr aRecvAddr,TInt aError);
	virtual void TimerComplete();
	void ShowHand();
	void GetRightHandPlayerCard();// Not actually a get function
	TInt SendCardNum(TInt aCardNum);

private:
	void RecvOfHandComplete();
	void RecvOfPlayerNumComplete();
	void FindPairsAndRemove();
	void RecvOfPlayTokenComplete();
	void SendOfCardComplete();
	void RecvOfCardComplete();
	void SendCard();
	void ReceiveNextRole();
	void SendGameStatus();
	void RecvUpdate();
	void SendReadyForUpdateComplete();
	void SentTokenRecvSuccess();
	void SentCardRecvNotification();
	void SentGameStatus();
	void SentReadyForToken();
	void SendOfCardNumberComplete();
	void SentHandSize();
	void RecvHandSize();
	void SendCardComplete();
	void SendAck();
	void SendAckAfterRecvCard();
	void SendHandSize();
	void SendOfHandSizeComplete();
	void PrepareToSendCard();
	void AckSentAfterRecvWait(TInt aError);

protected:	
	CScabbyQueenPlayer(CConsoleBase& aConsole, RSocket& aSocket);
	void ConstructL();
private:
	// Player send operation enumeration
	enum TPlayerSendMode
		{
		ESendPlayerCard,
		ESendReadyForUpdate,
		ESendRecvTokenSuccess,
		ESendCardRecvNotification,
		ESendGameStatus,
		ESendReadyForToken,
		ESendCardNumber,
		ESendOwnHandSize,
		EPlayerSendAckSendCard,
		EPlayerSendAckRecvHandSize,
		EPlayerSendAckRecvCard,
		EPlayerSendAckRecvWait
		};
	// Player receive operation enumeration		
	enum TPlayerRecvMode
		{
		EPlayerRecvNum,
		EPlayerRecvHand,
		EPlayerRecvStatus,
		EGameStatus,
		EPlayerUpdate,
		EDealerReady,
		ERecvHandSize,
		ERecvCardNum,
		ERecvCard 
		};	
	CConsoleBase& iConsole;
	//TBuf8<KDeckBufferLength> iTempBuffer;
	TSockXfrLength iHandLength;
	TPckgBuf<TInt> iSuccess;
	TInt iCurrentStatus;
	TBool iRecvTimerSet;
	RBuf8 iCheckSize;
public:
	TInt iResendCount;
	TBool iGameOver;
	RBuf8 iHand;
	TBool iCurrentPlayer;
	TPckgBuf<TPlayerStatus> iStatusBuffer;
	TPckgBuf<TGameStatus> iGameStatus;
	TBool iSendingPlayer;
	TBool iFinished;
	TBuf8<KCardLength> iReceivedCard;
	TBuf8<20> iPlayerUpdate;
//	TSockXfrLength iLengthOfUpdate;// To get the actual size of the update
	TPlayerSendMode iSendMode;
	TPlayerRecvMode iRecvMode;
	
	TPckgBuf<TInt> iPlayerNum;
	TPckgBuf<TInt> iUpdateSize;
	TPckgBuf<TInt> iPeerHandSize;
	TPckgBuf<TInt> iCardNum;
	TPckgBuf<TInt> iOwnHandSize;
	TPckgBuf<TInt> iAck;
	TPckgBuf<TInt> iGameOverBuffer;

	};


#endif // PLAYER_H