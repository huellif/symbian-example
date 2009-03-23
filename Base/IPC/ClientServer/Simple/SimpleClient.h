// SimpleClient.h
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.
//
#if !defined(__COUNTSERV_H__)
#define __COUNTSERV_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif


//**********************************
//RCountServSession
//**********************************

// Our client-side handle to a session with the server.
// The class forms a layer over the Symbian provided RSessionBase class.
//
// The class deals with the requests made by the main example code by forwarding
// them to the server. The type of request is identified by a code, one of
// the TCountServRqst enum values, and arguments are passed via a TIpcArgs object.
//
// The functions Increase(), Decrease() etc are wrappers for different calls to 
// SendReceive().
//
// Most of the functions here return void; if they fail, the server panics the client.
// If they return it can be assumed that there is no error.
//
// The RThread object is a handle to the thread that is acting as the server.


class RCountServSession : public RSessionBase
	{
public:
	RCountServSession();
	TInt Connect();
	TVersion Version() const;
	TInt UnsupportedRequest();
	TInt SetFromString(const TDesC& aString);
	void Increase();
	void Decrease();
	void IncreaseBy(TInt anInt);
	void DecreaseBy(TInt anInt);
	void Reset();
	TInt CounterValue();
	void BadRequest();
	void Close();
private:
    RThread iServerThread;
	};


#endif

