// ComplexClientSubSession.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.


/**
Implementation of the RCountSubSession class
*/

#include "ComplexClientAndServer.h"
#include "ComplexClient.h"



/**
Creates a new subsession with the count server.
*/
TInt RCountSubSession::Open(RCountSession &aSession)
    {
    return CreateSubSession(aSession,ECountServCreateSubSession);
	}


/**
Close the subsession.

This just a simple wrapper around the 
RSubSessionBase class function that does the actual close operation.
*/
void RCountSubSession::Close()
	{
	RSubSessionBase::CloseSubSession(ECountServCloseSubSession);
	}


/**
A server request to initialise the subsession counter 
using a descriptor (e.g passing the text string "12").
*/
TInt RCountSubSession::SetFromString(const TDesC& aString)
	{
	TIpcArgs args(&aString);
    return SendReceive(ECountServInitSubSession, args);
	}


/**
A server request to increase the counter value by 
the default value (i.e. 1).
*/
void RCountSubSession::Increase()  
	{
	if (SubSessionHandle())
		{
		SendReceive(ECountServIncrease);
		}
	}


/**
A server request to increase the counter value by the specified
integer value.
*/
void RCountSubSession::IncreaseBy(TInt anInt)
	{
	if (SubSessionHandle())
		{
		TIpcArgs args(anInt);
	    SendReceive(ECountServIncreaseBy, args);
		}
	}


/**
A server request to decrease the counter value by
the default value (i.e. 1).
*/
void RCountSubSession::Decrease() 
	{
	if (SubSessionHandle())
		{
		SendReceive(ECountServDecrease);
		}
	}


/**
A server request to decrease the counter value by the specified
integer value.
*/
void RCountSubSession::DecreaseBy(TInt anInt)
	{
	if (SubSessionHandle())
		{
		TIpcArgs args(anInt);
	    SendReceive(ECountServDecreaseBy, args);
		}
	}


/**
A server request to reset the counter value to 0.
*/
void RCountSubSession::Reset()
	{
	if (SubSessionHandle())
		{
		SendReceive(ECountServReset);
		}
	}


/**
A server request to get the curent value of the counter.

We pass a TPckgBuf across to the server.
*/
TInt RCountSubSession::CounterValue()
	{
	TInt res = KErrNotFound;
	TPckgBuf<TInt> pckg;
	
	if (SubSessionHandle())
		{
		  // Note that TPckgBuf is of type TDes8
	    TIpcArgs args(&pckg);
	    SendReceive(ECountServValue, args);
		
		  // Extract the value returned from the server. 
	    res = pckg();
		}
		
    return res;        
	}



