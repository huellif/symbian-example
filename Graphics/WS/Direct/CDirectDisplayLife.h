// CDirectDisplayLife.h
//
// Copyright (c) 2005 Symbian Softwares Ltd.  All rights reserved.
//

#if !defined(__DIRECT_H__)
#define __DIRECT_H__

#include <w32std.h>

#include "CLifeEngine.h"

// Displays a running Game of Life using DSA API
class CDirectDisplayLife : 
	public CTimer,				
	public MDirectScreenAccess	// Call backs from DSA
	{
public:
	// Construction & destruction
	CDirectDisplayLife (RWsSession& aClient, 
		RWindow& aWindow, 
		CLifeEngine& aLifeEngine);
	~CDirectDisplayLife ();
	void ConstructL();

	// Start game display
	void StartL();
	
	// Implement MDirectScreenAccess
	void Restart(RDirectScreenAccess::TTerminationReasons aReason);
	void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);

private:
	// Implement CTimer
	void RunL();
	void DoCancel();

private:
	// Window server handling
	RWsSession& iClient;
	RWindow& iWindow;

	// DSA objects
	CDirectScreenAccess* iDirectScreenAccess;
	CFbsBitGc* iGc;
	RRegion* iRegion;

	// Drawing constants
	static const TInt KBlockSize;
	static const TInt KGenerationInterval;
	static const TInt iXOrigin;
	static const TInt iYOrigin;

	// Life game engine
	CLifeEngine& iLifeEngine;
	};

#endif //__DIRECT_H__
