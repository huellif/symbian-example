// CImageDocument.h
//
// Copyright (c) 2001-2005 Symbian Software Ltd.  All rights reserved.
//

#ifndef CIMAGEDOCUMENT
#define CIMAGEDOCUMENT

#include <eikdoc.h>

class CImageDocument : public CEikDocument
	{
public:
	CImageDocument(CEikApplication& aApp);
private:
	 // from CEikDocument
	CEikAppUi* CreateAppUiL();
	};

#endif
