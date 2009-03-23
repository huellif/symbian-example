//  asyncwaiter.cpp
// 
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//
/**
@file
This encapsulates both issuing a request to an asynchronous service provider 
and handling the completed requests.
*/

#include "asyncwaiter.h"

/**
 Creates an object of CMessAsyncWaiter
*/  
CAsyncWaiter* CAsyncWaiter::NewL()
	{
	CAsyncWaiter* self = new(ELeave) CAsyncWaiter();
	return self;
	}

/**
Adds the active object to the scheduler
*/
CAsyncWaiter::CAsyncWaiter() : CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}	


CAsyncWaiter::~CAsyncWaiter()
	{
	Cancel();
	}
	
/**
Indicates that the active object has issued a request 
and that it is now outstanding .
*/ 
void CAsyncWaiter::StartAndWait()
	{
	SetActive();
	CActiveScheduler::Start();
	}
	

TInt CAsyncWaiter::Result() const
	{
	return iError;
	}
	
/**
Handles an active object’s request completion event.
It sets to run the desired task.
*/ 
void CAsyncWaiter::RunL()
	{
	iError = iStatus.Int();
	CActiveScheduler::Stop();
	}
	
/**
Implements cancellation of an outstanding request.
This function is called as part of the active object's Cancel().
*/
void CAsyncWaiter::DoCancel()
	{
	iError = KErrCancel;
	CActiveScheduler::Stop();
	}
