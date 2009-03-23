// SimpleServer.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

// The implementation of our simple server that increments and decrements a simple counter value.


//
//**NOTE**: The example does not demonstrate any security features - its purpose is simply
//          to demonstrate the basic principles of client/server interaction.
//


#include "ClientServer.h"
#include "SimpleServer.h"
#include <e32svr.h>
#include <e32uid.h>


//**********************************
//CCountServServer - implementations
//**********************************

/**
Constructor takes the server priority value. 

The server is an active object, and the priority value is the priority
of this active object.

It passes the priority value to the base class in the Ctor list.
By default, the session is not sharable, which is what we want here
so no second parameter is passed to the CServer2 constructor.
*/
CCountServServer::CCountServServer(CActive::TPriority aActiveObjectPriority)
	: CServer2(aActiveObjectPriority)
	{
	}


/**
Creates a new session with the server.
*/
CSession2* CCountServServer::NewSessionL(const TVersion& aVersion,const RMessage2& /*aMessage*/) const
    {
      // Check that the version is OK
    TVersion v(KCountServMajorVersionNumber,KCountServMinorVersionNumber,KCountServBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		User::Leave(KErrNotSupported);	
	
	// CAN USE THE aMessage argument to check client's security and identity
	// can make use of this later but for now ignore. AH 4/5/05
	// the connect message is delivered via the RMessage2 object passed. 
	
	// do something with this later (and move it to the start of the function?)
	
	  // Create the session.
	return new (ELeave) CCountServSession;
	}


/**
A utility function to panic the server.
*/
void CCountServServer::PanicServer(TCountServPanic aPanic)
	{
	_LIT(KTxtServerPanic,"Count server panic");
	User::Panic(KTxtServerPanic,aPanic);
	}


//***********************************
//CCountServSession - implementations
//***********************************


/**
Constructor
*/
CCountServSession::CCountServSession()
	{
    }

/**
Services a client request.
*/
void CCountServSession::ServiceL(const RMessage2& aMessage)
	{
	TRAPD(err,DispatchMessageL(aMessage));
	aMessage.Complete(err);
	}

/**
Called by ServiceL()

It tests the function code and then delegates to
the appropriate function.
*/
void CCountServSession::DispatchMessageL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
        {
	case ECountServSetFromString:
		SetFromStringL(aMessage);
		return;
	case ECountServIncrease:
		Increase();
		return;
	case ECountServIncreaseBy:
		IncreaseBy(aMessage);
		return;
	case ECountServDecrease:
		Decrease();
		return;
	case ECountServDecreaseBy:
		DecreaseBy(aMessage);
		return;
	case ECountServReset:
		Reset();
		return;
	case ECountServValue:
		CounterValueL(aMessage);
		return;
	  
	  // This is an example of a request that we know about, but don't support.
	  // We cause KErrNotSupported to be returned to the client.
	case ECountServUnsupportedRequest:
		User::Leave(KErrNotSupported);
		
	 //  Requests that we don't understand at all are a different matter.
	 //  This is considered a client programming error, so we panic the 
	 //  client - this also completes the message.
	default:
		PanicClient(aMessage,EBadRequest);
		return;
        }
	}

/**
Initialize the counter with the numeric equivalent of the descriptor contents
This function is here to demonstrate reading from the client address space.
Note that in this example, the client and the server are part of the same process,
*/
void CCountServSession::SetFromStringL(const RMessage2& aMessage)
	{
	
	  // length of passed descriptor (1st parameter passed from client)
	TInt deslen = aMessage.GetDesLength(0);
	
	  // Passed data will be saved in this descriptor.
    RBuf buffer;
      
      // Max length set to the value of "deslen", but current length is zero
    buffer.CreateL(deslen);
      
      // Do the right cleanup if anything subsequently goes wrong
    buffer.CleanupClosePushL();
    
      // Copy the client's descriptor data into our buffer.
    aMessage.ReadL(0,buffer,0);
    
      // Now do a validation to make sure that the string only has digits
    if (buffer.Length() == 0)
        {
    	User::Leave(ENonNumericString);
        }
    
    TLex16 lexer;
    
    lexer.Assign(buffer);
    while (!lexer.Eos())
        {
        TChar thechar;
        
        thechar = lexer.Peek();
        if (!thechar.IsDigit())
            {
        	User::Leave(ENonNumericString);
            }
        lexer.Inc();
        }
       
      // Convert to a simple TInt value. 
    lexer.Assign(buffer);           
    if (lexer.Val(iCount))
        {
    	User::Leave(ENonNumericString);
        }
    	
	  // Clean up the memory acquired by the RBuf variable "buffer"
	CleanupStack::PopAndDestroy();
	}


/**
Increases the session counter by default (1)
*/
void CCountServSession::Increase()
	{
	iCount++;
	}


/**
Increases the session counter by an integer.
*/
void CCountServSession::IncreaseBy(const RMessage2& aMessage)
	{
	iCount = iCount + aMessage.Int0();
	}

/**
Decreases the session counter by default (1)
*/
void CCountServSession::Decrease()
	{
	iCount--;
	}


/**
Decreases the session counter by an integer.
*/
void CCountServSession::DecreaseBy(const RMessage2& aMessage)
	{
	iCount = iCount - aMessage.Int0();	
	}


/**
Resets the session counter.
*/
void CCountServSession::Reset()
	{
	iCount=0;
	}


/**
Writes the counter value to a descriptor in the client address space.
This function demonstrates writing to the client.
*/
void CCountServSession::CounterValueL(const RMessage2& aMessage)
	{
	TPckgBuf<TInt> p(iCount);
	aMessage.WriteL(0,p);
	}


/**
Panics the client
*/
void CCountServSession::PanicClient(const RMessage2& aMessage,TInt aPanic) const
	{
	_LIT(KTxtServer,"CountServ server");
	aMessage.Panic(KTxtServer,aPanic);
	}



//**********************************
//Global functions
//**********************************

// The count server thread function that initialises the server.
GLDEF_C TInt CCountServServer::ThreadFunction(TAny* /**aStarted*/)
	{
	  // get clean-up stack
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup == NULL)
	    {
		CCountServServer::PanicServer(ECreateTrapCleanup);
	    }
	
	  // create an active scheduler and server
	CActiveScheduler *pA=new CActiveScheduler;
	__ASSERT_ALWAYS(pA!=NULL,CCountServServer::PanicServer(EMainSchedulerError));
	CCountServServer *pS=new CCountServServer(EPriorityStandard);
	__ASSERT_ALWAYS(pS!=NULL,CCountServServer::PanicServer(ESvrCreateServer));
		
	  //Install the active scheduler
	CActiveScheduler::Install(pA);
	  
	  // Start the server
	TInt err = pS->Start(KCountServerName);
	if (err != KErrNone)
	    {
		CCountServServer::PanicServer(ESvrStartServer);
	    }
	
      // Let everyone know that we are ready to
      // deal with requests.
    RThread::Rendezvous(KErrNone);
	
	  // And start fielding requests from client(s).
	CActiveScheduler::Start();

      // Tidy up... 	
    delete pS;
    delete pA;
    delete cleanup; 
    
	  // ...although we should never get here!
	return(KErrNone);
	}


/**
Create the thread that will act as the server.
This function is exported from the DLL and called by the client.

Note that a server can also be implemented as a separate
executable (i.e. as a separate process).
*/
EXPORT_C TInt StartThread(RThread& aServerThread)
    {
	TInt res=KErrNone;
	
	  // Create the server, if one with this name does not already exist.
	TFindServer findCountServer(KCountServerName);
	TFullName   name;
	
	  // Need to check that the server exists.
	if (findCountServer.Next(name)!=KErrNone)
		{
		  // Create the thread for the server.
 		res=aServerThread.Create(KCountServerName,
			CCountServServer::ThreadFunction,
			KDefaultStackSize,
			KDefaultHeapSize,
			KDefaultHeapSize,
			NULL
			);
			
          // The thread has been created OK so get it started - however
          // we need to make sure that it has started before we continue.
		if (res==KErrNone)
			{
			TRequestStatus rendezvousStatus;
			
			aServerThread.SetPriority(EPriorityNormal);
			aServerThread.Rendezvous(rendezvousStatus);
			aServerThread.Resume();
			User::WaitForRequest(rendezvousStatus);
			}
			
		  // The thread has not been created - clearly there's been a problem.
		else
			{
			aServerThread.Close();
			}
		}
    return res;
    }
    
    