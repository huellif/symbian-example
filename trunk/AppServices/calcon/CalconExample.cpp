// CalconExample.cpp
//
// Copyright (c) 2005-2006 Symbian Software Ltd.  All rights reserved.

// This example demonstrates conversion of Chinese lunar calendar dates to Gregorian dates
// and vice versa

#include <e32std.h>
#include <e32base.h>
#include <e32cons.h>
#include <calendarconverter.h> 

// local definitions 
static CConsoleBase* console;
// Pointer to the CChineseCalendarConverter
static CChineseCalendarConverter*	gCalendarConverter;
// strings for display
_LIT(KMsgPressAnyKey,"\nPress any key to continue\n\n");
_LIT(KConsoleStars,"\n*************************\n");

static void PrintChineseDate(TChineseDate aDate);
static void PrintGregorianDate(TDateTime aDate);

// Converts a Gregorian date to a Chinese date
static TChineseDate& GregorianDatesToLunarDatesL(TDateTime aStartDT)
	{
	_LIT(KConsoleMessage,"This function demonstrates conversion of Gregorian to Chinese dates\n");
	console->Printf(KConsoleMessage);
		
	_LIT(KConsoleMessage1,"\nBefore conversion, the date in Gregorian format is: ");
	console->Printf(KConsoleMessage1);
	PrintGregorianDate(aStartDT);
  	
	TInt err1;
	TChineseDate chineseDate;
	// Convert the earliest date in the convertible range
	TRAP(err1,gCalendarConverter->DateTimeToChineseL(aStartDT,chineseDate));
	if (err1)
		{
		_LIT(KConsoleMessage2,"Conversion error");
		console->Printf(KConsoleMessage2);	
		}
	else
		{
		_LIT(KConsoleMessage3,"\nAfter conversion, the date in Chinese format is:\n");
		console->Printf(KConsoleMessage3);
  		PrintChineseDate(chineseDate);
      	}
   	console->Printf(KConsoleStars);
	// Wait for user to press a key before destroying console
 	console->Printf(KMsgPressAnyKey);
	console->Getch();
	return chineseDate;
	}
	
// Converts a Chinese date to a Gregorian date
static void LunarDatesToGregorianDatesL(TChineseDate aChineseDate)
	{
	_LIT(KConsoleMessage,"This function demonstrates conversion of Chinese to Gregorian dates\n");
	console->Printf(KConsoleMessage);
		
	TDateTime dateTime;

	_LIT(KConsoleMessage1,"\nBefore conversion, the date in Chinese format is:\n");
	console->Printf(KConsoleMessage1);
	PrintChineseDate(aChineseDate);

	TInt err;
	// Convert the last date in the convertible range
	TRAP(err,gCalendarConverter->ChineseToDateTimeL(aChineseDate, dateTime));
	if (err)
		{
		_LIT(KConsoleMessage2,"Conversion error");
		console->Printf(KConsoleMessage2);	
		}
	else
		{
		_LIT(KConsoleMessage3,"\nAfter conversion, the date is: ");
		console->Printf(KConsoleMessage3);

	   	PrintGregorianDate(dateTime);
        console->Printf(KConsoleStars);

		}
	}
	
// Prints Chinese date
static void PrintChineseDate(TChineseDate aDate)	
	{
	TBuf16<32> buffer;
	_LIT(KFormatTxt,"Cycle: %d \n");
  	buffer.Format(KFormatTxt,aDate.iCycle);
  	console->Printf(buffer);

	_LIT(KFormatTxt2,"Year: %d \n");
 	buffer.Format(KFormatTxt2,aDate.iYear);
  	console->Printf(buffer);
  
	_LIT(KFormatTxt3,"Month: %d \n");
 	buffer.Format(KFormatTxt3,aDate.iMonth);
  	console->Printf(buffer);
  
	_LIT(KFormatTxt4,"LeapMonth: %d \n");
 	buffer.Format(KFormatTxt4,aDate.iLeapMonth);
  	console->Printf(buffer);
  
	_LIT(KFormatTxt5,"Day: %d \n");
 	buffer.Format(KFormatTxt5,aDate.iDay);
  	console->Printf(buffer);
	}
	
// Prints Gregorian date
static void PrintGregorianDate(TDateTime aDate)
	{
	TBuf16<32> buffer;
	_LIT(KFormatTxt,"%d%S  %S %d\n");
    buffer.Format(KFormatTxt,aDate.Day()+1,&(TDateSuffix(aDate.Day())),&(TMonthName(aDate.Month())),aDate.Year());
    console->Printf(buffer);
	}
	
static void EgCalconL()
	{
	gCalendarConverter = CChineseCalendarConverter::NewL();
	CleanupStack::PushL(gCalendarConverter);
	
	TDateTime startDT;
	TDateTime finishDT;
	
	gCalendarConverter->DateRange(startDT, finishDT);
	
	TChineseDate chineseDate = GregorianDatesToLunarDatesL(startDT);
	LunarDatesToGregorianDatesL(chineseDate);
	
	CleanupStack::PopAndDestroy(gCalendarConverter); 
	}
	
static void DoExampleL()
    {
	// Create the console to print the messages to. 
	_LIT(KConsoleMessageDisplay, "Calcon Example");
	console = Console::NewL(KConsoleMessageDisplay,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	console->Printf(KConsoleMessageDisplay);
	console->Printf(KConsoleStars);

   	TRAPD(err,EgCalconL());
	if (err)
		{
		_LIT(KFailed,"\n\nExample failed: leave code=%d");
		console->Printf(KFailed, err);
		}
	// wait for user to press a key before destroying console
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
  


