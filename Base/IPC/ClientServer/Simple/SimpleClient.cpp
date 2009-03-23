// SimpleClient.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.


//
//**NOTE**: The example does not demonstrate any security features - its purpose is simply
//        to demonstrate the basic principles of client/server interaction.
//



// needed for client interface
#include "ClientServer.h"

// needed for client (doExampleL)
#include "SimpleClient.h"
#include "CommonFramework.h"


const TUint kDefaultMessageSlots=4;


//**********************************
//RCountServ
//**********************************

RCountServSession::RCountServSession()
	{
	}


/**
Connects to the  server using 4 message slots.

In this example, the server is implemented as a separate thread.
The function starts that thread before attempting to create a session with the server.

The version information specifies the earliest version of the server that we can
talk to.
*/
TInt RCountServSession::Connect()
	{
	TInt r=StartThread(iServerThread);
	if (r==KErrNone)
		r=CreateSession(KCountServerName,Version(),kDefaultMessageSlots);
	return(r); 
	}


/**
Returns the earliest version number of the server that we can tal to.
*/	
TVersion RCountServSession::Version(void) const
	{
	return(TVersion(KCountServMajorVersionNumber,KCountServMinorVersionNumber,KCountServBuildVersionNumber));
	}


/**
A server request to set the counter value 
to the value defined in the string.
*/
TInt RCountServSession::SetFromString(const TDesC& aString)
	{
	TIpcArgs args(&aString);
    return SendReceive(ECountServSetFromString, args);
	}


/**
A server request to increase the counter value by the default value (i.e. 1)
*/
void RCountServSession::Increase()
	{
	SendReceive(ECountServIncrease);
	}


/**
A server request to increase the counter value by the specified
integer value.
*/
void RCountServSession::IncreaseBy(TInt anInt)
	{
	TIpcArgs args(anInt);
	SendReceive(ECountServIncreaseBy, args);
	}


/**
A server request to decrease the counter value by the default value (i.e. 1).
*/
void RCountServSession::Decrease()
	{
	SendReceive(ECountServDecrease);	
	}


/**
A server request to decrease the counter value by the specified
integer value.
*/
void RCountServSession::DecreaseBy(TInt anInt)
	{
	TIpcArgs args(anInt);
	SendReceive(ECountServDecreaseBy, args);
	}

/**
A server request to reset the counter value to 0.
*/
void RCountServSession::Reset()
	{
	SendReceive(ECountServReset);	
	}


/**
A server request to get the curent value of the counter.

We pass a TPckgBuf across to the server.
*/
TInt RCountServSession::CounterValue()
	{
	TInt res=0;
	TPckgBuf<TInt> pckg;
	
	  // Note that TPckgBuf is of type TDes8
	TIpcArgs args(&pckg);
	SendReceive(ECountServValue, args);
    
      // Extract the value returned from the server. 
	res = pckg();
	return res;
	}


/**
A server request to stop the server.

This is a request that is NOT implemented by the server; it is here to show
the handling of non-implemented requests.
*/
TInt RCountServSession::UnsupportedRequest()
	{
	return SendReceive(ECountServUnsupportedRequest);
	}

/**
A request that the server knows nothing about.
*/
void RCountServSession::BadRequest()
	{
	SendReceive(9999);
	}



/**
Closing the server and tidying up.
*/
void RCountServSession::Close()
	{
	RSessionBase::Close();
    iServerThread.Close();	
	}




/**
This is the main body of the example. It connects to the server,
causing it to be created if it doesn't already exist. 

It continues by making various requests on the server, displaying the results of those 
requests. 
*/
LOCAL_C void doExampleL()
	{
	_LIT(KTxtTestingCountServer,"Testing the count server \n\n");
	_LIT(KTxtInitCounterWith,"\nInitialize the counter with : ");		
    _LIT(KTxtInitCounterFailed,"\nSetting the counter from string failed: non-numeric character detected\n");	
    _LIT(KTxtGetCounterValue,"\nGetting the counter value back from the server: %d \n");
	
	
	  // Say that we are testing the count server
	console->Printf(KTxtTestingCountServer); 
	
	  // This is our handle to the server
    RCountServSession ss;

	  // Connect to the count server, starting it up if we need to 
	  // (which in this example we do need to).
	  // This creates a session with the server.
	User::LeaveIfError(ss.Connect());

      // At this point the server appears ready to accept requests.
	console->Printf(KTxtInitCounterWith);

      // Initialise the counter by passing an illegal string - this should
      // fail - but we will check anyway before reporting a failure.
    _LIT(KTxtIllegalString,"22h4");
	console->Printf(KTxtIllegalString);
	TInt ret = ss.SetFromString(KTxtIllegalString);
	if (ret==ENonNumericString)
	    {
		console->Printf(KTxtInitCounterFailed);
	    }
	
	  // Now try and initialise the counter using a legal string.
	console->Printf(KTxtInitCounterWith);
	_LIT(KTxtLegalString,"224");
	console->Printf(KTxtLegalString);
	ret = ss.SetFromString(KTxtLegalString);
	if (ret==ENonNumericString)
	    {
		console->Printf(KTxtInitCounterFailed);
	    }
	
	  // Now get the value back from the server, just to see what it believes
	  // it has. We are expecting the value 224 (of course!!!).
	ret = ss.CounterValue();
	console->Printf(KTxtGetCounterValue, ret);
	
	  // Now increase the counter value by the default value.
	_LIT(KTxt1,"\nIncrease counter (default 1)..");		
	console->Printf(KTxt1);
	ss.Increase();
	ret = ss.CounterValue();
	console->Printf(KTxtGetCounterValue, ret);

	  // Now increase the counter value by 2.
	_LIT(KTxt2,"\nIncrease counter by 2..");		
	console->Printf(KTxt2);
	ss.IncreaseBy(2);
	ret = ss.CounterValue();
	console->Printf(KTxtGetCounterValue, ret);
	
	  // Now decrease the counter value by the default value.	
	_LIT(KTxt3,"\nDecrease counter(default 1)..");		
	console->Printf(KTxt3);
	ss.Decrease();
    ret = ss.CounterValue();
	console->Printf(KTxtGetCounterValue, ret);
	
      // Now increase the counter value by 7.
	_LIT(KTxt4,"\nIncrease counter by 7..");		
	console->Printf(KTxt4);
	ss.IncreaseBy(7);
	ret = ss.CounterValue();
	console->Printf(KTxtGetCounterValue, ret);
		
      // Now increase the counter value again by the default value.		
	_LIT(KTxt5,"\nIncrease counter(default 1)..");	
	console->Printf(KTxt5);
	ss.Increase();
	ret = ss.CounterValue();
	console->Printf(KTxtGetCounterValue, ret);

      // Now decrease the counter value by 3.		
	_LIT(KTxt6,"\nDecrease counter by 3..");	
	console->Printf(KTxt6);
	ss.DecreaseBy(3);
	ret = ss.CounterValue();
	console->Printf(KTxtGetCounterValue, ret);
	
	  // Reset counter value
	_LIT(KTxt7,"\nReseting counter value to 0..");
	console->Printf(KTxt7);
	ss.Reset();
	ret = ss.CounterValue();
	console->Printf(KTxtGetCounterValue, ret);

	
	  // Call API function which is not implemented in the server
	_LIT(KTxt8,"\nAbout to call the unsupported function Stop()..");
	console->Printf(KTxt8);
	ret = ss.UnsupportedRequest();
	_LIT(KTxt9,"\nSorry, UnsupportedRequest() is not supported\n");
	if (ret==KErrNotSupported)
	    {
		console->Printf(KTxt9);
	    }
  		
     
      // This request will panic this client so do not remove the
      // following comment unless you want to try it.
//	ss.BadRequest();
   
   
	  // Close the sesssion with the count server.
	ss.Close();
	}
