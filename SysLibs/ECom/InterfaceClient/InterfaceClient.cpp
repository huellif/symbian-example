// InterfaceClient.cpp
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.

#include <badesca.h>

#include <Interface.h>

#include "CommonFramework.h"
#include "InterfaceClient.h"

// Utility clean up function
void CleanupEComArray(TAny* aArray);

// do the example
LOCAL_C void doExampleL()
    {
	_LIT(KText,"ECom client example\n\n");
	console->Printf(KText);

	TInterfaceClient client;
	client.GetDefaultL();
	client.GetBySpecificationL();
	client.GetByDiscoveryL();
	
	REComSession::FinalClose();
	}

void TInterfaceClient::GetDefaultL()
	{
	_LIT(KText,"Case 1: getting the default implementation\n");
	console->Printf(KText);

	// Get the default implementation and call its method
	CExampleInterface* ex1 = CExampleInterface::NewL();
	CleanupStack::PushL(ex1);
	TBuf<100> buf;
	ex1 -> DoMethodL(buf);
	CleanupStack::PopAndDestroy(); //ex1

	// Print results
	console -> Printf(buf);
	}

void TInterfaceClient::GetBySpecificationL()
	{
	_LIT(KText,"\nCase 2: getting an implementation by specification\n");
	console->Printf(KText);

	// Prepare data to pass to implementation
	CExampleInterface::TExampleInterfaceInitParams p;
	p.integer = 0;
	_LIT(KMsg1,"Data in value: %d\n");
	console -> Printf(KMsg1,p.integer);

	// Get the implementation that has a data identifier text/xml
	_LIT8(KSpec,"text/xml");
	CExampleInterface* ex1 = CExampleInterface::NewL(KSpec,p);
	CleanupStack::PushL(ex1);
	TBuf<100> buf;
	ex1 -> DoMethodL(buf);
	CleanupStack::PopAndDestroy(); //ex1

	// Print results
	console -> Printf(buf);
	_LIT(KMsg2,"Data out value: %d\n");
	console -> Printf(KMsg2,p.integer);
	}


void TInterfaceClient::GetByDiscoveryL()
	{
	_LIT(KText,"\nCase 3: getting all implementations\n");
	console->Printf(KText);

	// Read info about all implementations into infoArray
	RImplInfoPtrArray infoArray;
	// Note that a special cleanup function is required to reset and destroy
	// all items in the array, and then close it.
	TCleanupItem cleanup(CleanupEComArray, &infoArray);
	CleanupStack::PushL(cleanup);
	CExampleInterface::ListAllImplementationsL(infoArray);

	// Loop through each info for each implementation
	// and create and use each in turn
	CExampleInterface* ex;
	TBuf<255> buf;
	for (TInt i=0; i< infoArray.Count(); i++)
		{
		// Slice off first sub-section in the data section
		TPtrC8 type = infoArray[i]->DataType();
		type.Set(type.Left(type.Locate('|')));
		// Need to convert narrow descriptor to be wide in order to print it
		buf.Copy(type);
		console -> Printf(buf);
		console -> Printf(_L(": "));

		// Create object of type and call its function
		ex = CExampleInterface::NewL(type);
		CleanupStack::PushL(ex);
		ex -> DoMethodL(buf);
		CleanupStack::PopAndDestroy(); //ex

		// Print results
		console -> Printf(buf); 
		
		ex = NULL;
		buf.Zero();
		}

	// Clean up
	CleanupStack::PopAndDestroy(); //infoArray, results in a call to CleanupEComArray
	}

// CleanupEComArray function is used for cleanup support of locally declared arrays
void CleanupEComArray(TAny* aArray)
	{
	(static_cast<RImplInfoPtrArray*> (aArray))->ResetAndDestroy();
	(static_cast<RImplInfoPtrArray*> (aArray))->Close();
	}

