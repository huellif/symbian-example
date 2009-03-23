// server.cpp
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.
//

// Transient server example - server implementation

#include "server.h"

inline CShutdown::CShutdown()
	:CTimer(-1)
	{CActiveScheduler::Add(this);}
inline void CShutdown::ConstructL()
	{CTimer::ConstructL();}
inline void CShutdown::Start()
	{After(KMyShutdownDelay);}

inline CMyServer::CMyServer()
	:CServer2(0,ESharableSessions)
	{}

inline CMySession::CMySession()
	{}
inline CMyServer& CMySession::Server()
	{return *static_cast<CMyServer*>(const_cast<CServer2*>(CSession2::Server()));}
inline TBool CMySession::ReceivePending() const
	{return !iReceiveMsg.IsNull();}


///////////////////////

void CMySession::CreateL()
//
// 2nd phase construct for sessions - called by the CServer framework
//
	{
	Server().AddSession();
	}

CMySession::~CMySession()
	{
	Server().DropSession();
	}

void CMySession::Send(const TDesC& aMessage)
//
// Deliver the message to the client, truncating if required
// If the write fails, panic the client, not the sender
//
	{
	if (ReceivePending())
		{
		TPtrC m(aMessage);
		if (iReceiveLen<aMessage.Length())
			m.Set(m.Left(iReceiveLen));
		TInt r=iReceiveMsg.Write(0,m);
		if (r==KErrNone)
			iReceiveMsg.Complete(KErrNone);
		else
			PanicClient(iReceiveMsg,EPanicBadDescriptor);
		}
	}

void CMySession::ServiceL(const RMessage2& aMessage)
//
// Handle a client request.
// Leaving is handled by CMyServer::ServiceError() which reports
// the error code to the client
//
	{
	switch (aMessage.Function())
		{
	case ESend:
		{
		TBuf<KMaxMyMessage> message;
		aMessage.ReadL(0,message);
		Server().Send(message);
		aMessage.Complete(KErrNone);
		break;
		}
	case EReceive:
		if (ReceivePending())
			PanicClient(aMessage,EPanicAlreadyReceiving);
		else
			{
			iReceiveMsg=aMessage;
			iReceiveLen=aMessage.Int1();
			}
		break;
	case ECancelReceive:
		if (ReceivePending())
			iReceiveMsg.Complete(KErrCancel);
		aMessage.Complete(KErrNone);
		break;
	default:
		PanicClient(aMessage,EPanicIllegalFunction);
		break;
		}
	}

void CMySession::ServiceError(const RMessage2& aMessage,TInt aError)
//
// Handle an error from CMySession::ServiceL()
// A bad descriptor error implies a badly programmed client, so panic it;
// otherwise use the default handling (report the error to the client)
//
	{
	if (aError==KErrBadDescriptor)
		PanicClient(aMessage,EPanicBadDescriptor);
	CSession2::ServiceError(aMessage,aError);
	}

void CShutdown::RunL()
//
// Initiate server exit when the timer expires
//
	{
	CActiveScheduler::Stop();
	}

CServer2* CMyServer::NewLC()
	{
	CMyServer* self=new(ELeave) CMyServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CMyServer::ConstructL()
//
// 2nd phase construction - ensure the timer and server objects are running
//
	{
	StartL(KMyServerName);
	iShutdown.ConstructL();
	// ensure that the server still exits even if the 1st client fails to connect
	iShutdown.Start();
	}


CSession2* CMyServer::NewSessionL(const TVersion&,const RMessage2&) const
//
// Cretae a new client session. This should really check the version number.
//
	{
	return new(ELeave) CMySession();
	}

void CMyServer::AddSession()
//
// A new session is being created
// Cancel the shutdown timer if it was running
//
	{
	++iSessionCount;
	iShutdown.Cancel();
	}

void CMyServer::DropSession()
//
// A session is being destroyed
// Start the shutdown timer if it is the last session.
//
	{
	if (--iSessionCount==0)
		iShutdown.Start();
	}

void CMyServer::Send(const TDesC& aMessage)
//
// Pass on the signal to all clients
//
	{
	iSessionIter.SetToFirst();
	CSession2* s;
	while ((s=iSessionIter++)!=0)
		static_cast<CMySession*>(s)->Send(aMessage);
	}

void PanicClient(const RMessagePtr2& aMessage,TMyPanic aPanic)
//
// RMessage::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
//
	{
	_LIT(KPanic,"MyServer");
	aMessage.Panic(KPanic,aPanic);
	}

static void RunServerL()
//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
	{
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(RThread::RenameMe(KMyServerName));
	//
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	//
	// create the server (leave it on the cleanup stack)
	CMyServer::NewLC();
	//
	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);
	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

TInt E32Main()
//
// Server process entry-point
//
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;
	}
