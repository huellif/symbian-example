//
// Copyright (c) 2000-2005 Symbian Software Ltd.  All rights reserved.
//
// NumberStoreExtensionImplementation_v2.cpp
//
//
// CPP file demonstrating Extension DLL pattern
//  This file contains the implementation of the extension functions which
//  have been declared in the numberstore.h file.
//  These file will be built into extensiondll.dll and thus in order to use
//  this function a client must link to extensiondll.lib
//
// NOTE this file is unchanged from the version used previously, but it must be 
// rebuilt to align with the rearranged ordinals in originaldll_v2

#include "numberstore_v2.h"

//Implementation of the added function

EXPORT_C TInt CNumberStore::AddNumbers () const
	{
	return (iNumber1+iNumber2);
	}

EXPORT_C TInt CNumberStore::MultiplyNumbers() const
	{
	return DoMultiplyNumbers();
	}
