// ComplexServer.h
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

#if !defined(__ComplexServer_H__)
#define __ComplexServer_H__

//needed for creating server thread.
const TUint KDefaultHeapSize=0x10000;

// panic reasons
enum TCountServPanic
	{
	EBadRequest = 1,
	EBadDescriptor,
	EDescriptorNonNumeric,
	EMainSchedulerError,
	ESvrCreateServer,
	ESvrStartServer,
	ECreateTrapCleanup,
	EBadCounterRemove, 
	EBadSubsessionHandle 
	};




/*
CCountServer class

Represents the server.
	
The server starts with the first client connect call.
Start includes setting up active scheduler, the server active object,
and the object container index which produces object object containers for each session.
*/
class CCountServer : public CServer2
	{
public:
    

      // Creates a new session with the server; the function
	  // implements the pure virtutal function 
	  // defined in class CServer2
	CSession2* NewSessionL(const TVersion &aVersion,const RMessage2& aMessage) const;

public:	
      // Creats a new server object
    static CCountServer* NewL(CActive::TPriority aActiveObjectPriority);
    
      // The thread function executed by the server 
	static TInt ThreadFunction(TAny* aStarted);

	  // utility function to panic the server.
	static void PanicServer(TCountServPanic aPanic);

public :
      // Constructor
	CCountServer(CActive::TPriority aActiveObjectPriority);
	
	  // Second phase constructor
	void ConstructL();

	  // Returns an object container, and guaranteed 
	  // to produce object containers with unique
	  // ids within the server.
	  // Called by a new session to create a container 
	CObjectCon* NewContainerL();
    
	  // Destructor; exists to do some tidying up.
	~CCountServer();

private:
	  // The server has an object container index that
	  // creates an object container for each session.
	CObjectConIx* iContainerIndex; 
	};




/*
CCountSession class

Represents a session with the server.
	
Functions are provided to respond appropriately to client messages.
A session can own any number of subsession objects.
*/
class CCountSubSession;
class CCountSession : public CSession2
	{
public:
	  // Create the session
	static CCountSession* NewL();
	
public:
      // Constructor
	CCountSession();
	
	  // Called by client/server framework after 
	  // session has been successfully created
    void CreateL(); 
		
	  // Service request
	void ServiceL(const RMessage2& aMessage);
	void DispatchMessageL(const RMessage2& aMessage);

      // Creates new subsession
    void NewCounterL(const RMessage2& aMessage);  
      
      // Closes the session
	void CloseSession();
	
      // Gets the number of resources (i.e. CCountSubSession objects)
	void NumResourcesL(const RMessage2& aMessage);
	
	  // Utility to return the CCountSubSession (subsession) object
    CCountSubSession* CounterFromHandle(const RMessage2& aMessage,TInt aHandle);	

	  // Delete the subsession object through its handle.
	void DeleteCounter(TInt aHandle);
      
      // Gets the number of server-side subsession objects.
	TInt CountResources();
	  
	  // Panics client
	void PanicClient(const RMessage2& aMessage,TInt aPanic) const;

private:
      // Object container for this session.
	CObjectCon *iContainer;

	  // Object index which stores objects
	  // (CCountSubSession instances) for this session.
	CObjectIx* iCountersObjectIndex;

      // Total number of resources. In this example
      // a resource is just the number of CCountSubSession objects.
    TInt iResourceCount;
	};




/*
CCountSubSession class

Represents a subsession of CCountSession.
*/
class CCountSubSession : public CObject
	{
public:
      // creates a new CCountSubSession object.
	static CCountSubSession* NewL(CCountSession* aSession);
	
public: 
    CCountSubSession(CCountSession* aSession);	
	void ConstructL(CCountSession* aSession);
	void SetFromStringL(const RMessage2& aMessage);
	void Increase();
	void IncreaseBy(const RMessage2& aMessage);
	void Decrease();
	void DecreaseBy(const RMessage2& aMessage);
	void Reset();
	void CounterValueL(const RMessage2& aMessage);
   
protected:
      // The session that owns this CCountSubSession object.
	CCountSession* iSession;
	
private:
      // The counter value
 	TInt iCount;
	};

#endif
