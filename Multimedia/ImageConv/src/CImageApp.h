// CImageApp.h
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
//

#ifndef CIMAGEAPP
#define CIMAGEAPP

#include <eikapp.h>

class CImageApp : public CEikApplication
	{
private:
	// from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};

#endif // CIMAGEAPP
