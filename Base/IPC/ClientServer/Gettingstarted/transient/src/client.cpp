// client.cpp
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.
//

// Transient server example - client interface implementation

#include <t-client.h>
#include "clientserver.h"

static TInt StartServer()
//
// Start the server process. Simultaneous launching
// of two such processes should be detected when the second one attempts to
// create the server object, failing with KErrAlreadyExists.
//
	{
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);
	RProcess server;
	TInt r=server.Create(KMyServerImg,KNullDesC,serverUid);
	if (r!=KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

EXPORT_C TInt RMySession::Connect()
//
// Connect to the server, attempting to start it if necessary
//
	{
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KMyServerName,TVersion(0,0,0),1);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}

EXPORT_C TInt RMySession::Send(const TDesC& aMessage)
	{
	if (aMessage.Length()>KMaxMyMessage)
		return KErrArgument;
	return SendReceive(ESend,TIpcArgs(&aMessage));
	}

EXPORT_C void RMySession::Receive(TRequestStatus& aStatus,TDes& aMessage)
	{
	SendReceive(EReceive,TIpcArgs(&aMessage,aMessage.MaxLength()),aStatus);
	}

EXPORT_C void RMySession::CancelReceive()
	{
	SendReceive(ECancelReceive);
	}
