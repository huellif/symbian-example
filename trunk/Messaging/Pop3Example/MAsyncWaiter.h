//  MAsyncWaiter.h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//
/**
@file
Contains the CMessAsyncWaiter class.
*/

#ifndef __MESS_ASYNCWAITER_H_
#define __MESS_ASYNCWAITER_H_

#include <e32base.h>

/**
CMessAsyncWaiter is a class that publicily inherits the class CActive.
CActive is core class of the active object abstraction.
It encapsulates both issuing a request to an asynchronous service provider 
and handling the completed requests.
*/
class CMessAsyncWaiter : public CActive
	{
public:
	static CMessAsyncWaiter* NewL();
	~CMessAsyncWaiter();

	void StartAndWait();
	TInt Result() const;
private:
	CMessAsyncWaiter();
	// CActive implementation
	virtual void RunL();
	virtual void DoCancel();
private:
	TInt iError;
	};
#endif // __MESS_ASYNCWAITER_H_
