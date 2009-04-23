/*
 ============================================================================
 Name		: hellosis.pan
 Author	  : 
 Copyright   : Your copyright notice
 Description : This file contains panic codes.
 ============================================================================
 */

#ifndef __HELLOSIS_PAN__
#define __HELLOSIS_PAN__

/** hellosis application panic codes */
enum ThellosisPanics
	{
	EhellosisUi = 1
	// add further panics here
	};

inline void Panic(ThellosisPanics aReason)
	{
	_LIT(applicationName, "hellosis");
	User::Panic(applicationName, aReason);
	}

#endif // __HELLOSIS_PAN__
