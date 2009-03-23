// ControlFrameworkDocument.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef __CONTROLFRAMEWORKDOCUMENT_H
#define __CONTROLFRAMEWORKDOCUMENT_H

#include <eikdoc.h>

class CEikAppUi;
class CEikApplication;

//Document class
class CControlFrameworkDocument : public CEikDocument
	{
public:
	static CControlFrameworkDocument* NewL(CEikApplication& aApp);
	~CControlFrameworkDocument();
	
private:
	CControlFrameworkDocument(CEikApplication& aApp);
	
	// from CEikDocument
	CEikAppUi* CreateAppUiL();
	};


#endif // __CONTROLFRAMEWORKDOCUMENT_H

