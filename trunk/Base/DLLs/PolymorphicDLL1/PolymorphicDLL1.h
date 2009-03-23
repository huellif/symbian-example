// PolymorphicDLL1.h
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

#ifndef __PolymorphicDLL1_H
#define __PolymorphicDLL1_H

// get definition of base class
#include "UsingDLLs.h"

class CFrenchMessenger : public CMessenger
  	{
public:
	// constructor support
	virtual void ConstructL(CConsoleBase* aConsole, const TDesC& aName);
	// destructor
	virtual ~CFrenchMessenger();
	// useful functions
	virtual void ShowMessage();
	};

#endif
