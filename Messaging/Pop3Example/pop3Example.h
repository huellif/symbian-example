// pop3Example.h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//
// @file
// Contains the Class CPop3Example.

#ifndef __POP3EXAMPLE_H__
#define __POP3EXAMPLE_H__

// epoc includes
#include <e32cons.h>
#include <mtclreg.h> 
#include "pop3set.h"
#include "popcmtm.h"
#include <mtclbase.h>

#include <IAPPrefs.h>
#include <cemailaccounts.h>

#include <es_sock.h>
#include <commdb.h>
#include <cdbpreftable.h>
#include <commdbconnpref.h>
#include <CommsDatTypesV1_1.h>
#include <commsdat.h>
using namespace CommsDat;

_LIT(LDD_NAME,"ECOMM");

#if defined (__WINS__)
_LIT(PDD_NAME,"ECDRV");
#else
_LIT(PDD_NAME,"ECUART1");
#endif

_LIT(KMailtestPath, "c:\\mailtest\\");
_LIT( KRfc822Dir,"c:\\mailtest\\rfc822\\");

// Create global variables
_LIT(KTitle, "pop3example" );
_LIT(KDataComponentFileName, "c:\\system\\mtm\\impc.dat" );

/**
A class that publicily inherits two classes, CBase and MMsvSessionObserver. 
CBase is the base class for all classes to be instantiated on the heap. 
MMsvSessionObserver is the interface for notification of events from a Message Server session.
Objects that need to observe a session view should implement this interface.
*/
class CPop3Example :public CBase, public MMsvSessionObserver
{
public:
	static CPop3Example* NewL();
	~CPop3Example();
	// Overriden function of session event.
	//  Session observer, reports whatever notification it receives.
	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	void CreateClientRegistryL();
	void CreatePopAndSmtpAccountL();
	void CreateSmtpMessage();
	void SendSMTPMessage();
	void ConnectDownloadAndDisconnectPOPServerL();
	
private:

	CPop3Example();
	void ConstructL();
	/** Pointer to the console interface*/
	CConsoleBase* 		iConsole;
	/** Pointer to the Client-side MTM registry*/
	CClientMtmRegistry* iClientRegistry;
	/** Pointer to the channel of communication between a client thread and the Message Server thread*/
	CMsvSession*		iSession;
	/** Pointer to the CBase MTM*/
	CBaseMtm*			iMtm;
	/** Pointer to the CMsvOperation to get progress information about the operation*/
	CMsvOperation*		iOperation;
	/** ID of the entry to access*/
	TMsvId 				iPopServiceId;
	/** POP account Name*/
	TPopAccount			iPopAccount;
	/** SMTP account Name*/
	TSmtpAccount		iSmtpAccount;


    };

#endif //__POP3EXAMPLE_H__
