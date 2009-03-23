// testclient.cpp
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.
//

// Transient server example - test client implementation

#include <e32base.h>
#include <t-client.h>
#include "testclient.h"

class CMyClient : public CClient
	{
	IMPORT_C static CClient* NewL();
	~CMyClient();
 	TInt Send(const TDesC& aMessage);
	void Receive(TRequestStatus& aStatus,TDes& aMessage);
	void CancelReceive();
private:
	RMySession iSession;
	};

EXPORT_C CClient* CMyClient::NewL()
	{
	CMyClient* self=new(ELeave) CMyClient;
	CleanupClosePushL(*self);
	User::LeaveIfError(self->iSession.Connect());
	CleanupStack::Pop();
	return self;
	}

CMyClient::~CMyClient()
	{
	iSession.Close();
	}

TInt CMyClient::Send(const TDesC& aMessage)
	{
	return iSession.Send(aMessage);
	}

void CMyClient::Receive(TRequestStatus& aStatus,TDes& aMessage)
	{
	iSession.Receive(aStatus,aMessage);
	}

void CMyClient::CancelReceive()
	{
	iSession.CancelReceive();
	}
