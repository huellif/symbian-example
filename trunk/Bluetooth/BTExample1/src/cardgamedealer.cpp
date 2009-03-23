// cardgamedealer.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#include <e32base.h>
#include <e32math.h>

#include "cardgamedealer.h"
#include "cardgamebase.h"

const TInt KCardDesLength = 2; // Descriptor length for a single card

/**
 * D'tor
 */
CCardGameDealer::~CCardGameDealer()
	{
	}

/**
 * Constructor, takes remote name array and calls base constructor.
 */	
void CCardGameDealer::ConstructL(RArray<TInetAddr>& aRemoteNames, RSocket& aSocket)
	{
	iRemoteNames = aRemoteNames;
	CCardGameBase::ConstructL(aSocket);
	}	

/**
 * A function that takes a deck of cards in the form of a descriptor
 * and shuffles the cards in a random manner.
 * 
 * @param aCards
 * Cards to be shuffled
 */	
void CCardGameDealer::ShuffleCards(TDesC8& aCards)
	{
    RArray<TInt> usedNumbers;// Array of used positions
	TBool oldNum = ETrue;
	TPtrC8 ptr;
	TInt random = 0;
	const TInt deckLength = aCards.Length();
	RBuf8 tempBuffer;
	tempBuffer.Create(deckLength);
	//Take first card (2chars) and put it into tempBuffer in random position
	// make a note of this pos as we can use it again. or just check if empty
	for (TInt i=0; i<=deckLength; i=i+KCardDesLength)
		{
		while (oldNum)
			{
			//generate random number between 0 and 96
			random = Math::Random() % deckLength-KCardDesLength;
			if (random <= deckLength-KCardDesLength && random >= 0)
				{
				TReal remainder;
				Math::Mod(remainder,random,KCardDesLength);
				if (remainder)// if true then we have an odd , make it into an even.
					random++;
				// now we have an even number check that we haven't used it before
				oldNum = EFalse;
				for (TInt j=0; j<usedNumbers.Count();j++)
					{
					if (random == usedNumbers[j])
						oldNum = ETrue;
					}
					
				if (!oldNum)
					{
					// We have a card that we haven't used before
					usedNumbers.Append(random);
					}
				}

			}
		
		
		ptr.Set(aCards.Mid(random,KCardDesLength));
		tempBuffer.Append(ptr);
		oldNum = ETrue;
		}
		
	//finished shuffle, now copy into full deck.
	aCards = tempBuffer;
	tempBuffer.FillZ();// clear the temp. buffer
	usedNumbers.Close();
	tempBuffer.Close();
	}

/**
 * A function that takes a deck of cards in the form of a descriptor,
 * and deals out the cards using the RSocket connection.
 * 
 * @param aCards
 * A pack of cards
 */	
void CCardGameDealer::DealCards(TDesC8& aCards)
	{
	TInt deckLength = aCards.Length();
	TInt playerCount = iRemoteNames.Count();
	// playerCount is used to divide deck.


	ShuffleCards(aCards);
	TInt handLength = (deckLength) / playerCount;
	TReal remainder;
	Math::Mod(remainder, deckLength, playerCount);
	// we want to give each player handlength *2 cards
	// then n(remainder) players one card extra each
	
	// send out a message to each player stating the size of recieve buffer they will need
	TRequestStatus status;
	TInt size;
	TInt startPoint = 0;
	for (TInt i=0;i<iRemoteNames.Count();i++)
		{
		if (remainder > 0)
			{
			// we want to set recieve buffer to handLength*2 + 2
			size = handLength*2 + 2;
			// Send pointer to portion of the deck
			iSocket.SendTo(aCards.Mid(startPoint,size), iRemoteNames[i], 0, status);
			User::WaitForRequest(status);
			User::LeaveIfError(status.Int());
			startPoint = size;// change start point to end of last hand sent out
			remainder--;
			}
		else
			{
			//we want to set the players recieve buffer to handlength * 2
			size = handLength*2;		
			iSocket.SendTo(aCards.Mid(startPoint,size), iRemoteNames[i], 0, status);
			User::WaitForRequest(status);
			User::LeaveIfError(status.Int());
			startPoint = size+startPoint;
			}
		}
	
	}
