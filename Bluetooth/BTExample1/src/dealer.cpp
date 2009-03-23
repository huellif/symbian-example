// dealer.cpp
//
// Copyright (c) Symbian Software Ltd 2006.  All rights reserved.
//

#include <e32base.h>
#include <e32cons.h>

#include <btsdp.h>
#include "dealer.h"
#include "cardgamedealer.h"



/**
 * CScabbyQueenDealer NewL. 
 */
CScabbyQueenDealer* CScabbyQueenDealer::NewL(CConsoleBase& aConsole, 
											 RArray<TInetAddr>& aRemoteNames, 
											 RSocket& aSocket)
	{
	CScabbyQueenDealer* self = new CScabbyQueenDealer(aConsole, aRemoteNames, aSocket);
	self->ConstructL();
	return self;
	}

/**
 * Primary constructor. 
 */
CScabbyQueenDealer::CScabbyQueenDealer(CConsoleBase& aConsole, 
									   RArray<TInetAddr>& aRemoteNames, 
									   RSocket& aSocket)
									   :iConsole(aConsole) 
	{
	CCardGameDealer::ConstructL(aRemoteNames, aSocket);
	}

void CScabbyQueenDealer::StartTheGame()
	{
	InformStaticPlayers();
	}

/**
 * Destructor. 
 */	
CScabbyQueenDealer::~CScabbyQueenDealer()
	{
	iLostPlayers.Close();
	iFinishedPlayers.Close();
	iCurrentPlayers.Close();
	}

/**
 * Secondary constructor. 
 */	
void CScabbyQueenDealer::ConstructL()
	{
	iPlayerToken = 0;
	// construct the deck.
	iFullDeck = HBufC8::NewL(KDeckBufferLength);
	iFullDeck->Des().Append(KHearts);
	iFullDeck->Des().Append(KDiamonds);
	iFullDeck->Des().Append(KSpades);
	iFullDeck->Des().Append(KClubs);
	iGameOverBuffer = KErrCompletion;
	iPlayerNum = 0;
	}
	
/**
 * Send out the player numbers
 */	
void CScabbyQueenDealer::SendPlayerNumbers()
	{
	// Player are waiting to receive player numbers from 
	// us so give them
	if (iPlayerNum() < iRemoteNames.Count())
		{		
		iSendMode = ESendPlayerNum;
		BaseSendTo(iPlayerNum, iRemoteNames[iPlayerNum()]);
		iPlayerNum()++;
		}

	}
/**
 * Function that will deal the deck out to the players. 
 */
void CScabbyQueenDealer::DealDeckL()
	{
	DealCards(*iFullDeck);
	iConsole.Printf(_L("\nDealt"));
	iFullDeck->Des().Delete(0, iFullDeck->Length());
	}
	

/**
 * THis function will be called each time a send has completed
 */	
void CScabbyQueenDealer::SendComplete(TInt aError)
	{
	switch(iSendMode)	
		{
		case ESendPlayerNum:
		SendPlayerNumbers();
		break;
		case ESendRecvToken:
			ReceivingPlayerInformed(aError);
		break;
	
		case ESendSendToken:
			RecvHandSize(aError);
		break;
	
		case ESendWaitToken:
			SendCompleted(aError);
		break;
	
		case ESendCard:
			CardSent();
		break;
	
		case ESendCheckRecvStatus:
			SendOfCheckStatusComplete(aError);
		break;
	
		case ESendCheckSenderStatus:
			SendOfCheckStatusComplete(aError);
		break;
	
		case ESendReadyInquiry:
			ReadyInquirySent(aError);
		break;
	
		case ESendPlayerUpdate:
			PlayerUpdateSent(aError);
		break;
	
		case ESendHandSize:
			SendCompleted(aError);
		break;
	
		case ESendPlayerCardNumber:
			SendCompleted(aError);
		break;

		case ESendGameOver:
			SentGameOverMessage();
		break;
		default:
		break;
		}
	}
	

	
/**
 * Implementation of base class virtual function
 */	
void CScabbyQueenDealer::RecvComplete(TInetAddr /*aRecvAddr*/, TInt /*aError*/)
	{

	switch (iRecvMode)
		{			
		case ERecvSuccess:
			InformSendingPlayer();
		break;
		
		case ERecvCardNotification:
			RecvOfFinishNotify();
		break;
		
		case EDealerRecvCard:
			RecvOfCardComplete();
		break;
		
		case ERecvStatusRecv:
			DealWithStatusFromRecv();
		break;
		
		case ERecvStatusSender:
			DealWithStatusFromSender();
		break;
		
		case ERecvReadyResult:
			SendReadyInquiry();// call the next
		break;
		
		case ERecvUpdateResult:
			SendPlayerUpdate();//call the next
		break;
	
		case EDealerRecvHandSize:
			SendHandSize();
		break;
		
		case EDealerRecvCardNum:
			SendCardNumber();
		break;
			
		case EDealerRecvAck:
			RecvOfAckComplete();
		break;
			
		default:
			break;
		}

	
	}
	
void CScabbyQueenDealer::TimerComplete()
	{
	// the ack timer has complete before we have received the ack
	iConsole.Printf(_L("\nTimed out! Informing players of Game Over"));
	iConsole.Printf(_L("\nFatal Error, exiting game"));
	BaseCancelSendTo();
	InformNextGameOver();

	}
	
	
/**
 * Function that displays the deck to the dealer.
 */	
void CScabbyQueenDealer::ShowDeckL()
	{
	TInt cardNum;
	iConsole.Printf(_L("\nThe Deck...\n"));
	
	for (TInt i=0; i<=iFullDeck->Length()-KCardLength; i=i+KCardLength)
		{
		if (i==0)
			cardNum = i;
		else
			cardNum = i/2;
		HBufC16* buffer = HBufC16::NewL(4);
		buffer->Des().Copy(iFullDeck->Mid(i,2));
		iConsole.Printf(_L("%d - %S | "),cardNum,buffer);
		}
	iConsole.Getch();
	}
	

/**
 * Function that will inform the receiving player (iPlayerToken in socket array)
 * of their receiving status.
 */	
void CScabbyQueenDealer::InformReceivingPlayer()
	{
	if (iPlayerToken == iRemoteNames.Count())
		{
		// reset the Token to start of socket array
		iPlayerToken = 0;
		}
		
	iRecvMode = ERecvSuccess;
	iCheckSize.Create(iAck);
	BaseRecvFrom(iAck); // On return iRecvDes will be filled with iAck
	iCheckSize.Close();
	//iAckTimer->Cancel();
	StopTimer();
	iStatusBuffer = EReceiving;
	iSendMode = ESendRecvToken;// set the mode for the active object
	BaseSendTo(iStatusBuffer, iRemoteNames[iPlayerToken]);
	StartTimer(KMaxSendTime);
	}

/**
 * Function called when the receiving player has been notified,
 * if the send was successful then we will setup to Recv an ack from the player
 * telling us that they have receive the status successfully.
 * If the send failed, we will try and send one more time but if we fail again
 * then we we start the Game Over procedure. 
 */
void CScabbyQueenDealer::ReceivingPlayerInformed(TInt aError)
	{
	if (aError == KErrNone)
		{

		}
	else
		{
		// send has failed.
		StopTimer();
		BaseCancelRecvFrom();
		// that the game is over.
		iConsole.Printf(_L("\nFatal Error, exiting game"));
		iCount = iSender;
		InformNextGameOver();
		}
	}

/**
 * Inform the player that will be sending (iSender).
 */
void CScabbyQueenDealer::InformSendingPlayer()
	{
	if (iAck() == KErrNone)
		{
		StopTimer();
		// We wont set an ack this time, instead when the sender sends
		// back their hand size we will use this as an ack.
		iStatusBuffer = ESending;
		iSendMode = ESendSendToken;
		BaseSendTo(iStatusBuffer, iRemoteNames[iSender]);
		StartTimer(KMaxSendTime);
		}
	else
		{
		// We did get an Ack back but it was what we expected
		}

	}

/**
 * Inform the players that wont be doing anything this time round.
 * i.e. not the sender or receiver of a card.
 */
void CScabbyQueenDealer::InformStaticPlayers()
	{
	StopTimer();
	InformStaticPlayersProcess();
	}
	
/**
 * Function that deals with informing the static players.
 */
void CScabbyQueenDealer::InformStaticPlayersProcess()
	{	
	// inform everyone except current receviver (iPlayerToken) and iSender
	while (iFinishedPlayers.Find(iPlayerToken) != KErrNotFound)// make sure we are not a finished player
		{
		iPlayerToken++;
		if (iPlayerToken == iRemoteNames.Count())
			iPlayerToken = 0;// we have come to the end so start over.
		}
	iCount++;// increment count, this will be used to address the socket array.

	if (iPlayerToken == 0)
		iSender = iRemoteNames.Count()-1;
	else
		iSender = iPlayerToken-1;
	
	if (iPlayerCount == 2)
		{
		InformReceivingPlayer();// no waiting players so move directly to next stage
		}
	else
		{
		// as there are more that 2 players we need to send out wait nofitications
		if (iCount == iRemoteNames.Count())
			{
			iCount = 0;
			}
			
	
		if (iCount != iPlayerToken && iCount != iSender)
			// make sure that iCount isn't a sender or receiver
			{
			// Get ready to receive the ack back
			iRecvMode = EDealerRecvAck;
			iCheckSize.Create(iAck);
			BaseRecvFrom(iAck);
			iCheckSize.Close();
			StopTimer();
			iSendMode = ESendWaitToken;
			iStatusBuffer = EWaiting;
			BaseSendTo(iStatusBuffer, iRemoteNames[iCount]);
			StartTimer(KMaxSendTime);
			}
		else
			{
			// If iCount is a S or R then recall the function to increment iCount
			InformStaticPlayers();
			}
		}
	}

/**
 * Function that is called after all the static players have been informed.
 * When they have we move onto the next stage which is informing the Receiver and 
 * then the sender. 
 * If there was an error in sending, we try to send to the last player again, if
 * we fail a second time then we start the game over process.
 */
void CScabbyQueenDealer::StaticPlayersInformed(TInt aError)
	{
	StopTimer();	
	if (aError == KErrNone)
		{
		// this will be called when all static players have been informed
		iResponseCount = 0;
		InformReceivingPlayer();
		}
	else
		{
		// send has failed.
		BaseCancelSendTo();
		iConsole.Printf(_L("\nFatal Error, exiting game"));
		InformNextGameOver();	
		}

	}

/**
 * Function called to setup the Recv of the players hand size.
 */
void CScabbyQueenDealer::RecvHandSize(TInt aError)
	{
	if (aError == KErrNone)
		{
		// This will also act as the ack for sender token send as well
		iRecvMode = EDealerRecvHandSize;
		iCheckSize.Create(iSenderHandSize);
		BaseRecvFrom(iSenderHandSize);
		iCheckSize.Close();
		}
	else
		{
		// send has failed.
		BaseCancelSendTo();	
		iConsole.Printf(_L("\nFatal Error, exiting game"));
		iCount = iSender;
		InformNextGameOver();
		}

	}


/**
 * Function called when we, the dealer, receives an Ack from a player.
 * A switch statement is used to determine what the player is ack'ing,
 * then the appropriate function is called.
 */
void CScabbyQueenDealer::RecvOfAckComplete()
	{
	// when the ack recv completes we will get called.
	// if all is ok then we want to setup the next stage.
	iResendCount = 0;
	StopTimer();
	if (iAck() == KErrNone)// did they receive it ok?
		{
		switch (iSendMode)
			{
			case ESendHandSize:
				ReceiveCardNum();
			break;

			case ESendWaitToken:
				{
				// We have received the ack after dealer told waiting player
				iWaitingPlayerCount++;
				if (iWaitingPlayerCount == iPlayerCount-2)
					{
					// All waiting players informed
					StaticPlayersInformed(KErrNone);
					iWaitingPlayerCount = 0; 
					}
				else
					{
					//we havent finished telling all the waiting players yet
					InformStaticPlayers();
					}	
				}

			break;
			default:
			break;
			}
		}
	else
		{
		// we have the ack back but the sender has reported an error
		iConsole.Printf(_L("\nFatal Error, exiting game"));
		iCount = 0;
		BaseCancelSendTo();
		InformNextGameOver();
		}
	}

/**
 * Function called to send the size of the senders hand to the receiver.
 * The usual error check exists , we also get ready to receive the Ack from
 * the receiver.
 */
void CScabbyQueenDealer::SendHandSize()
	{
	// We have now recv the hand size from sender, this is also the ack.
	if (iSenderHandSize() >= 0)
		{
		StopTimer();
		// get ready to receive the ack
		iRecvMode = EDealerRecvAck;
		iCheckSize.Create(iAck);
		BaseRecvFrom(iAck);
		iCheckSize.Close();
		//now send the hand size on
		iSendMode = ESendHandSize;
		BaseSendTo(iSenderHandSize, iRemoteNames[iPlayerToken]);
		StartTimer(KMaxSendTime);
		}
	else
		{
		// We have fail to send twice 
		iConsole.Printf(_L("\nFatal Error, exiting game"));
		BaseCancelSendTo();
		iCount = iPlayerToken;
		InformNextGameOver();
		}

	}
	
	
/**
 * Function that will be called until all the devices on the PAN have
 * been informed that the Game is over.
 */	
void CScabbyQueenDealer::InformNextGameOver()
	{

	if (iRemoteNames.Count() > iLostPlayers.Count())// still got players?
		{
		// before we tell the next player that we have finished
		// check that we havent already disconnected.
		TInt index = iLostPlayers.Find(iCount);
		if (index == KErrNotFound)
			{
			// remove the device just lost
			iLostPlayers.Append(iCount);
			iRemoteNames.Remove(iCount);
			iCount++;//move onto next
			// we are ok to disconnect a device
			
			if (iCount == iRemoteNames.Count())
				iCount = 0;
			//now send the hand size on
			iSendMode = ESendGameOver;
			BaseSendTo(iGameOverBuffer, iRemoteNames[iCount]);
			}
		else
			{
			// device already disconnected, move onto next
			iCount++;
			InformNextGameOver();
			}	
		}
	else
		{
		iConsole.Printf(_L("\nAll players have left the game"));
		}

	}

/**
 * Get ready to receive the card number that the receiver has selected.
 */	
void CScabbyQueenDealer::ReceiveCardNum()
	{
	iRecvMode = EDealerRecvCardNum;
	BaseSendTo(iCardNum, iRemoteNames[iPlayerToken]);
	}
	
/**
 * Now that we have received the card number from the receiver we 
 * send it to the sender who will then send that card from theirhand.
 */
void CScabbyQueenDealer::SendCardNumber()
	{
	if (iCardNum() >= 0)
		{
		StopTimer();
		// get ready to receive the card (this will also be the ack)
		RecvCard();
		// send the player the card number we just received
		iSendMode = ESendPlayerCardNumber;
		BaseSendTo(iCardNum, iRemoteNames[iSender]);
		StartTimer(KMaxSendTime);
		}
	else
		{
		// We have failed to send twice, start the game over procedure.
		iConsole.Printf(_L("\nFatal Error, exiting game"));
		BaseCancelSendTo();
		BaseCancelRecvFrom();
		iCount = iPlayerToken;
		InformNextGameOver();
		}
	}

/**
 * Called when some of the sends complete. These sends have Ack timers,
 * so when the send completes successfully nothing is done, wait until the
 * receive of the ack completes before stopping the timer.
 */
void CScabbyQueenDealer::SendCompleted(TInt aError)
	{
	// in the case where the send completes successfully nothing will be done.
	// if there was an error in the send we will cancel the timer and attempt a resend
	if (aError !=KErrNone)
		{
		// we have failed to send
		// cancel timer
		StopTimer();
		BaseCancelRecvFrom();
		// We have failed to send twice, start the game over procedure.
		iConsole.Printf(_L("\nFatal Error, exiting game"));
		iCount = 0;
		InformNextGameOver();
		}
	else
		{
		//we have sent successfully, timer will continue.
		// when recv of ack completes we can proceed
		}
	}
	
	
/**
 * Setup to receive card from the sender.
 */
void CScabbyQueenDealer::RecvCard()
	{
	iRecvMode = EDealerRecvCard;
	BaseRecvFrom(iReceivedCard);
	}


/**
 * Dealer has received the card, send it to the receiver player. Also we need
 * to get ready to recv the ack back from the receiver after they receive the card.
 */
void CScabbyQueenDealer::RecvOfCardComplete()
	{
	// we have recvd the card from sender. this is also the ack.
	if (iReceivedCard != KNullDesC8)
		{
		StopTimer();
		iSendMode = ESendCard;
		BaseSendTo(iReceivedCard, iRemoteNames[iPlayerToken]);
		}
	else
		{
		// We have failed to send twice so start the Game over procedure.
		iConsole.Printf(_L("\nFatal Error, exiting game"));
		iCount = iSender;
		BaseCancelSendTo();
		InformNextGameOver();
		}
			
	}

/**
 * Called after a send of card to the receiver was successful. Get ready to receive 
 * a sttus check from the player.
 */
void CScabbyQueenDealer::CardSent()
	{
	// We have sent the card to the player, now get ready to receive an ack from them
	iRecvMode = ERecvCardNotification;
	iCheckSize.Create(iStatusBuffer);
	BaseRecvFrom(iStatusBuffer);
	iCheckSize.Close();
	}
	
	
/**
 * Receiving player has ack'd back to say that they have received the card.
 * Display the move that has just taken place. And call the function CheckPlayerStatus()
 * that will begin the process of collecting player status'.
 */	
void CScabbyQueenDealer::RecvOfFinishNotify()
	{
	// receiver has told us he has received the card, he and the sender are waiting
	// this stuff want to go into function called after we have heard back from player status
	if (iStatusBuffer() == EReceivedCard)
		{
		iConsole.Printf(_L("\nPlayer[%d] ----> Player[%d]"),iSender , iPlayerToken);
		CheckPlayerStatus();
		}
	else
		{
		iConsole.Printf(_L("\nWe were expecting received card!"));
		}

	}

/**
 * Function called when the send of a check status request completes.
 * This could either be sent to the receiver or the sender.
 * After we have sent the request, get ready to receive it.
 */
void CScabbyQueenDealer::SendOfCheckStatusComplete(TInt aError)
	{
	if (iSendMode == ESendCheckRecvStatus)
		{
		// we have sent to the recv'r
		if (aError == KErrNone)
			{
			iRecvMode = ERecvStatusRecv;
			iCheckSize.Create(iGameStatus);
			BaseRecvFrom(iGameStatus);
			iCheckSize.Close();
			}
		else
			{
			// send has failed.
			StartTimer(KMaxSendTime);
			BaseCancelSendTo();
			iConsole.Printf(_L("\nFatal Error, exiting game"));
			iCount = iSender;
			InformNextGameOver();
			}
		}
	else
		{
		// we have sent to the sender.
		if (aError == KErrNone)
			{
			iRecvMode = ERecvStatusSender;
			iCheckSize.Create(iGameStatus);
			BaseRecvFrom(iGameStatus);
			iCheckSize.Close();
			}
		else
			{
			// send has failed.
			StopTimer();
			BaseCancelSendTo();
			iConsole.Printf(_L("\nFatal Error, exiting game"));
			iCount = iPlayerToken;	
			InformNextGameOver();
			}
		}
	}

	
/**
 * Function called after the dealer receives a status response from the sender.
 * The players status will be dealt with in the DealWithGameStatus() function.
 */	
void CScabbyQueenDealer::DealWithStatusFromSender()
	{
	// We have got the status of the Sender, deal with it
	StopTimer();
	iResendCount = 0;
	DealWithGameStatus(iGameStatus());
	}
	
/**
 * Function called after the dealer receives a status response from the receiver.
 * The players status will be dealt with in the DealWithGameStatus() function.
 * A request is then sent to the sender to get their status.
 */	
void CScabbyQueenDealer::DealWithStatusFromRecv()
	{
	// We have got the status from receiver, deal with it
	StopTimer();
	iResendCount = 0;
	DealWithGameStatus(iGameStatus());
	// Now we want to tell the Sender to send their status
	iGameStatus = EGameStatusRequest;
	iSendMode = ESendCheckSenderStatus;
	BaseSendTo(iGameStatus, iRemoteNames[iSender]);
	StartTimer(KMaxSendTime);
	}
	
	
/**
 * Function that will send a request to the receiver asking for their game status.
 */	
void CScabbyQueenDealer::CheckPlayerStatus()
	{
	// Tell the Receiver that we want their game status
	StopTimer();
	iGameStatus = EGameStatusRequest;
	iSendMode = ESendCheckRecvStatus;
	BaseSendTo(iGameStatus, iRemoteNames[iPlayerToken]);
	StartTimer(KMaxSendTime);
	}


/**
 * Function called after the receipt of a game status from a player.
 * The game status is dealt with by placing the player number into an array of either
 * finished or current players. When both player have responded then we can construct
 * an update descriptor to send to all players.
 */
void CScabbyQueenDealer::DealWithGameStatus(TGameStatus aGameStatus)
	{

	if (iResponseCount == 0)
		{
		// First response, reset Update
		iCurrentPlayers.Reset();
		iPlayerUpdate.Delete(0,iPlayerUpdate.Length());
		}
		
	iResponseCount++;
	
	// Check the enum status that we have received, and append the
	// appropriate player number to the appropriate array.
	switch(aGameStatus)
		{
		case EPlayingZero:
			iCurrentPlayers.Append(0);
		break;
		
		case EPlayingOne:
			iCurrentPlayers.Append(1);
		break;
		
		case EPlayingTwo:
			iCurrentPlayers.Append(2);	
		break;
		
		case EPlayingThree:
			iCurrentPlayers.Append(3);		
		break;
		
		case EPlayingFour:
			iCurrentPlayers.Append(4);		
		break;
		
		case EPlayingFive:		
			iCurrentPlayers.Append(5);
		break;
	
		case EPlayingSix:		
			iCurrentPlayers.Append(6);
		break;
		
		case EPlayingSeven:		
			iCurrentPlayers.Append(7);
		break;
	
		case EFinishedZero:		
			iFinishedPlayers.Append(0);
		break;

		case EFinishedOne:		
			iFinishedPlayers.Append(1);
		break;
		
		case EFinishedTwo:		
			iFinishedPlayers.Append(2);
		break;
		
		case EFinishedThree:		
			iFinishedPlayers.Append(3);
		break;
	
		case EFinishedFour:		
			iFinishedPlayers.Append(4);
		break;
	
		case EFinishedFive:		
			iFinishedPlayers.Append(5);
		break;

		case EFinishedSix:		
    		iFinishedPlayers.Append(6);
		break;
		
		case EFinishedSeven:		
			iFinishedPlayers.Append(7);
		break;
		
		default:
		break;
		}
			
	if (iResponseCount == 2)// heard back from both
		{
		if (iOldPlayerUpdate == KNullDesC8)// first time 
			{
			// as this is the first time we can construct the
			// update descriptor as follows
			// append any finished players
			for (TInt i=0;i<iFinishedPlayers.Count();i++)
				{
				iPlayerUpdate.AppendNum(iFinishedPlayers[i]);
				}
			// append the F that seperates In and Out Players.
			iPlayerUpdate.Append('F');
			
			// append the players that are still in
			for (TInt i=0;i<iCurrentPlayers.Count();i++)
				{
				iPlayerUpdate.AppendNum(iCurrentPlayers[i]);
				}
				
			// append all the players that didnt do nothing
			for (TInt i=0;i<iRemoteNames.Count();i++)
				{ 
				if (i == iPlayerToken || i == iSender)
					{	
					}
				else
					{
					iPlayerUpdate.AppendNum(i);
					}
				} 
			}
		else
			{

			// as oldupdate contains something then this isn't the first time
			// a card has been exchanged we need to inspect the finished array.
			// for each of the values in the array we need to check whether it is to the left of the 
			// F (we already know that they have finished) if it isn't then we need to add it.
			// and remove it from the right of the F (these are the current players)
			
			
			TInt index = 0;
			// locate where the F is
			while (iOldPlayerUpdate[index] != 'F')
				{
				index++;
				}
				
			// index is position of F
			// get a pointer to finished players
			TPtrC8 playersOut(iOldPlayerUpdate.Mid(0, index));


			for (TInt i=0; i<iFinishedPlayers.Count();i++)
				{
				// i is a finished player, check that we have this number to the left of the F
				// if we dont then add it.
				TBuf8<1> num;
				num.AppendNum(iFinishedPlayers[i]);
				TInt loc = playersOut.Find(num);// has num finished already
				if (loc == KErrNotFound)
					{
					// we have finished with i but it isnt in the finished descriptor list
					iPlayerUpdate.AppendNum(iFinishedPlayers[i]);
					TInt match = iOldPlayerUpdate.Find(num);
					if (match != KErrNotFound)
						{
						iOldPlayerUpdate.Delete(match, 1);
						}
					}
				}
			iPlayerUpdate.Insert(0, playersOut);
			index = 0;
			// locate where the F is
			while (iOldPlayerUpdate[index] != 'F')
				{
				index++;
				}
			iOldPlayerUpdate.Delete(0, index);
			}
	

		if (iOldPlayerUpdate.Length() == 2)//  we only have one char to the right of the F
			{
			// we have our loser. hopefully they will have the scabby queen!
			// instead of sending a ready inquiry send a notification that tells the user game has finished.
			iGameOver = ETrue;
			}
			
		iPlayerUpdate.Append(iOldPlayerUpdate);
		iOldPlayerUpdate.Copy(iPlayerUpdate);
		TBufC16<20> buffer;
		buffer.Des().Copy(iPlayerUpdate);
		iConsole.Printf(_L("Update : %S"), &buffer);
		
		iResponseCount=0;
		iCount = 0;
		SendReadyInquiry();
		}
	}
	
/**
 * Function called if the timer completes, if this is the case then  timeout has occured 
 * in a transaction, we try to send again.
 */	
void CScabbyQueenDealer::TimerCompleted()
	{
	// the ack timer has complete before we have received the ack
	iConsole.Printf(_L("\nTimed out! Informing players of Game Over"));
	iConsole.Printf(_L("\nFatal Error, exiting game"));
	BaseCancelSendTo();
	InformNextGameOver();
	}


/**
 * Function that is called after the send of an update size has been sent.
 * We get ready to recv a response from the player and then move onto inform the next.
 */	
void CScabbyQueenDealer::ReadyInquirySent(TInt aError)
	{
	// we have sent a size update
	if (aError == KErrNone)
		{
		iRecvMode = ERecvReadyResult;
		iCheckSize.Create(iGameStatus);
		BaseRecvFrom(iGameStatus);
		iCheckSize.Close();
		iCount++;
		}
	else
		{
		// send has failed.
		iConsole.Printf(_L("\nFatal Error, exiting game"));
		BaseCancelSendTo();
		InformNextGameOver();
		}

	}


/**
 * Function that will notify the player of the size of the update that they will
 * receive.
 */	
void CScabbyQueenDealer::SendReadyInquiry()
	{
	StopTimer();
	if (iCount < iRemoteNames.Count())
		{
		iSendMode = ESendReadyInquiry;
		iUpdateSize = iPlayerUpdate.Length();// this is the update size
		BaseSendTo(iUpdateSize, iRemoteNames[iCount]);
		StartTimer(KMaxSendTime);
		}
	else
		{
		// all are ready, move on to send update
		iCount = 0;
		SendPlayerUpdate();
		}
	}

/**
 * Function that will send the Player update information to a player.
 * this will be called for each player.
 */	
void CScabbyQueenDealer::SendPlayerUpdate()
	{// here we need to a send update	
	StopTimer();
	BaseCancelSendTo();
	if (iCount < iRemoteNames.Count())
		{
		iSendMode = ESendPlayerUpdate;
		BaseSendTo(iPlayerUpdate, iRemoteNames[iCount]);
		StartTimer(KMaxSendTime);
		}
	else
		{// all are ready, move on to send update
			//DONE!
		if (iGameOver)
			{
			iConsole.Printf(_L("\n		GAME OVER"));
			}
		else
			{
			iCount = 0;
			iPlayerToken++;// player token is on reciever always!!
			InformStaticPlayers();//START OVER
			}
		}
	}

/**
 * Function called when a player update has been sent out. We wait to recv a response
 * so that we can move on to the next.
 */
void CScabbyQueenDealer::PlayerUpdateSent(TInt aError)
	{
	if (aError == KErrNone)
		{
		iRecvMode = ERecvUpdateResult;
		iCheckSize.Create(iGameStatus);
		BaseRecvFrom(iGameStatus);
		iCheckSize.Close();
		iCount++;
		}
	else
		{
		// send has failed.
		StopTimer();
		BaseCancelSendTo();
		iConsole.Printf(_L("\nFatal Error, exiting game"));
		InformNextGameOver();
		}
	}
	
/**
 * Function that is called after we have sent a game over message to a player.
 * Call the functuion that will send the next game over message.
 */
void CScabbyQueenDealer::SentGameOverMessage()
	{
	// we have sent a message to a player, move onto next
	iConsole.Printf(_L("\nPlayer %d has left the game"), iCount);
	InformNextGameOver();
	}
