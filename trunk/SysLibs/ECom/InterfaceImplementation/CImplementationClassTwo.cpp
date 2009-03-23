//
// CImplementationClassTwo.cpp
// Copyright © 1997-2001 Symbian Ltd. All rights reserved.
//

#include "CImplementationClassTwo.h"

// Construction and destruction functions

CImplementationClassTwo* CImplementationClassTwo::NewL(TAny* aParams)
	{
	CImplementationClassTwo* self=new(ELeave) CImplementationClassTwo(aParams);  
	CleanupStack::PushL(self);
	self->ConstructL(); 
	CleanupStack::Pop();
	return self;
	}

CImplementationClassTwo::~CImplementationClassTwo()
	{
	delete iDescriptor;
	}

CImplementationClassTwo::CImplementationClassTwo(TAny* aInitParams)
:  iInitParams((CExampleInterface::TExampleInterfaceInitParams*)aInitParams)
	{
	// See ConstructL() for initialisation completion.
	}

void CImplementationClassTwo::ConstructL()
// Safely complete the initialization of the constructed object	
	{
	// Set up the data to pass back
	_LIT(KDescriptor, "Using Implementation Two\n");
	iDescriptor = KDescriptor().AllocL();
	}


// Implementation of CExampleInterface

void CImplementationClassTwo::DoMethodL(TDes& aString)
	{
	aString = *iDescriptor;

	if (iInitParams)
		iInitParams->integer=2;
	}

