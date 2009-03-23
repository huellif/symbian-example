// NumberStore.h
//
// Copyright (c) 1997-2003 Symbian Ltd.  All rights reserved.
//
// Header file demonstrating Extension DLL pattern
// This is the header file declaring the CNumberStore class.
// This file contains both the original functions and also the functions added by
// the extension.  Therefore this file is modified by the extender of the API.

#if !defined(__NUMBERSTORE_V2__)
#define __NUMBERSTORE_V2__

#include <e32std.h>
#include <e32base.h>

/* class CNumberStore
A trivial class to own two numbers */
class CNumberStore : public CBase
	{	
public: //functions
	
	// Original functions contained in dll from supplier
	IMPORT_C CNumberStore ();
	IMPORT_C CNumberStore (TInt aNumber1, TInt aNumber2);
	IMPORT_C void SetNumber1 (TInt aNumber1);
	IMPORT_C void SetNumber2 (TInt aNumber2);
	IMPORT_C TInt Number1 () const;
	IMPORT_C TInt Number2 () const;

	// New functions added as a product specific extension to be implemented elsewhere
	// AddNumbers is a "non-intrusive" function that will be fully implemented in the extension
	IMPORT_C TInt AddNumbers() const;
	// MultiplyNumbers is an "intrusive" function that will be implemented in the orginal dll as DoMultiplyNumbers
	IMPORT_C TInt MultiplyNumbers() const;

private: //functions
	
	// DoMultiplyNumbers is the local implementation of MultiplyNumbers. Private to force all clients to call it
	// via the MultiplyNumbers function in the extension dll.  It must be implemented in this DLL as it needs access
	// to a constant which is defined in originaldll.dll
	IMPORT_C TInt DoMultiplyNumbers() const;

private: //data
	TInt iNumber1;
	TInt iNumber2;
	};

#endif //__NUMBERSTORE_V2__
