// player.cpp
//
// Copyright (c) Symbian Software Ltd 2006.  All rights reserved.
//

#include <e32cons.h>
#include <es_sock.h>

#include "player.h"
#include "cardgameplayer.h"


/**
 * CScabbyQueenPlayer NewL.
 */
CScabbyQueenPlayer* CScabbyQueenPlayer::NewL(CConsoleBase& aConsole, RSocket& aSocket)
	{
	CScabbyQueenPlayer* self = new CScabbyQueenPlayer(aConsole, aSocket);
	self->ConstructL();
	return self;
	}


/**
 * Primary constructor.
 */
CScabbyQueenPlayer::CScabbyQueenPlayer(CConsoleBase& aConsole, RSocket& aSocket)
						:iConsole(aConsole)
					
	{
	CCardGamePlayer::ConstructL(aSocket);
	}

/**
 * Destructor.
 */	
CScabbyQueenPlayer::~CScabbyQueenPlayer()
	{
	iHand.Close();
	}

/**
 * 2nd constructor.
 */	
void CScabbyQueenPlayer::ConstructL()
	{
	iHand.Create(KDeckBufferLength);
	iRecvMode = EPlayerRecvNum;
	iCheckSize.Create(iPlayerNum);
	BaseRecvFrom(iPlayerNum);
	iCheckSize.Close();
	iGameOverBuffer = KErrCompletion;
	}
	
void CScabbyQueenPlayer::RecvOfPlayerNumComplete()
	{
	iRecvMode = EPlayerRecvHand;
	BaseRecvFrom(iHand);
	StartTimer(KMaxRecvTime);
	}


void CScabbyQueenPlayer::SendComplete(TInt aError)
	{
	switch (iSendMode)
		{
		case ESendReadyForUpdate:
			SendReadyForUpdateComplete();
		break;
		
		case ESendRecvTokenSuccess:
			SentTokenRecvSuccess();
		break;
		
		case ESendCardRecvNotification:
			SentCardRecvNotification();
		break;
		
		case ESendGameStatus:
			SentGameStatus();
		break;
		
		case ESendReadyForToken:
			SentReadyForToken();
		break;
		
		case ESendCardNumber:
			SendOfCardNumberComplete();
		break;
		
		case ESendOwnHandSize:
			SendOfHandSizeComplete();
		break;
		
		case ESendPlayerCard:
			SendCardComplete();
		break;
			
		case EPlayerSendAckSendCard:
			SendCard();
		break;
			
		case EPlayerSendAckRecvHandSize:
			PrepareToSendCard();
		break;
			
		case EPlayerSendAckRecvCard:
			RecvOfCardComplete();
		break;
		
		case EPlayerSendAckRecvWait:
			AckSentAfterRecvWait(aError);
		break;
		default:
		break;
		}
	}
		

void CScabbyQueenPlayer::RecvComplete(TInetAddr /*aRecvAddr*/, TInt /*aError*/)
	{

	switch (iRecvMode)
		{
		case EPlayerRecvNum:
			RecvOfPlayerNumComplete();
		break;
		
		case EPlayerRecvHand:
			RecvOfHandComplete();
		break;
		
		case EPlayerRecvStatus:
			RecvOfPlayTokenComplete();
		break;
		
		case EGameStatus:
			SendGameStatus();
		break;
		
		case EPlayerUpdate:
			ReceiveNextRole();
		break;
		
		case EDealerReady:
			RecvUpdate();
		break;
		
		case ERecvHandSize:
			RecvHandSize();
		break;
		
		case ERecvCardNum:
			SendCard();
		break;
		
		case ERecvCard:
			RecvOfCardComplete();
		break;

		default:
			break;
		}
	}
	

/**
 * Function called after player receives their hand from the dealer.
 * The hand is then looked through to remove any pairs.
 */
void CScabbyQueenPlayer::RecvOfHandComplete()
	{
	StopTimer();
	if (iHand == iGameOverBuffer)
		{
		// We haven't recieved our hand but a game over message.
		iConsole.Printf(_L("\nGAME OVER"));
		}
	else
		{
		TInt handLength = iHandLength();
		iHand.SetLength(handLength);
		iConsole.Printf(_L("\n%d cards received."), handLength/2);
		FindPairsAndRemove();
		//Wait to receive notification of what we are doing (token)
		iRecvTimerSet = ETrue;
		iRecvMode = EPlayerRecvStatus;
		iCheckSize.Create(iStatusBuffer);
		BaseRecvFrom(iStatusBuffer);
		iCheckSize.Close();
		// we should now have our hand
		}

	}
	


/**
 * Function called when player has received a token from the 
 * dealer stating what role the player will have this time. This role could either be EReceiving,
 * ESending or EWaiting.
 */	
void CScabbyQueenPlayer::RecvOfPlayTokenComplete()
	{
	// We have been called because we have been told that we are the current player
	StopTimer();
	if (iStatusBuffer == iGameOverBuffer)
		{
		// We haven't recieved our hand but a game over message.
		iConsole.Printf(_L("\nGAME OVER"));
		}
	else
		{
		iCurrentStatus = iStatusBuffer();
		switch (iCurrentStatus)
			{
			case EWaiting:
				iConsole.Printf(_L("\nYou are not in play this time"));
				// need to wait to recv size of the update, then continue as with 
				// the other two players.
				iRecvMode = EDealerReady; // this will then call RecvUpdate
				iCheckSize.Create(iUpdateSize);
				BaseRecvFrom(iUpdateSize);
				iCheckSize.Close();
				// Send Ack back to dealer
				iAck = KErrNone;
				iSendMode = EPlayerSendAckRecvWait;
				BaseSendTo(iAck, KDealerIpAddr);
			break;
			
			case EReceiving:
				{
				// we need to set card reciever
				iAck = KErrNone;
				iSendMode = ESendRecvTokenSuccess;
				BaseSendTo(iAck, KDealerIpAddr);
				}
			break;
			
			case ESending:
				{
				iConsole.Printf(_L("\nYou are not in play this time"));
				SendHandSize();
				}
			break;
			
			default:
			break;
			}
		}
	}



/**
 * Function that will send this players hand size to the dealer.
 * The player should receive an ack back from the dealer when they have received 
 * the hand size.
 */	
void CScabbyQueenPlayer::SendHandSize()
	{
	// send the size of our hand
	iSendMode = ESendOwnHandSize;
	iOwnHandSize = iHand.Length();
	BaseSendTo(iOwnHandSize, KDealerIpAddr);
	}



/**
 * The dealer has ack'd that they have received the hand size.
 * Get ready to receive a card number from the dealer. This will
 * be the card number that the receiving player has requested.
 */
void CScabbyQueenPlayer::SendOfHandSizeComplete()
	{
	// they have recv'd our hand size, proceed and setup recv of card num 
	iRecvMode = ERecvCardNum;
	iCheckSize.Create(iCardNum);
	BaseRecvFrom(iCardNum);
	iCheckSize.Close();
	}
	

/**
 * Function that will be called when the ack timer completes.
 * If this is the case then we have experienced a timeout.
 * Try to send again.
 */
void CScabbyQueenPlayer::TimerComplete()
	{
	iConsole.Printf(_L("\nTimed out!"));
	iConsole.Printf(_L("\nWaited too long to hear from the dealer, sorry."));
	}

/**
 * Function called after we have told the dealer that we have
 * received the player token successfully. Get ready to receive the hand size 
 * from the other player.
 */
void CScabbyQueenPlayer::SentTokenRecvSuccess()
	{
	// Get ready to receive hand size
	// We don't care if there was an error in sending because the 
	// Dealer will inform us next time of a Game Over
	iRecvMode = ERecvHandSize;
	iCheckSize.Create(iPeerHandSize);
	BaseRecvFrom(iPeerHandSize);
	iCheckSize.Close();
	}

/**
 * Function called when the player has received the hand size from the
 * dealer. Send an Ack back to the dealer.
 */
void CScabbyQueenPlayer::RecvHandSize()
	{
	if (iPeerHandSize == iGameOverBuffer)
		{
		// We have receved a game over message
		iConsole.Printf(_L("\nGAME OVER"));
		}
	else
		{
		iSendMode = EPlayerSendAckRecvHandSize;
		iAck = KErrNone;
		BaseSendTo(iAck, KDealerIpAddr);
		// now we will go back to the menu and we will give the option to send a card number
		}

	}

/**
 * Function called when the player has sent an ack to the dealer. This means that we
 * are the receiving player.
 */	
void CScabbyQueenPlayer::PrepareToSendCard()
	{
	// We have sent the ack to say we have receive the peer hand size.
	iCurrentPlayer = ETrue;
	iConsole.Printf(_L("\nPress any key..."));
	}


/**
 * Function that displays the current hand.
 */	
void CScabbyQueenPlayer::ShowHand()
	{
	TInt cardNum;
	iConsole.Printf(_L("\nYour hand...\n"));
	for (TInt i=0; i<=iHand.Length()-2; i=i+2)
		{
		if (i==0)
			cardNum = i;
		else
			cardNum = i/2;
		HBufC16* buffer = HBufC16::NewL(4);
		CleanupStack::PushL(buffer);
		buffer->Des().Copy(iHand.Mid(i,2));
		iConsole.Printf(_L("%d <%S>|"),cardNum,buffer);
		CleanupStack::PopAndDestroy(buffer);
		}
	iConsole.Getch();
	}

/**
 * Function that will look through the players hand and remove any pairs of
 * cards that exist. e.g. 4D and 4C.
 */	
void CScabbyQueenPlayer::FindPairsAndRemove()
	{
	TInt lastCardIndex = iHand.Length();
	while (lastCardIndex >= 4)
		{
		lastCardIndex = lastCardIndex-KCardLength;
		TPtrC8 ptrWholeCard = iHand.Mid(lastCardIndex, KCardLength);// pointer to last card
		TPtrC8 ptr = iHand.Mid(lastCardIndex);// pointer to value of last card
		TInt result = iHand.Locate(ptr[0]);// search the hand for duplicate
		if (result == lastCardIndex)
			{
			// This card is unique
			// no match was found, dont throw away
			}
		else
			{
			// we have a match!
			iHand.Delete(lastCardIndex, KCardLength);// delete the last card
			iHand.Delete(result, KCardLength);// delete the card that matched it
			// we have removed an extra card, index of last will now be 2 less
			lastCardIndex = lastCardIndex-KCardLength;
			}
		}

	}

/**
 * Function that displays the cards available for taking to the receiver.
 */	
void CScabbyQueenPlayer::GetRightHandPlayerCard()
	{
	TInt numOfCards = iPeerHandSize()/2;
	iConsole.Printf(_L("Select a card between 0 - %d: "), numOfCards-1);
	}

	
/**
 * When a player is selected to be a receiver this
 * function will send the card number that was selected by the user.
 */	
TInt CScabbyQueenPlayer::SendCardNum(TInt aCardNum)
	{
	
	if (aCardNum >= iPeerHandSize())
		{
		iConsole.Printf(_L("\nInvalid selection, try again numb nuts"));
		return (KErrTooBig);	
		}
	else
		{
		// Get ready to recv 
		iCardNum = aCardNum;
		iSendMode = ESendCardNumber;
		BaseSendTo(iCardNum, KDealerIpAddr);
		}

	return (KErrNone);
	}
	

/**
 * Function that is called after the send of a card number has completed,
 * the next thing to do is to get ready to receive the card from sender.
 */	
void CScabbyQueenPlayer::SendOfCardNumberComplete()
	{
	// now recv card
	// This function is called as soon as Send of card num to the dealer
	// has completed. We wont error check as dealer will tell us if game is over
	iRecvMode = ERecvCard;
	BaseRecvFrom(iReceivedCard);
	}
	


/**
 * Function that is called after the player has received the card from
 * the dealer. We want to add that card to the hand and check for pairs.
 * Then send a response to the dealer to say that the player has received the card.
 */	
void CScabbyQueenPlayer::RecvOfCardComplete()
	{
	if (iReceivedCard == iGameOverBuffer)
		{
		// its true. the game is over.
		BaseCancelRecvFrom();
		iConsole.Printf(_L("\nDealer Says: GAME OVER"));
		}
	else
		{
		iConsole.Printf(_L("Recevied Card"));
		iCurrentPlayer = EFalse;
		iHand.Append(iReceivedCard);//add card to hand
		FindPairsAndRemove();// Check for pairs
		if (iHand.Length() == 0)
			iFinished = ETrue;
		// Tell the dealer that we have received the card
		// This is the ack that they are expecting
		iSendMode = ESendCardRecvNotification;
		iStatusBuffer = EReceivedCard;
		BaseSendTo(iStatusBuffer, KDealerIpAddr);
		}
	}


/**
 * Function called after the player has sent the ack to the dealer
 * stating that we have received the card. Get ready to recv the request to send this
 * players game status to the dealer.
 */
void CScabbyQueenPlayer::SentCardRecvNotification()
	{
	// recv request to send our game status
	iRecvMode = EGameStatus;
	iCheckSize.Create(iGameStatus);
	BaseRecvFrom(iGameStatus);
	iCheckSize.Close();
	}


/**
 * Function is called after the player receives the update size from the 
 * dealer. the update buffer is set to the correct size and then the dealer
 * is notified that the player is ready to receive the update.
 */
void CScabbyQueenPlayer::RecvUpdate()
	{
	if (iUpdateSize == iGameOverBuffer)
		{
		// We were expecting an update size but got a game over
		iConsole.Printf(_L("\nDealer Says: GAME OVER"));
		}
	else
		{
		iPlayerUpdate.SetLength(iUpdateSize());// set size of update to size just recv from dealer
		iGameStatus = EReadyForUpdate;
		iSendMode = ESendReadyForUpdate;
		BaseSendTo(iGameStatus, KDealerIpAddr);
		}

	}


/**
 * Function called after the Send of the notify that the player is ready
 * to receive the update, get ready to recv the update.
 */
void CScabbyQueenPlayer::SendReadyForUpdateComplete()
	{
	// Now we have said we are read for the update, get ready to receive it!
	iRecvMode = EPlayerUpdate;
	BaseRecvFrom(iPlayerUpdate);
	}


/**
 * Function that send this player current status to the dealer
 */
void CScabbyQueenPlayer::SendGameStatus()
	{

	// Called after we have received game status request from dealer
	// Decide which player we are and send the necessary game status
	if (iGameStatus() == EGameStatusRequest)
		{
		switch (iPlayerNum())
			{
			case 0:
				if (iFinished)
					iGameStatus = EFinishedZero;
				else
					iGameStatus = EPlayingZero;
			break;
			
			case 1:
				if (iFinished)
					iGameStatus = EFinishedOne;
				else
					iGameStatus = EPlayingOne;
			break;
				
			case 2:
				if (iFinished)
					iGameStatus = EFinishedTwo;
				else
					iGameStatus = EPlayingTwo;
			break;
				
			case 3:
				if (iFinished)
					iGameStatus = EFinishedThree;
				else
					iGameStatus = EPlayingThree;
			break;
				
			case 4:
				if (iFinished)
					iGameStatus = EFinishedFour;
				else
					iGameStatus = EPlayingFour;
			break;
			
			case 5:
				if (iFinished)
					iGameStatus = EFinishedFive;
				else
					iGameStatus = EPlayingFive;
			break;
				
			case 6:
				if (iFinished)
					iGameStatus = EFinishedSix;
				else
					iGameStatus = EPlayingSix;
			break;

			case 7:
				if (iFinished)
					iGameStatus = EFinishedSeven;
				else
					iGameStatus = EPlayingSeven;
			break;
			}
		iSendMode = ESendGameStatus;			
		BaseSendTo(iGameStatus, KDealerIpAddr);
		}
		
	else if (iGameStatus == iGameOverBuffer)
		{
		iConsole.Printf(_L("\nDealer Says: GAME OVER"));
		}
	}
	

/**
 * Function called after the player has sent their game status to the dealer.
 * The player now gets ready to receive the size of the update 
 */	
void CScabbyQueenPlayer::SentGameStatus()
	{
	iRecvMode = EDealerReady; // this will then call RecvUpdate
	iCheckSize.Create(iUpdateSize);
	BaseRecvFrom(iUpdateSize);
	iCheckSize.Close();
	}
	
/**
 * After the player has received the update after the last iteration of the game.
 * We want to display the current game status, if there is only one player remaining then
 * the game has finished.
 */	
void CScabbyQueenPlayer::ReceiveNextRole()
	{
	if (iPlayerUpdate == iGameOverBuffer)
		{
		iConsole.Printf(_L("\nDealer Says: GAME OVER"));
		}
	else
		{
		// Break down the update descriptor and display the result
		TBufC<20> buffer;
		buffer.Des().Copy(iPlayerUpdate);	
		iConsole.Printf(_L("\n\nUpdate from the dealer...%S\n"), &buffer);
		TInt position;
		TPtrC8 ptr(iPlayerUpdate);
		TLex8 lex;
		TInt player;
		TBool stillIn = EFalse;
		iConsole.Printf(_L("\n	Number of players: %d\n"), iPlayerUpdate.Length()-1);
	
		TInt stillInCount = 0;
		for (TInt i=0; i<iPlayerUpdate.Length();i++)
			{
			TChar inspect = ptr[i];
		
			if (inspect != 'F')
				{
				TBuf8<2> buff;
				buff.Append(inspect);
				lex.Assign(buff);
				lex.Val(player);
				
				if (stillIn)
					{
					TBufC16<2> buff;
					buff.Des().Append(inspect);
					iConsole.Printf(_L("\n 	Player %S still in play"), &buff);
					stillInCount++;
					}
				else
					{
					position = i+1;
					if (i == 0)
						{
						iConsole.Printf(_L("\n The winner is player %d"), player);
						}
					else
						{
						iConsole.Printf(_L("\n 	In position %d is player %d"), position, player);
						}
					}		
				}
			else 
				{
				stillIn = ETrue;
				}
			}
	
		if (stillInCount == 1)
			{
			// If there is only one player left the game is over.
			iGameOver = ETrue;
			iConsole.Printf(_L("\nThe loser is player %d, give them a smack"), player);
			iConsole.Printf(_L("\n\nDealer Says: GAME OVER"));
			}
		iGameStatus = EReadyForToken;
		iSendMode = ESendReadyForToken;
		BaseSendTo(iGameStatus, KDealerIpAddr);
		}
	
	}


/**
 * Function called after the player has told the dealer that they are ready to 
 * to receive the next player token.
 */
void CScabbyQueenPlayer::SentReadyForToken()
	{
	// Told the dealer we are ready, setup Recv of our next status
	iRecvMode = EPlayerRecvStatus;
	iCheckSize.Create(iStatusBuffer);
	BaseRecvFrom(iStatusBuffer);
	iCheckSize.Close();
	}


/**
 * Function that will send a card from the players hand. After the sending player
 * has received the number of the card the receiver wants the sender responds.
 */
void CScabbyQueenPlayer::SendCard()
	{
	// we have ack'd the send of card num from the dealer.
		
	if (iCardNum == iGameOverBuffer)
		{
		// dealer has told us that we have lost a player
		// and that the game is over
		iConsole.Printf(_L("Dealer Says: GAME OVER"));
		}
	else
		{		
		// send the card requested
		iSendMode = ESendPlayerCard;
		TBuf8<2> buffer(iHand.Mid(iCardNum(),2));
		BaseSendTo(buffer, KDealerIpAddr);
		
		}

	}


/**
 * Function that send an ack to the dealer after the player has received a card.
 */	
void CScabbyQueenPlayer::SendAckAfterRecvCard()
	{
	//deler is expecting an ack to show that we have received the card
	iAck = KErrNone;
	iSendMode = EPlayerSendAckRecvCard;
	BaseSendTo(iAck, KDealerIpAddr);
	}
	
/**
 * Function that is called after the player has sent a card, dlete the card
 * from the players hand and ge ready to receive the request to send the game status
 * to the dealer.
 */	
void CScabbyQueenPlayer::SendCardComplete()
	{
	// Send of card has completed
	iHand.Delete(iCardNum(),2);// delete card from hand
	if (iHand.Length() == 0)
		iFinished = ETrue;
	iRecvMode = EGameStatus;// recv request to send our game status
	iCheckSize.Create(iGameStatus);
	BaseRecvFrom(iGameStatus);
	iCheckSize.Close();
	}
	
void CScabbyQueenPlayer::AckSentAfterRecvWait(TInt aError)
	{
	if (aError != KErrNone)
		{
		// The send of the ack has failed
		// GAME OVER
		BaseCancelRecvFrom();
		iConsole.Printf(_L("\nDealer Says: GAME OVER"));
		}
	}
	
