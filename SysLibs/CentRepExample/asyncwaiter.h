//  asyncwaiter.h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//
/**
@file
Contains the CAsyncWaiter class.
*/
#ifndef __ASYNCWAITER_H_
#define __ASYNCWAITER_H_

#include <e32base.h>

/**
CAsyncWaiter is a class that publicly inherits the class CActive.
CActive is core class of the active object abstraction.
It encapsulates both issuing a request to an asynchronous service provider 
and handling the completed requests.
*/
class CAsyncWaiter : public CActive
	{
public:
	static CAsyncWaiter* NewL();
	~CAsyncWaiter();

	void StartAndWait();
	TInt Result() const;
private:
	CAsyncWaiter();
	// CActive implementation
	virtual void RunL();
	virtual void DoCancel();
private:
	TInt iError;
	};
#endif // __ASYNCWAITER_H_
