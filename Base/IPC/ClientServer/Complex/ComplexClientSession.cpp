// ComplexClientSession.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.


/**
Implementation of the RCountSession class
*/

#include "ComplexClientAndServer.h"
#include "ComplexClient.h"


// number of message slots.
const TUint kDefaultMessageSlots=4;

/**
Constructor
*/
RCountSession::RCountSession()
	{
	}


/**
Connects to the  server using 4 message slots.

In this example, the server is implemented as a separate thread.
The function starts that thread before attempting to create a session with the server.

The version information specifies the earliest version of the server that we can
talk to.
*/
TInt RCountSession::Connect()
	{
	TInt r=StartThread(iServerThread);
	if (r==KErrNone)
		r=CreateSession(KCountServerName,Version(),kDefaultMessageSlots);
	return(r); 
	}
	

/**
Returns the earliest version number of the server that we can talk to.
*/	
TVersion RCountSession::Version(void) const
	{
	return(TVersion(KCountServMajorVersionNumber,KCountServMinorVersionNumber,KCountServBuildVersionNumber));
	}


/**
A request to close the session.

It makes a call to the server, which deletes the object container and object index
for this session, before calling Close() on the base class.
*/
void RCountSession::Close()
	{
	SendReceive(ECountServCloseSession);
	RHandleBase::Close();
	iServerThread.Close();
	}


/**
A server request to get the number of subsessions
in this session.
*/
TInt RCountSession::ResourceCount()
	{
	TInt count=0;
	TPckgBuf<TInt> pckgcount;
	
	  // Note that TPckgBuf is of type TDes8
	TIpcArgs args(&pckgcount);
	SendReceive(ECountServResourceCount, args);
	
	  // Extract the value returned from the server. 
	count = pckgcount();
	return count;
	}



