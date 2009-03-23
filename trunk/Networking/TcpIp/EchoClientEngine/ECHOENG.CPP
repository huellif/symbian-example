
// Copyright (c) 2000-2005 Symbian Software Ltd.  All rights reserved.
//
//
// Echoeng.cpp
//
// Defines CEchoEngine, CEchoRead, CEchoWrite, CTimeOutTimer

#include "echoeng.h"

// 50 seconds time-out
const TInt KTimeOut = 50000000; 
					 
/*
 CEchoEngine: main engine class for connection and shutdown
*/
EXPORT_C CEchoEngine::CEchoEngine() : CActive(EPriorityStandard)
	{
	}

EXPORT_C CEchoEngine* CEchoEngine::NewL(MUINotify* aConsole)
	{
	CEchoEngine* self = NewLC(aConsole);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CEchoEngine* CEchoEngine::NewLC(MUINotify* aConsole)
	{
	CEchoEngine* self = new(ELeave) CEchoEngine;
	CleanupStack::PushL(self);
	self->ConstructL(aConsole);
	return self;
	}
/*
 Constructs object, and opens a socket 
*/
EXPORT_C void CEchoEngine::ConstructL(MUINotify* aConsole)
	{
	iConsole = aConsole;
	iEngineStatus = EComplete;

	iTimeOut = KTimeOut; 
	iTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);
	CActiveScheduler::Add(this); 

	// Open channel to Socket Server
	User::LeaveIfError(iSocketServ.Connect());
	// Open a TCP socket
	User::LeaveIfError(iEchoSocket.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp));

	iEchoRead = new CEchoRead(&iEchoSocket, aConsole);
	iEchoWrite = CEchoWrite::NewL(&iEchoSocket, aConsole);
	}

void CEchoEngine::DoCancel()
// Cancel asychronous requests
	{
	iTimer->Cancel();

	// Cancel appropriate request to socket
	switch (iEngineStatus)
		{
	case EConnecting:
		iEchoSocket.CancelConnect();
		break;
	case ELookingUp:
		// Cancel look up attempt
		iResolver.Cancel();
		iResolver.Close();
		break;
	default:;
		}
	}
/*
 Connect to an Echo Socket by IP address	
*/
EXPORT_C void CEchoEngine::ConnectTo(TUint32 aAddr)
	{
	// port number for test purposes - may need to be changed
	iAddress.SetPort(7);
	iAddress.SetAddress(aAddr);
	iEchoSocket.Connect(iAddress, iStatus);
	iEngineStatus = EConnecting;
	SetActive();
	iTimer->After(iTimeOut);
	}

EXPORT_C void CEchoEngine::ConnectL(const TDesC& aServerName)
	// Connect to an Echo socket by hostname
	{
	// Initiate DNS
	User::LeaveIfError(iResolver.Open(iSocketServ, KAfInet, KProtocolInetUdp));
	// DNS request for name resolution
	iResolver.GetByName(aServerName, iNameEntry, iStatus);
	
	iEngineStatus=ELookingUp;
	// Request time out
	iTimer->After(iTimeOut);
	SetActive();
	}

EXPORT_C void CEchoEngine::TestGetByAddrL(TUint32 aAddr)
	// Looks up hostname, and then connects to an Echo socket 
	{
	// Initiate DNS
	User::LeaveIfError(iResolver.Open(iSocketServ, KAfInet, KProtocolInetUdp));
	// DNS request for address lookup
	iAddress.SetAddress(aAddr);
	iResolver.GetByAddress(iAddress, iNameEntry, iStatus);

	iEngineStatus=ELookingUp;
	// Request time out
	iTimer->After(iTimeOut);
	SetActive();
	}
/*
Exported function wrapping call to CEchoWrite: writes character to socket
*/
EXPORT_C void CEchoEngine::Write(TChar aChar)
	{
   	/* 
	In this simple implementation, if iEchoWrite is already active, ignore call.
	Full implementation would buffer data 
	*/
	if ((iEngineStatus == EConnected) && !iEchoWrite->IsActive())
		iEchoWrite->IssueWrite(aChar);
	}
/*
Exported function wrapping call to CEchoRead: reads character from socket
*/	
EXPORT_C void CEchoEngine::Read()
	{
	if ((iEngineStatus == EConnected)&&(!iEchoRead->IsActive())) 
		iEchoRead->IssueRead();
	}

/*
 Active object request complete handler.
 iEngineStatus flags what request was made, so its
 completion can be handled appropriately
*/
void CEchoEngine::RunL()
	{
	// Cancel TimeOut timer before completion
	iTimer->Cancel(); 
	_LIT(KConnecting,"\n<CEchoEngine> Connecting\n");
	_LIT(KConnectionFailed,"\n<CEchoEngine> Connection failed");
	_LIT(KDNSFailed,"\n<CEchoEngine> DNS lookup failed");
	_LIT(KTimedOut,"\n<CEchoEngine> Timed out\n");
	_LIT(KDomainName,"\nDomain name = ");
	_LIT(KIPAddress,"\nIP address = ");

	TBuf<15> ipAddr;

	switch(iEngineStatus)
		{
	case EConnecting:
		// IP connection request
		if (iStatus == KErrNone)
		// Connection completed sucessfully
			{
			iConsole->PrintNotify(KConnecting);
			iEngineStatus = EConnected;
           		//Start CEchoRead Active object 
			Read(); 
			}
		else
			{
			iEngineStatus = EConnectFailed;
			iConsole->ErrorNotifyL(KConnectionFailed, iStatus.Int());
			}
		break;
	case ETimedOut:
		// Timeout request
		iConsole->ErrorNotifyL(KTimedOut, KErrTimedOut);
		break;
	case ELookingUp:
		iResolver.Close();
		if (iStatus == KErrNone)
		// DNS look up successful
			{
			iNameRecord = iNameEntry();
			// Extract domain name and IP address from name record
			iConsole->PrintNotify(KDomainName);
			iConsole->PrintNotify(iNameRecord.iName);
			TInetAddr::Cast(iNameRecord.iAddr).Output(ipAddr);
			iConsole->PrintNotify(KIPAddress);
			iConsole->PrintNotify(ipAddr);
			// And connect to the IP address
			ConnectTo(TInetAddr::Cast(iNameRecord.iAddr).Address());
			}
		else
			{	
			iStatus = ELookUpFailed;
			iConsole->ErrorNotifyL(KDNSFailed, iStatus.Int());
			}
		break;

	default:;
		};
	}

CEchoEngine::~CEchoEngine()
	{
	delete iEchoRead;
	delete iEchoWrite;
	delete iTimer;
	iEchoSocket.Close();
	iSocketServ.Close();
	}

/*
 Implements MTimeOutNotify: called when timeout expired
*/
void CEchoEngine::TimerExpired()
	{
	Cancel();
	iEngineStatus = ETimedOut;
	TRequestStatus* p=&iStatus;		
	SetActive();
	User::RequestComplete(p, ETimedOut);
	}

/*
 Shutdown connection request
*/
EXPORT_C void CEchoEngine::Stop()
	{
	_LIT(KETerminate,"\n<CEchoEngine> Terminating\n");

	iConsole->PrintNotify(KETerminate);

	switch (iEngineStatus)
		{
	case EConnected:
		// Stop live connection
		iEchoRead->Cancel();
		iEchoWrite->Cancel();
		break;
	case EConnecting:
	case ELookingUp:
		// if request to CEchoEngine, then stop it
		Cancel();
		break;
	default:;
		}
	}


CEchoRead::CEchoRead(RSocket* aSocket, MUINotify* aConsole) 
:CActive(EPriorityStandard), iEchoSocket(aSocket), iConsole(aConsole)
	{
	CActiveScheduler::Add(this);
	}


/*
 Cancel asychronous read request
*/
void CEchoRead::DoCancel()
	{
	iEchoSocket->CancelRead();
	}
/*
 Active object request complete handler
*/
void CEchoRead::RunL()
	{
	if (iStatus == KErrNone)
		// Character has been read from socket
		{
		_LIT(KDot,".");
		iConsole->PrintNotify(KDot);
		TBuf16<1> Buffer;
		Buffer.Copy(iBuffer);
		iConsole->PrintNotify(Buffer);
		IssueRead();
		}
	else
		{
		// Error: pass it up to user interface
		_LIT(KCEchoReadError,"\nCEchoRead error");
		iConsole->ErrorNotifyL(KCEchoReadError, iStatus.Int());
		}	
	}
/*
 Read data from a stream socket
*/
void CEchoRead::IssueRead()
	{
	if (!IsActive())
		{
		iEchoSocket->Recv(iBuffer, 0, iStatus);
		SetActive();
		}
	}
/*
 This function is not actually used -
 It shows how to read data from a datagram socket 
*/
void CEchoRead::IssueRecvFrom(TInetAddr &aAddr)

	{
	iEchoSocket->RecvFrom(iBuffer,aAddr,NULL,iStatus);
	SetActive();
	};

/*
 CEchoWrite: active object wrapping comms write requests
*/
CEchoWrite::CEchoWrite() : CActive(EPriorityStandard)
	{

	};

CEchoWrite* CEchoWrite::NewL(RSocket* aSocket, MUINotify* aConsole)
	{
	CEchoWrite* self = NewLC(aSocket, aConsole);
	CleanupStack::Pop();
	return self;
	};

CEchoWrite* CEchoWrite::NewLC(RSocket* aSocket, MUINotify* aConsole)
	{
	CEchoWrite* self = new(ELeave) CEchoWrite;
	CleanupStack::PushL(self);
	self->ConstructL(aSocket, aConsole);
	return self;
	};

/*
ConstructL function - adds the active object to the Scheduler
*/	
void CEchoWrite::ConstructL(RSocket* aSocket, MUINotify* aConsole)
	{
	iEchoSocket = aSocket;
	iConsole = aConsole;
	CActiveScheduler::Add(this);

	iTimeOut = KTimeOut; 
	iTimer = CTimeOutTimer::NewL(10, *this);
	iWriteStatus = EWaiting;
	};

CEchoWrite::~CEchoWrite()
	{
	delete iTimer;
	}
/*
Cancels asychronous write request
*/
void CEchoWrite::DoCancel()
	{	
	iEchoSocket->CancelWrite();
	};
/*
 Implements MTimeOutNotify: called when timeout expired
*/
void CEchoWrite::TimerExpired()
	{
	Cancel();
	iWriteStatus = ETimedOut;
	TRequestStatus* p=&iStatus;		
	SetActive();
	User::RequestComplete(p, ETimedOut);
	}
/*
 Active object request complete handler
*/
void CEchoWrite::RunL()
	{
	if (iStatus == KErrNone)
		{
		_LIT(KWriteOperationTimedOut,"\nWrite operation timed out");
		switch(iWriteStatus)
			{
		// Character has been written to socket
		case ESending:
            // Cancel TimeOut timer
			iTimer->Cancel(); 
			iWriteStatus = EWaiting;
			break;
		// Request timed out
		case ETimedOut:
			iConsole->ErrorNotifyL(KWriteOperationTimedOut, KErrTimedOut);
			break;
		default:;
			};
		}
	else 
		{
		// Error: pass it up to user interface
		_LIT(KCEchoWriteError,"\nCEchoWrite error");
		iConsole->ErrorNotifyL(KCEchoWriteError, iStatus.Int());
		}
	}
/*
Write data to a stream socket
*/
void CEchoWrite::IssueWrite(const TChar &aChar)
	{
	// Set up buffer
	iBuffer.SetLength(0);
	iBuffer.Append(aChar);
	iEchoSocket->Write(iBuffer, iStatus);
	// Request timeout
	iTimer->After(iTimeOut);
	SetActive();
	iWriteStatus = ESending;
	};

void CEchoWrite::IssueSendTo(TInetAddr &aAddr, const TChar &aChar)
// This function is not actually used -
// it shows how to write data to a datagram socket 
	{
	// Set up buffer
	iBuffer.SetLength(0);
	iBuffer.Append(aChar);
	iEchoSocket->SendTo(iBuffer,aAddr,NULL,iStatus);
	iTimer->After(iTimeOut);
	SetActive();
	iWriteStatus = ESending;
	};

/*
 CTimeOutTimer: timer for comms time-outs
*/
CTimeOutTimer::CTimeOutTimer(const TInt aPriority)
    : CTimer(aPriority)
    {
    }

CTimeOutTimer::~CTimeOutTimer()
    {
	Cancel();
    }

CTimeOutTimer* CTimeOutTimer::NewL(const TInt aPriority, MTimeOutNotify& aTimeOutNotify)
    {
    CTimeOutTimer *p = new (ELeave) CTimeOutTimer(aPriority);
    CleanupStack::PushL(p);
	p->ConstructL(aTimeOutNotify);
	CleanupStack::Pop();
    return p;
    }
/**
* ConstructL function is used to add the active object to the scheduler 
*/
void CTimeOutTimer::ConstructL(MTimeOutNotify &aTimeOutNotify)
    {
	iNotify=&aTimeOutNotify;
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
    }
/**
 Timer request has completed, so notify the timer's owner
*/
void CTimeOutTimer::RunL()
  
    {
	iNotify->TimerExpired();
	}
	
