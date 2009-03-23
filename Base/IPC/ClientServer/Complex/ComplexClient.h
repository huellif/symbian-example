// ComplexClient.h
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.


#if !defined(__ComplexClient_H__)
#define __ComplexClient_H__

#include <e32base.h>

/**
RCountSession

The client-side handle to a session with the server,
The class forms a layer over the Symbian provided RSessionBase class.

The class deals with the requests made by the main example code by forwarding
them to the server. The type of request is identified by a code, one of
the TCountServRqst enum values, and arguments are passed via a TIpcArgs object.
*/
class RCountSession : public RSessionBase
	{
public:
	RCountSession();
	TInt Connect();
	TVersion Version() const;
	TInt ResourceCount();
	void Close();
private:
    RThread iServerThread;	
	};


/**
RCountSubSession

The class represents a subsession of the session represented
by RCountSession.
*/
class RCountSubSession : public RSubSessionBase
	{
public:
	TInt Open(RCountSession& aServer);
	TInt SetFromString(const TDesC& aString);
	void Close();
	void Increase();
	void Decrease();
	void IncreaseBy(TInt anInt);
	void DecreaseBy(TInt anInt);
	void Reset();
	TInt CounterValue();
	};


#endif

