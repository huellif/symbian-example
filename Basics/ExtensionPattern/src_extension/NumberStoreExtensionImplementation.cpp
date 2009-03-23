//  Copyright (c) 2000-2005 Symbian Software Ltd.  All rights reserved.
//
//  NumberStoreExtensionImplementation.cpp
//
//  This file contains the implementation of the extension functions which
//  have been declared in the numberstore.h file.
//  These file will be built into extensiondll.dll and thus in order to use
//  this function a client must link to extensiondll.lib
//

#include "numberstore.h"


//Implementation of the added functions

EXPORT_C TInt CNumberStore::AddNumbers () const
	{
	return (iNumber1+iNumber2);
	}

EXPORT_C TInt CNumberStore::MultiplyNumbers() const
	{
	return DoMultiplyNumbers();
	}
