// CalExample.h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef __CALEXAMPLE_H__
#define __CALEXAMPLE_H__

#include <e32base.h>
#include <e32cons.h>

#include <calsession.h>
#include <calprogresscallback.h>
#include <calrrule.h>
#include <calentry.h>
#include <calentryview.h>
#include <caltime.h>
#include <calalarm.h>
#include <calcategory.h>

#include <caluser.h>
#include <calinstanceview.h>

_LIT8(KGuid, "13012006GlobalUId@symbian.com");

// Demonstrates how to create, add, fetch and update entries. 
// Uses an entry view to display a non repeating appointment, with 
// a start and end date/time, a description, a category and some attendees.
class CCalExample: public CBase, public MCalProgressCallBack
	{
public:

	static CCalExample* NewL();
	~CCalExample();
	
	void AddEntryL();
	void UpdateEntryL();		
	void FetchEntriesL(RPointerArray<CCalEntry>& aCalEntryArray);
	void PrintEntryDetailsL();
	
	void FindInstanceL();
	void AddOriginatingEntryL();
	void AddmodifyingEntryL();	
	void DeleteEntryL();	
	
	// Overridden functions of MCalProgressCallBack class
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt aError);
	TBool NotifyProgress();
	
private:
	CCalExample();
	void ConstructL();
	void SetEntryDetailsL(CCalEntry* aEntry,const TDesC& aDescription, TDateTime& aStartTime,TDateTime& aEndTime);
	
private:
	CConsoleBase* 		iConsole;
	CCalSession* 		iCalSession;
	CCalEntryView* 		iCalEntryView;
    };

#endif //__CALEXAMPLE_H__
