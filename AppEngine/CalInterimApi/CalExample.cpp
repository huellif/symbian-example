// CalExample.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#include "CalExample.h"

#include <calinstance.h>

CCalExample::CCalExample()
	{
	}

CCalExample::~CCalExample()
	{
	_LIT(KExitMsg,"\n\nPress any key to exit the application ");
	iConsole->Printf(KExitMsg); 
	iConsole->Getch();
	
	delete iCalEntryView;
	delete iCalSession;	
	delete iConsole; 
	}

CCalExample* CCalExample::NewL()
 	{
	CCalExample* self= new (ELeave) CCalExample();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();    
	return self;
	}

void CCalExample::ConstructL()
	{
	_LIT(KTitle, "calexample" );
	iConsole = Console::NewL(KTitle,TSize(KConsFullScreen,KConsFullScreen));

	_LIT(KPressAKeyMsg, "\n\nPress any key to step through the example");
	iConsole->Printf ( KPressAKeyMsg );
	iConsole->Getch ();
	
	iCalSession = CCalSession::NewL();
	// Create and open a calendar file
	_LIT(KFileName,"calendarfile");
	TRAPD(err, iCalSession->CreateCalFileL(KFileName));
	// ignore KErrAlreadyExists leave code
	if (err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	// Open the calendar file
	iCalSession->OpenL(KFileName);
	
	_LIT(KProgressMsg, "\nCalendar entry view creation is in progress...");
	iConsole->Printf(KProgressMsg);	
	// Create a calendar entry view to view the entries
	iCalEntryView = CCalEntryView::NewL(*iCalSession,*this);
	CActiveScheduler::Start();		
	}

// Called during calendar entry view creation
void CCalExample::Progress(TInt aProgress)
	{
	_LIT(KProgressVal,"\n%d%% complete");
	iConsole->Printf(KProgressVal,aProgress);
	}

// Called on completion of calendar entry view creation
void CCalExample::Completed(TInt /*aError*/)
	{
	CActiveScheduler::Stop();
	}

// Returns whether or not progress notification is required
TBool CCalExample::NotifyProgress()
	{
	// Progress notification is required
	return ETrue;
	}
		
// Sets some entry details - start and end date/time, description, categories and attendees
void CCalExample::SetEntryDetailsL(CCalEntry* aEntry,const TDesC& aDescription, TDateTime& aStartTime,TDateTime& aEndTime)
	{
  	TCalTime startTime;
   	TCalTime endTime;
   	
	// Set the start and end times using time values local to the current system time zone
	startTime.SetTimeLocalL(aStartTime);
	endTime.SetTimeLocalL(aEndTime);
	aEntry->SetStartAndEndTimeL(startTime, endTime);	

	// Set the description text
	aEntry->SetDescriptionL(aDescription);	
	
	// Add a category
	const RPointerArray<CCalCategory> categoryList = aEntry->CategoryListL();
	if(categoryList.Count() == 0)
		{
		TBuf<32> buf;
		_LIT(KCategoryName, "Dummy Category");
		buf.Copy(KCategoryName);
		
		CCalCategory* category = CCalCategory::NewL(buf);
		CleanupStack::PushL(category);
		aEntry->AddCategoryL(category);
		CleanupStack::Pop(category);
		} 
		
	// Add some attendees
	const RPointerArray<CCalAttendee>attendeeList = aEntry->AttendeesL();
	if(attendeeList.Count() == 0)
		{
		_LIT(KAttendeeOne, "symbianuk@symbian.com");
		_LIT(KAttendeeTwo, "symbianindia@symbian.com");

		CCalAttendee* attendeeOne = CCalAttendee::NewL(KAttendeeOne);
		CleanupStack::PushL(attendeeOne);	
		aEntry->AddAttendeeL(attendeeOne);	
		CleanupStack::Pop(attendeeOne);
	
		CCalAttendee* attendeeTwo = CCalAttendee::NewL(KAttendeeTwo);
		CleanupStack::PushL(attendeeTwo);	
		aEntry->AddAttendeeL(attendeeTwo);
		CleanupStack::Pop(attendeeTwo);
		}
	}
	
// Add a non-repeating appointment to the calendar file
void CCalExample::AddEntryL()
	{ 
	const TDesC8& guidDes = KGuid;
	HBufC8* guid = guidDes.AllocL();
	
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::PushL(entry);
		
	// For an appointment, the time as well as the date is relevant
	TDateTime startTime(2006, EJanuary, 04, 10, 0, 0, 0);
	TDateTime endTime(2006, EJanuary, 05, 16, 0, 0, 0);
	
	_LIT(KAddingMsg, "\n\nAdding an entry...");
	iConsole->Printf ( KAddingMsg );
		
	_LIT(KDescription,"Meeting is scheduled in 1st week of January");
	SetEntryDetailsL(entry,KDescription, startTime, endTime);
		
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry);

	TInt success(0);
	// If StoreL() leaves, 'success' contains the number of entries that were
	// stored before it failed
	iCalEntryView->StoreL(array, success);

	CleanupStack::PopAndDestroy(&array);
	CleanupStack::PopAndDestroy(entry);
	}

// Destroy the RPointerArray
void DestroyRPointerArray(TAny* aPtr)
	{
	RPointerArray<CCalEntry>* self = static_cast<RPointerArray<CCalEntry>*> (aPtr);
	self->ResetAndDestroy();
	}

// Fetch the entry and update some of its details
void CCalExample::UpdateEntryL()
	{
	RPointerArray<CCalEntry> array;
	CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &array));
	iCalEntryView->FetchL(KGuid, array);

	//Fetch the first entry in the list
	_LIT(KUpdatingMsg,"\n\nUpdating the entry...");
	iConsole->Printf(KUpdatingMsg);
	CCalEntry* entry = array[0];
	
	_LIT(KNewDescription,"Meeting rescheduled to 2nd week of January");
	TDateTime startTime(2006, EJanuary, 11, 10, 0, 0, 0); 
	TDateTime endTime(2006, EJanuary, 12, 16, 0, 0, 0);
	SetEntryDetailsL(entry,KNewDescription, startTime, endTime);	
			
	TInt numberOfEntries(0);
	// If UpdateL() leaves, 'numberOfEntries' contains the number of entries that were
	// updated before it failed
	iCalEntryView->UpdateL(array,numberOfEntries);
	CleanupStack::PopAndDestroy(&array);
	}
	
// Add a repeating appointment. This is called the originating entry.
void CCalExample::AddOriginatingEntryL()
	{
	_LIT(KAddingMsg, "\n\nAdding a repeating entry...");
	iConsole->Printf ( KAddingMsg );
	
	TTime startTime(TDateTime(2006, EJanuary, 0, 14, 0, 0, 0)); // January 1st 2pm
	TTime endTime(startTime + TTimeIntervalHours(1)); // January 1st 3pm	
	
	const TDesC8& guidDes = KGuid;
	HBufC8* guid = guidDes.AllocL();
	CCalEntry* originatingEntry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::PushL(originatingEntry);
	
	_LIT(KDescription,"An originating entry");
	originatingEntry->SetDescriptionL(KDescription);
	
	TCalTime entryStartTime;
	entryStartTime.SetTimeLocalL(startTime);
	TCalTime entryEndTime;
	entryEndTime.SetTimeLocalL(endTime);
	originatingEntry->SetStartAndEndTimeL(entryStartTime, entryEndTime);
	
	// Set the repeat rule
	// The entry repeats weekly with 3 instances being created
	TCalRRule weeklyRptRule(TCalRRule::EWeekly);
	weeklyRptRule.SetDtStart(entryStartTime);	
	weeklyRptRule.SetCount(3); 
	weeklyRptRule.SetInterval(1);
	// Also need to set the day that the entry repeats on
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(ESunday);
	weeklyRptRule.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	originatingEntry->SetRRuleL(weeklyRptRule);

	// Now write the entry to the view
	RPointerArray<CCalEntry> array;
	CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &array));
	array.AppendL(originatingEntry);
	TInt success(0);
	// If StoreL() leaves, 'success' contains the number of entries that were
	// stored before it failed
	iCalEntryView->StoreL(array, success);
	
	// clean up	
	array.Close();
	CleanupStack::PopAndDestroy(&array);
	CleanupStack::PopAndDestroy(originatingEntry);	
	}
	
// Adds a modifying, or "child" entry.
// A modifying entry modifies one or more instances of the "originating" repeating entry. 
// It has the same UID, and is deleted when the originating entry is deleted.
void CCalExample::AddmodifyingEntryL()
	{
	RPointerArray<CCalEntry> modifyingArray;
	CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &modifyingArray));
	
	_LIT(KAddingMsg, "\n\nAdding a modifying entry...");
	iConsole->Printf ( KAddingMsg );
	// Create repeating modifying entry
	// Has the same recurrence Id as the second instance of the originating and
	// this and all future instances are modified.
	TTime recurrenceId(TDateTime(2006, EJanuary, 7, 14, 0, 0, 0)); // January 8th 2pm
	TCalTime recurrenceIdCal;
	recurrenceIdCal.SetTimeLocalL(recurrenceId);
	
	const TDesC8& guidDes = KGuid;
	HBufC8* guid = guidDes.AllocL();

	// Entry type is appointment, group scheduling method none and modifying this and all future instances 
	CCalEntry* modifyingEntry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 1, recurrenceIdCal, CalCommon::EThisAndFuture);
	CleanupStack::PushL(modifyingEntry);

	// The modifying entry has a different description to the originating entry
	_LIT(KChildDescription,"A modifying entry");
	modifyingEntry->SetDescriptionL(KChildDescription);
	// The modifying entry modifies the originating entry's date by one day
	TTime childStartTime(TDateTime(2006, EJanuary, 8, 14, 0, 0, 0)); // January 9th 2pm
	TTime childEndTime(childStartTime + TTimeIntervalHours(1)); // January 9th 3pm
	
	TCalTime childEntryStartTime;
	childEntryStartTime.SetTimeLocalL(childStartTime);
	TCalTime childEntryEndTime;
	childEntryEndTime.SetTimeLocalL(childEndTime);
	modifyingEntry->SetStartAndEndTimeL(childEntryStartTime, childEntryEndTime);
	// Target instance for recurrence range in the modifying entry is second instance
	TCalRRule childRptRule(TCalRRule::EWeekly);
	childRptRule.SetDtStart(childEntryStartTime);
	childRptRule.SetCount(2); 
	childRptRule.SetInterval(1);
	
	// The modifying entry modifies the originating entry by occurring on Mondays
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EMonday);
	childRptRule.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	modifyingEntry->SetRRuleL(childRptRule);

	// add the modifying entry to the entry view.
	modifyingArray.AppendL(modifyingEntry);

	TInt success(0);
	// If StoreL() leaves, 'success' contains the number of entries that were
	// stored before it failed
	iCalEntryView->StoreL(modifyingArray, success); 

	// clean up	
	CleanupStack::PopAndDestroy(modifyingEntry);
	modifyingArray.Reset();
	CleanupStack::PopAndDestroy(&modifyingArray);	
	}
	
// Searches for all instances in January, then prints out their details.
// Creates an instance view to display the instances. 
void CCalExample::FindInstanceL()
	{ 
	_LIT(KPressAKeyMsg, "\n\nPress any key to view the instances");
	iConsole->Printf(KPressAKeyMsg);
	iConsole->Getch ();
	_LIT(KProgressMsg, "\nCalendar instance view creation is in progress...");
	iConsole->Printf(KProgressMsg);		
	CCalInstanceView* instanceView = CCalInstanceView::NewL(*iCalSession, *this);
	CleanupStack::PushL(instanceView);
	CActiveScheduler::Start();
	
	RPointerArray<CCalInstance> instanceList;
	CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &instanceList));
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TCalTime startDateForInstanceSearch;
	TCalTime endDateForInstanceSearch;
	
	// Set date and time range for searching for instances
	startDateForInstanceSearch.SetTimeLocalL(TDateTime( 2006, EJanuary, 0, 12, 0, 0, 0));  // 1st January 12 pm
	endDateForInstanceSearch.SetTimeLocalL(TDateTime( 2006, EJanuary, 30, 10, 0, 0, 0)); //  31st January 10 am
	CalCommon::TCalTimeRange searchTimeRange(startDateForInstanceSearch, endDateForInstanceSearch);
	
	// Searches for instances within the specified date range
	instanceView->FindInstanceL(instanceList, filter, searchTimeRange);
	
	_LIT(KInstancesFound,"\n\nFound %d instances:");
	iConsole->Printf(KInstancesFound,instanceList.Count());
		
	TDateTime date; 
	for(TInt i = 0; i<instanceList.Count(); i++) 
		{
		TPtrC des = instanceList[i]->Entry().DescriptionL();
		_LIT(KEntryDesc,"\nDescription: %S");
		iConsole->Printf(KEntryDesc,&des); 

		TTime time = instanceList[i]->StartTimeL().TimeLocalL();
		TBuf<40> datetimeStr;		
		_LIT(KFormat,"%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S");
		time.FormatL(datetimeStr,KFormat);
		
		// Print out the start date/time
		_LIT(KStartDate,"\nLocal instance start date/time: %S ");
		iConsole->Printf(KStartDate,&datetimeStr);
		}
		
	CleanupStack::PopAndDestroy(&instanceList);	
	CleanupStack::PopAndDestroy(instanceView);	
	}	

// Fetch and print the entry details
void CCalExample::PrintEntryDetailsL()
	{
	_LIT(KPressAKeyMsg, "\nPress any key to view entry details");
	iConsole->Printf ( KPressAKeyMsg );
	iConsole->Getch ();

	RPointerArray<CCalEntry> array;
	CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &array));
	iCalEntryView->FetchL(KGuid, array);
	for(TInt i=0; i<array.Count(); i++)
		{
		CCalEntry* entry = array[i];
		CleanupStack::PushL(entry);
		
		TPtrC des = entry->DescriptionL();
		_LIT(KEntryDesc," \n\nDescription: %S");
		iConsole->Printf(KEntryDesc,&des); 

		TCalTime startTime = entry->StartTimeL();
		TCalTime endTime = entry->EndTimeL();
		TTime time = startTime.TimeLocalL();
		
		TBuf<40> datetimeStr;		
		_LIT(KFormat,"%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S");
		time.FormatL(datetimeStr,KFormat);
		
		// Print out the start date/time
		_LIT(KStartDate,"\nStart date/time: %S ");
		iConsole->Printf(KStartDate,&datetimeStr);
		
		// Now, print out the end date/time
		time = endTime.TimeLocalL();
		time.FormatL(datetimeStr,KFormat);
		_LIT(KEndDate,"\nEnd date/time: %S");
		iConsole->Printf(KEndDate,&datetimeStr);
		CleanupStack::Pop(entry);
		
		// Fetch category
       		RPointerArray<CCalCategory> categoryList = entry->CategoryListL();
    		for( int j=0; j<categoryList.Count(); j++)
			{
			const TDesC& category = categoryList[j]->ExtendedCategoryName();
			HBufC* catbuf = category.AllocL();
			_LIT(KCatType,"\nCategory type: %S");
			iConsole->Printf(KCatType,catbuf);
			delete catbuf;
			} 
			   	
		// Fetch attendee
		RPointerArray<CCalAttendee> attendeeList;
    		attendeeList = entry->AttendeesL();
    		for( int k=0; k<attendeeList.Count(); k++)
			{
    			const TDesC16& address = attendeeList[k]->Address();
    			HBufC16* addr = address.AllocL();
    			_LIT(KAttendee,"\nAttendee %d: %S");
    			iConsole->Printf(KAttendee,k+1, addr);
    			delete addr;		
    			}
		}
	CleanupStack::PopAndDestroy(&array);
	}

// Delete the originating entry	
void CCalExample::DeleteEntryL()
	{
	_LIT(KPressAKeyMsg, "\n\nPress any key to delete the originating entry, (also deletes the modifying entry)");
	iConsole->Printf ( KPressAKeyMsg );
	iConsole->Getch ();
	
	RPointerArray<CCalEntry> entryArray;
	CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &entryArray));
	
	// Fetch the entry to be deleted
	iCalEntryView->FetchL(KGuid, entryArray);
	CCalEntry* entry = entryArray[0];

	// Delete originating entry, thereby also deleting the modifying entry
	iCalEntryView->DeleteL(*entry);
	CleanupStack::PopAndDestroy(&entryArray); 			
	}

LOCAL_C void MainL()
	{
	CCalExample* app = CCalExample::NewL();
	CleanupStack::PushL(app);
	
	// Add an entry and display the entry details
	app->AddEntryL();
	app->PrintEntryDetailsL();	

	// Update an entry and display the updated entry details
	app->UpdateEntryL();
	app->PrintEntryDetailsL();
	
	// Create a repeating entry and display the entry details
	app->AddOriginatingEntryL();
	app->PrintEntryDetailsL(); 
	
	// Display the repeat instances
	app->FindInstanceL();		
	
	// Create a modifying entry and display the entry details
	app->AddmodifyingEntryL();
	app->PrintEntryDetailsL(); 
	
	// Display the repeat instances
	app->FindInstanceL();
	
	// Delete the originating thereby deleting the modifying entry and all repeat instances
	app->DeleteEntryL();		 
	
 	CleanupStack::PopAndDestroy(app);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	// Active scheduler required as this is a console app
	CActiveScheduler* scheduler=new CActiveScheduler;
	
	// If active scheduler has been created, install it.
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler); 
		
		// Cleanup stack needed
		CTrapCleanup* cleanup = CTrapCleanup::New();
		if(cleanup)
    			{
			TRAPD(err, MainL());
			if(err != KErrNone)
				{
				_LIT(KUserPanic,"Failed to complete");	
				User::Panic(KUserPanic, err);
				}
			delete cleanup;
    			}
		}
	delete scheduler;

	__UHEAP_MARKEND;
	return KErrNone;
	}
