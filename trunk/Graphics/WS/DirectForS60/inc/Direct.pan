/*
 ============================================================================
 Name		: Direct.pan
 Author	  : 
 Copyright   : Your copyright notice
 Description : This file contains panic codes.
 ============================================================================
 */

#ifndef __DIRECT_PAN__
#define __DIRECT_PAN__

/** Direct application panic codes */
enum TDirectPanics
	{
	EDirectUi = 1
	// add further panics here
	};

inline void Panic(TDirectPanics aReason)
	{
	_LIT(applicationName, "Direct");
	User::Panic(applicationName, aReason);
	}

#endif // __DIRECT_PAN__
