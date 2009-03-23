// ControlFrameworkApplication.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef __CONTROLFRAMEWORKAPPLICATION_H
#define __CONTROLFRAMEWORKAPPLICATION_H

#include <eikapp.h>

//Application class
class CControlFrameworkApplication : public CEikApplication
	{
public:
	static CApaApplication* NewApplication();
	~CControlFrameworkApplication();
	
private:
	CControlFrameworkApplication();
	
	// from CApaApplication
	TUid AppDllUid() const;
	CApaDocument* CreateDocumentL();
	};


#endif // __CONTROLFRAMEWORKAPPLICATION_H

