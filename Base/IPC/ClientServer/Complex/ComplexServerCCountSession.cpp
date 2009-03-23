// ComplexServer.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

#include "ComplexClientAndServer.h"
#include "ComplexServer.h"
#include <e32svr.h>
#include <e32uid.h>


//*******************************
//CCountSession - implementations
//*******************************

/**
Constructor
*/
CCountSession::CCountSession()
	{
	}


/**
Called by client/server framework after 
session has been successfully created.

In effect, a second-phase constructor.

Creates:

1. the object index
2. the object container for this session.

We are then ready for subsessions.
*/
void CCountSession::CreateL()
	{
	  // Create new object index
	iCountersObjectIndex=CObjectIx::NewL();
	
	  // Initialize the object container
	  // using the object container index in the server.
	iContainer=((CCountServer*)Server())->NewContainerL();
	}


/**
Closes the session.

It deletes the object index and object container.

This could be done in the destructor, but it seems neater to do it here. 
*/
void CCountSession::CloseSession() 
	{
	  // Deletes the object index.
	delete iCountersObjectIndex;
	iCountersObjectIndex = NULL;
	
	  // Deletes the object container
	delete iContainer;
	iContainer = NULL;
	}


/**
A simple utility function to return the appropriate 
CCountSubSession object given a client's subsession handle.
Panics client if the handle cannot be found.
*/
CCountSubSession* CCountSession::CounterFromHandle(const RMessage2& aMessage,TInt aHandle)
    {
	CCountSubSession* counter = (CCountSubSession*)iCountersObjectIndex->At(aHandle);
	if (counter == NULL)
	    {
		PanicClient(aMessage, EBadSubsessionHandle); 
	    }
	return counter;
    }



/**
First line servicing of a client request.

This function dispatches requests to the appropriate handler.
Some messages are handled by the session itself, and are
implemented as CCountSession member functions, while 
other messages are handled by the subsession, and are
implemented as CCountSubSession member functions.
*/
void CCountSession::ServiceL(const RMessage2& aMessage)
	{
	TRAPD(err,DispatchMessageL(aMessage));
	aMessage.Complete(err);
	}


/**
Called by ServiceL()

It tests the function code and then delegates to
the appropriate function.
*/
void CCountSession::DispatchMessageL(const RMessage2& aMessage)
	{
	  // First check for session-relative requests
	switch (aMessage.Function())
		{
	case ECountServCreateSubSession:
		NewCounterL(aMessage);
		return;
	case ECountServCloseSession:
		CloseSession();
		return;
	case ECountServResourceCount:
		NumResourcesL(aMessage);
		return; 
		}
		
	  // All other function codes must be subsession relative.
	  // We need to find the appropriate server side subsession
	  // i.e. the CCountSubSession object. 
	  // The handle value is passed as the 4th aregument.
    CCountSubSession* counter=CounterFromHandle(aMessage,aMessage.Int3());
	switch (aMessage.Function())
        {
	case ECountServInitSubSession:
		counter->SetFromStringL(aMessage);
		return; 
	case ECountServCloseSubSession:
	     DeleteCounter(aMessage.Int3());
	     return;
	case ECountServIncrease:
		counter->Increase();
		return;
	case ECountServIncreaseBy:
		counter->IncreaseBy(aMessage);
		return;
	case ECountServDecrease:
		counter->Decrease();
		return;
	case ECountServDecreaseBy:
		counter->DecreaseBy(aMessage);
		return;
	case ECountServReset:
		counter->Reset();
		return;
	case ECountServValue:
		counter->CounterValueL(aMessage);
		return;
	default:
		PanicClient(aMessage,EBadRequest);
		return;
        }
	}


/**
Creates a new counter object,
and passes its handle back via the message.

The counter object is the server side "partner" to the
client side subsession.
*/
void CCountSession::NewCounterL(const RMessage2& aMessage)
	{
	  // make a new counter object
	CCountSubSession* counter= new (ELeave) CCountSubSession(this);
	 
	  // add the CCountSubSession object to 
	  // this subsession's object container
	  // to gererate a unique id
	iContainer->AddL(counter);
	
	  // Add the object to object index; this returns
	  // a unique handle so that we can find the object
	  // again laterit later.
	TInt handle=iCountersObjectIndex->AddL(counter);

      // Write the handle value back to the client.
      // NB It's not obvious but the handle value must be passed
      // back as the 4th parameter (i.e. parameter number 3 on
      // a scale of 0 to 3). 
      // The arguments that are passed across are actually
      // set up by RSubSessionBase::DoCreateSubSession().
      // If you pass your own arguments into a call
      // to RSubSessionBase::CreateSubSession(), which calls DoCreateSubSession, 
      // then only the first three are picked up - the 4th is reserved for the
      // the subsession handle.
	TPckgBuf<TInt> handlePckg(handle);
	TRAPD(res,aMessage.WriteL(3,handlePckg));
	if (res!=KErrNone)
		{
		iCountersObjectIndex->Remove(handle);
		PanicClient(aMessage,EBadDescriptor);
		return;
		}
		
	  // We now have another "resource"
	iResourceCount++;
	}




/**
Delete a subsession (counter) object through its handle.
*/
void CCountSession::DeleteCounter(TInt aHandle)
	{
	
	  // This will delete the CCountSubSession object; the object is
	  // reference counted, and removing the handle causes the object to be closed
	  // [closing reduces the access count - the object is deleted if the access
	  //  count reaches zero etc].
	iCountersObjectIndex->Remove(aHandle); 
	  // decrement resource count
	iResourceCount--;
	}


// return the number of resources owned by the session
// required by CSession if derived class implements resource
// mark-start and mark-end protocol
TInt CCountSession::CountResources()
	{
	return iResourceCount;
	}


/**
Gets the number of resources, i.e. the number of CCount objects
that exist on the server side.
*/
void CCountSession::NumResourcesL(const RMessage2& aMessage)
	{
	TPckgBuf<TInt> resourcesPckg(iResourceCount);
	aMessage.WriteL(0,resourcesPckg);
	}


/**
Panics the client
*/
void CCountSession::PanicClient(const RMessage2& aMessage,TInt aPanic) const
	{
	_LIT(KTxtServer,"CountServ server");
	aMessage.Panic(KTxtServer,aPanic);
	}

