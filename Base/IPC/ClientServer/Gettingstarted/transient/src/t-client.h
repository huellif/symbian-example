// t-client.h
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.
//

// Transient server example - client interface

#ifndef __T_CLIENT_H__
#define __T_CLIENT_H__

#include <e32std.h>

class RMySession : public RSessionBase
	{
public:
	IMPORT_C TInt Connect();
	IMPORT_C TInt Send(const TDesC& aMessage);
	IMPORT_C void Receive(TRequestStatus& aStatus,TDes& aMessage);
	IMPORT_C void CancelReceive();
	};

#endif