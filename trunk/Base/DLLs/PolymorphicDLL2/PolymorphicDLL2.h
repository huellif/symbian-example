// PolymorphicDLL2.h
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

#ifndef __PolymorphicDLL2_H
#define __PolymorphicDLL2_H

// get definition of base class
#include "UsingDLLS.h"

class CGermanMessenger : public CMessenger
  	{
public:
	// constructor support
	virtual void ConstructL(CConsoleBase* aConsole, const TDesC& aName);
	// destructor
	virtual ~CGermanMessenger();
	// useful functions
	virtual void ShowMessage();
	};

#endif
