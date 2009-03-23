// SimpleServer.h
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

#include <e32base.h>

// needed for creating server thread.
const TUint KDefaultHeapSize=0x10000;

// reasons for server panic
enum TCountServPanic
	{
	EBadRequest = 1,
	EBadDescriptor,
	EMainSchedulerError,
	ESvrCreateServer,
	ESvrStartServer,
	ECreateTrapCleanup,
	ENotImplementedYet,
	};


//**********************************
//CCountServServer
//**********************************
/**
Our server class - an active object - and therefore derived ultimately from CActive.
It accepts requests from client threads and forwards
them to the client session to be dealt with. It also handles the creation
of the server-side client session.
*/
class CCountServServer : public CServer2
	{
public:
	  // Creates a new session with the server; the function
	  // implements the pure virtutal function 
	  // defined in class CServer2
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
public :
      // The thread function executed by the server
    static TInt ThreadFunction(TAny* aStarted);
      // Function to panic the server
    static void PanicServer(TCountServPanic aPanic);

protected:
	CCountServServer(CActive::TPriority aActiveObjectPriority);
	};


//**********************************
//CCountServSession
//**********************************
/**
This class represents a session with the  server.
Functions are provided to respond appropriately to client messages.
*/
class CCountServSession : public CSession2
	{
public:
	CCountServSession();

	  //service request
	void ServiceL(const RMessage2& aMessage);
	void DispatchMessageL(const RMessage2& aMessage);

	 //services available to initialize/increase/decrease/reset and return the counter value.
	void SetFromStringL(const RMessage2& aMessage);
	void Increase();
	void Decrease();
	void IncreaseBy(const RMessage2& aMessage);
	void DecreaseBy(const RMessage2& aMessage);
	void CounterValueL(const RMessage2& aMessage);
	void Reset();

protected:
	  // panic the client
	void PanicClient(const RMessage2& aMessage,TInt aPanic) const;
		
private:
	TInt iCount;
	};


