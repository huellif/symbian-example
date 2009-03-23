//
// CImplementationClassOne.h
// Copyright © 2001 Symbian Ltd. All rights reserved.
//

#ifndef _CIMPLEMENTATIONCLASSONE__
#define _CIMPLEMENTATIONCLASSONE__

#include <Interface.h>

// An implementation of the CExampleInterface definition
class CImplementationClassOne : public CExampleInterface
	{
public:
	// Standardised safe construction which leaves nothing the cleanup stack.
	static CImplementationClassOne* NewL(TAny* aInitParams);
	// Destructor	
	~CImplementationClassOne();

	// Implementation of CExampleInterface
	void DoMethodL(TDes& aString);

private:
	// Construction
	CImplementationClassOne(TAny* aParams);
	void ConstructL();

private:
	// Data to pass back from implementation to client
	HBufC* iDescriptor;
	// Parameters taken from client
	CExampleInterface::TExampleInterfaceInitParams* iInitParams;
	};  

#endif

