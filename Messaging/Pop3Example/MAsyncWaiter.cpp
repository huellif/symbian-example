//  MAsyncWaiter.cpp
// 
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//
/**
@file
Encapsulates both issuing a request to an asynchronous service provider 
and handling the completed requests.
*/

#include "MAsyncWaiter.h"

/**
 Creates an object of CMessAsyncWaiter
*/  
CMessAsyncWaiter* CMessAsyncWaiter::NewL()
	{
	CMessAsyncWaiter* self = new(ELeave) CMessAsyncWaiter();
	return self;
	}

/**
Adds the active object to the scheduler
*/
CMessAsyncWaiter::CMessAsyncWaiter() : CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}	


CMessAsyncWaiter::~CMessAsyncWaiter()
	{
	Cancel();
	}
	
/**
Indicates that the active object has issued a request 
and that it is now outstanding .
*/ 
void CMessAsyncWaiter::StartAndWait()
	{
	SetActive();
	CActiveScheduler::Start();
	}
	

TInt CMessAsyncWaiter::Result() const
	{
	return iError;
	}
	
/**
Handles an active object’s request completion event.
It sets to run the desired task.
*/ 
void CMessAsyncWaiter::RunL()
	{
	iError = iStatus.Int();
	CActiveScheduler::Stop();
	}
	
/**
Implements cancellation of an outstanding request,called as part of the active object's Cancel().
*/
void CMessAsyncWaiter::DoCancel()
	{
	iError = KErrCancel;
	CActiveScheduler::Stop();
	}

