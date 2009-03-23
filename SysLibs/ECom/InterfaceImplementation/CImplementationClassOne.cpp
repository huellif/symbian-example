//
// CImplementationClassOne.cpp
// Copyright © 1997-2001 Symbian Ltd. All rights reserved.
//

#include "CImplementationClassOne.h"

// Construction and destruction functions

CImplementationClassOne* CImplementationClassOne::NewL(TAny* aInitParams)
	{
	CImplementationClassOne* self=new(ELeave) CImplementationClassOne(aInitParams);
	CleanupStack::PushL(self);
	self->ConstructL(); 
	CleanupStack::Pop();
	return self;
	}

CImplementationClassOne::~CImplementationClassOne()
	{
	delete iDescriptor;
	}

CImplementationClassOne::CImplementationClassOne(TAny* aInitParams)
// Store input/output parameters
: iInitParams((CExampleInterface::TExampleInterfaceInitParams*)aInitParams)
	{
	// See ConstructL() for initialisation completion.
	}

void CImplementationClassOne::ConstructL()
// Safely complete the initialization of the constructed object	
	{
	// Set up the data on the heap to pass back
	_LIT(KDescriptor, "Using Implementation One\n");
	iDescriptor = KDescriptor().AllocL();
	}


// Implementation of CExampleInterface

void CImplementationClassOne::DoMethodL(TDes& aString)
	{
	aString = *iDescriptor;

	// Set the parameter to something significant
	if (iInitParams)
		iInitParams->integer=1;
	}
	
