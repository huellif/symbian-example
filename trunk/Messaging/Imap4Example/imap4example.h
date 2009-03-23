// imap4example.h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef __IMAP4EXAMPLE_H__
#define __IMAP4EXAMPLE_H__

// epoc include
#include <mtclreg.h> 
#include <cemailaccounts.h> 
#include <CommsDatTypesV1_1.h>
using namespace CommsDat;

_LIT(LDD_NAME,"ECOMM");

#if defined (__WINS__)
_LIT(PDD_NAME,"ECDRV");
#else
_LIT(PDD_NAME,"ECUART1");
#endif

/**
CImap4Example is a class that publicly inherits two 
classes, CBase and MMsvSessionObserver. 
CBase is the base class for all classes to be instantiated on the heap. 
MMsvSessionObserver is the interface for notification of events from a Message Server session.
Objects that need to observe a session view should implement this interface.
The class demonstrates the creation of client registry, smtp and imap accounts,
Smtp and imap messages, connection to email server and sending of the message using imap4 protocol.
*/
class CImap4Example :public CBase, public MMsvSessionObserver
	{
public:
	static CImap4Example* NewL();
	~CImap4Example();
	
	// Overriden function of session event.
	//  Session observer, reports whatever notification it receives.
	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	void CreateClientRegistryL();
	void CreateImapAndSmtpAccountL();
	void CreateSmtpMessageL();
	void SendSMTPMessageL();
	void ConnectAndCopyAllMailAndDisconnectL();

private:

	CImap4Example();
	void ConstructL();
	/** Pointer to the console interface */
	CConsoleBase* 		iConsole;
	/** Pointer to the Client-side MTM registry*/
	CClientMtmRegistry* iClientRegistry;
	/** Pointer to the channel of communication between a client thread and the Message Server thread*/
	CMsvSession*		iSession;
	/** Pointer to the CBase MTM*/
	CBaseMtm*			iMtm;
	/** Pointer to the CMsvOperation to get progress information about the operation*/
	CMsvOperation*		iOperation;
	/** Imap account Name*/
	TImapAccount		iImapAccount;
	/** Smtp account Name*/
	TSmtpAccount		iSmtpAccount;


    };

#endif //__IMAP4EXAMPLE_H__

