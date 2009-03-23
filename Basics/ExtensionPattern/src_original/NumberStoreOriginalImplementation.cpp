//
// Copyright (c) 2000-2005 Symbian Software Ltd.  All rights reserved.
//
// 
// NumberStoreOriginalImplementation.cpp
//
// CPP file demonstrating Extension DLL pattern
// This file contains the original implementation of the CNumberStore function.
// This file has _not_ been modified in order to add public extension functions.
// The only extension is to add the private "DoMultiplyNumbers" function which is
// deliberately "private" to stop code calling it.  It must be called through the 
// public "MultiplyNumbers" function which is available in the extension dll.


#include <numberstore.h>



//This constant is defined in the CPP to demonstrate an intrusive change; 
//DoMultiplyNumbers must be implemented in this DLL in order to see this const
const TInt KMagicMultiplyer=2;
	
//Function implementations of all _original_ functions

EXPORT_C CNumberStore::CNumberStore ()
	{
	}

EXPORT_C CNumberStore::CNumberStore (TInt aNumber1, TInt aNumber2)
	{
	SetNumber1(aNumber1);
	SetNumber2(aNumber2);
	}

EXPORT_C void CNumberStore::SetNumber1 (TInt aNumber1)
	{
	iNumber1=aNumber1;
	}

EXPORT_C void CNumberStore::SetNumber2 (TInt aNumber2)
	{
	iNumber2=aNumber2;
	}

EXPORT_C TInt CNumberStore::Number1 () const
	{
	return iNumber1;
	}

EXPORT_C TInt CNumberStore::Number2 () const
	{
	return iNumber2;
	}

/**
Function implementations of private functions which are only used by the extension dll
This function must be defined in this file so it can see KMagicMultiplyer.
*/
EXPORT_C TInt CNumberStore::DoMultiplyNumbers() const
	{
	return (iNumber1*iNumber2*KMagicMultiplyer);
	}
