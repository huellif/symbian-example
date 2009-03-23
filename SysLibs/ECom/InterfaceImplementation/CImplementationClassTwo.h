//
// CImplementationClassTwo.h
// Copyright © 2001 Symbian Ltd. All rights reserved.
//

#ifndef _CIMPLEMENTATIONCLASSTWO__
#define _CIMPLEMENTATIONCLASSTWO__

#include <Interface.h>

// An implementation of the CExampleInterface definition
class CImplementationClassTwo : public CExampleInterface
	{
public:
	// Standardised safe construction which leaves nothing the cleanup stack.
	static CImplementationClassTwo* NewL(TAny* aInitParams);
	// Destructor	
	~CImplementationClassTwo();

	// Implementation of CExampleInterface
	void DoMethodL(TDes& aString);

private:
	// Construction
	CImplementationClassTwo(TAny* aInitParams);
	void ConstructL();

private:
	// Data to pass back from implementation to client
	HBufC* iDescriptor;
	// Parameters taken from client
	CExampleInterface::TExampleInterfaceInitParams* iInitParams;
	};  

#endif
