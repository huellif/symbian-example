// testclient.h
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.
//

// Transient server example - test client interface

#include "plugin.h"

class CClient : public CPlugIn<CClient>
	{
public:
 	virtual TInt Send(const TDesC& aMessage) =0;
	virtual void Receive(TRequestStatus& aStatus,TDes& aMessage) =0;
	virtual void CancelReceive() =0;
protected:
	virtual ~CClient() {}
	};

