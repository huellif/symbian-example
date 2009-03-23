// TZExample.cpp
//
// Copyright (c) 2005 Symbian Software Ltd.  All rights reserved.

// This example demonstrates using the time zone server (RTz) 
// to convert a local time to UTC time, then to convert the current 
// time for one time zone to another time zone, 

#include <e32base.h>
#include <e32cons.h>
#include <tz.h>
#include <tzconverter.h>

// Local definitions 
static CConsoleBase* console;
static RTz tzServer;
// Date/time format string
_LIT(KDateTimeString, "%*E%*D%X%*N%*Y %1 %2 '%3 %H%:1%T%:2%S");

// length of string to hold the date/time for display
static const TInt KMaxDateTimeStringLength	= 30;
// string to hold the date/time for display
static TBuf<KMaxDateTimeStringLength> dateTimeString;
// length of string to hold the time zone for display
const TInt KMaxTimeZoneStringLength	= 20;

// converts a UTC time to local time
static void ConvertUtcToLocalTimeL()
	{
	// randomly choose a historical utc date/time, format it and print it out
	TTime time(TDateTime(2001,EJuly,20,10,0,0,0));
	time.FormatL(dateTimeString, KDateTimeString);
	_LIT(KConsoleMessage,"\n A random date / time in UTC is %S");
	console->Printf(KConsoleMessage,&dateTimeString);	
	
	// now convert the date/time to local time for Australia/Sydney time zone
	_LIT8(KAustraliaSydney,"Australia/Sydney");
	CTzId* timezoneId = CTzId::NewL(KAustraliaSydney);
	CleanupStack::PushL(timezoneId);
	
	TInt results=tzServer.ConvertToLocalTime(time,*timezoneId);
	if(results == KErrNone)
	// conversion successful
		{
		TBuf<KMaxTimeZoneStringLength> timeZoneString;
		timeZoneString.Copy(timezoneId->TimeZoneNameID());
		time.FormatL(dateTimeString, KDateTimeString);
		_LIT(KConsoleMessage1,"\n The date and time in the %S time zone is %S");
		console->Printf(KConsoleMessage1,&timeZoneString, &dateTimeString);	
		}
	else
		{
		_LIT(KConsoleMessage2,"\n Error %d while converting ");
		console->Printf(KConsoleMessage2, results);	
		}
	CleanupStack::PopAndDestroy(timezoneId);		
	}
	
// convert the current local time for the system time zone 
// to local time for another time zone.
static void ConvertLocalToLocalTimeL()
	{
	// first, get the current local time, format it and print it out
	TTime time;
	time.HomeTime();
	time.FormatL(dateTimeString, KDateTimeString);
	_LIT(KConsoleMessage3,"\n\n The current local time is %S");
	console->Printf(KConsoleMessage3,&dateTimeString);
	// this is a 2 stage process - first convert the date/time to UTC.
	// No need to specify the time zone because 
	// this function uses the current system time zone.
	TInt results=tzServer.ConvertToUniversalTime(time);
	if(results == KErrNone)
		{
		//conversion successful
		time.FormatL(dateTimeString, KDateTimeString);
		_LIT(KConsoleMessage4,"\n The current UTC time is %S");
		console->Printf(KConsoleMessage4, &dateTimeString);	
		}
	else
		{
		_LIT(KConsoleMessage5,"\n Error %d while converting ");
		console->Printf(KConsoleMessage5, results);	
		}

	_LIT8(KAustraliaSydney,"Australia/Sydney");
	// create a timezone ID object
	CTzId* timezoneId = CTzId::NewL(KAustraliaSydney);
	CleanupStack::PushL(timezoneId);
	// convert UTC time to local time for Australia/Sydney
	results=tzServer.ConvertToLocalTime(time,*timezoneId);
	if(results == KErrNone)
		// conversion successful
		{
		TBuf<KMaxTimeZoneStringLength> timeZoneString;
		timeZoneString.Copy(timezoneId->TimeZoneNameID());
		time.FormatL(dateTimeString, KDateTimeString);
		_LIT(KConsoleMessage6,"\n The current date and time in the %S time zone is %S");
		console->Printf(KConsoleMessage6,&timeZoneString, &dateTimeString);	
		}
	else
		{
		_LIT(KConsoleMessage7,"\n Error %d while converting ");
		console->Printf(KConsoleMessage7, results);	
		}
	CleanupStack::PopAndDestroy(timezoneId);	
	}
	
static void ConvertTimeL()
	// Connects to the time zone server and does the conversion
	{
	User::LeaveIfError(tzServer.Connect());
	CleanupClosePushL(tzServer);
	//do the conversions
	ConvertUtcToLocalTimeL();
	ConvertLocalToLocalTimeL();
	CleanupStack::PopAndDestroy(1); // causes tzServer.Close() to be called
	}
	
static void DoExampleL()
	{
	_LIT(KTxtConsoleTitle, "Time Zone example");
	// Create the console object to print the messages. 
	console = Console::NewL(KTxtConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
   	TRAPD(err,ConvertTimeL());
	if (err)
		{
		_LIT(KFailed,"\n\nConversion failed: leave code=%d");
		console->Printf(KFailed, err);
		}
	// wait for user to press a key before destroying console
 	_LIT(KMsgPressAnyKey,"\n\nPress any key to continue\n\n");
	console->Printf(KMsgPressAnyKey);
 	console->Getch();
 	CleanupStack::PopAndDestroy(console);
	}

// Standard entry point function
TInt E32Main()
	{
	__UHEAP_MARK;
	// Active scheduler required as this is a console app
	CActiveScheduler* scheduler=new CActiveScheduler;
	// If active scheduler has been created, install it.
	if (scheduler)
		{
 		CActiveScheduler::Install(scheduler); 
		// Cleanup stack needed
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			TRAP_IGNORE(DoExampleL());
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }
  


