// gameconstants.H
//
// Copyright (c) Symbian Software Ltd 2006.  All rights reserved.
//

#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H


#include <e32base.h>
#include <in_sock.h>


const TInt KMaxBufferSize = 128;// Is this used still?
const TInt KDeckLength = 49; // Number of cards
const TInt KDeckBufferLength = KDeckLength*2;// Length of deck descriptor for scabby queen
const TInt KCardLength = 2; // Descriptor length of an individual card

const TUint KGenericPort = 5123; // Random port for socket to bind to
const TUint32 KDealerIpAddr = INET_ADDR(11,11,11,1);// IP address of scabby dealer
const TInt KMaxSendTime=15000000;
const TInt KMaxRecvTime=120000000;

// The suits for scabby queen, omitting 3 queens
_LIT8(KClubs,    "AC2C3C4C5C6C7C8C9C0CJCKC");
_LIT8(KHearts,   "AH2H3H4H5H6H7H8H9H0HJHKH");
_LIT8(KSpades,   "AS2S3S4S5S6S7S8S9S0SJSQSKS");
_LIT8(KDiamonds, "AD2D3D4D5D6D7D8D9D0DJDKD");


// enum used by the dealer to inform a player of there state.	
enum TPlayerStatus 
	{
	EWaiting = 0,
	EReceiving,
	ESending,
	EReceivedCard
	};

// This is the enum used to send the current status of a player
// to the dealer.
enum TGameStatus
	{
	EPlayingZero = 0,
	EPlayingOne,
	EPlayingTwo,
	EPlayingThree,
	EPlayingFour,
	EPlayingFive,
	EPlayingSix,
	EPlayingSeven,
	EFinishedZero,
	EFinishedOne,
	EFinishedTwo,
	EFinishedThree,
	EFinishedFour,
	EFinishedFive,
	EFinishedSix,
	EFinishedSeven,
	EGameStatusRequest,
	EReady,
	EReadyInquiry,
	EReadyForUpdate,
	EReadyForToken,
	EGameOver
	};

#endif // GAMECONSTANTS_H


	