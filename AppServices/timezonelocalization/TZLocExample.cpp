// TZLocExample.cpp
//
// Copyright (c) 2005 Symbian Software Ltd.  All rights reserved.

// This examples demonstrates using the TimeZone Localization API 
// (CTzLocalizer and associated classes) to retrieve a list of time zones 
// and to get information about a particular time zone. It adds a city 
// to a time zone/city group.

#include <e32base.h>
#include <e32cons.h>
#include <TzLocalizer.h> 
#include <TzLocalizationDataTypes.h>

// local definitions 
static CConsoleBase* console;
static CTzLocalizer* localizer;
static TInt timeZoneId;
static TInt cityGroupId;

// length of buffer to hold city name
const TInt KMaxCityBufLength = 26;

// strings for display
_LIT(KConsoleNewLine,"\n");
_LIT(KMsgPressAnyKey,"\n\nPress any key to continue\n");

// Sort and print out all time zones
static void GetAllTimeZonesL()
	{
	// Use standard names in alphabetical order
	CTzLocalizedTimeZoneArray* timeZones = localizer->GetAllTimeZonesL(CTzLocalizer::ETzAlphaStandardNameAscending);

	TInt count= timeZones->Count();
	_LIT(KConsoleMessage,"\nAll timezones:");	
	console->Printf(KConsoleMessage);	
	for(TInt i=0; i<count; ++i)
		{
		TPtrC temp(timeZones->At(i).StandardName());
		// if the time zone has a standard name, print it and the short name
		if ((&temp)->Length()!=0)
			{
			console->Printf(KConsoleNewLine);
			console->Printf(temp);
			_LIT(KConsoleTab,"\t");
			console->Printf(KConsoleTab);
			console->Printf(timeZones->At(i).ShortStandardName());
			}
		} 
		
	delete timeZones;
 	console->Printf(KMsgPressAnyKey);
 	console->Getch();
	}

// Add a city to a specific time zone and city group
static void AddCityL()
	{
	// Add the new city to the same time zone and city group as London
	_LIT(KCityName,"London");
	TBufC<KMaxCityBufLength> cityBuf(KCityName);
	TPtrC cityName(cityBuf);
	// First, find the time zone and city group that London is in 
	CTzLocalizedCity* city=localizer->FindCityByNameL(cityName);
	if (!city)
	// city doesn't exist
		User::Leave(KErrNotFound);
	
	// Get the city's time zone ID and city group ID
	timeZoneId = city->TimeZoneId();
	cityGroupId = city->GroupId();
	delete city;

	// Now add the new city to that time zone/city group.
	// First check it is not already present
	CTzLocalizedCityArray* cities=localizer->GetCitiesL(timeZoneId);
	CleanupStack::PushL(cities);
	_LIT(KNewCityName,"Cambridge");
	TBufC<KMaxCityBufLength> newCityBuf(KNewCityName); 
	TPtrC newCityName(newCityBuf);
	TBool present=EFalse;
	CTzLocalizedCity* temp;
	// (AddCityL() leaves if the city has already been added, so trap the leave)
	TRAPD(err,temp=localizer->AddCityL(timeZoneId,newCityName,cityGroupId));
	// Ignore return value
	if (err==KErrNone)
			delete temp;
	CleanupStack::PopAndDestroy(cities);
	}
	
// print out all the cities in the time zone
static void GetCitiesInTimeZoneL()
	{
	CTzLocalizedCityArray* tzCities=localizer->GetCitiesL(timeZoneId);
	CleanupStack::PushL(tzCities);
	// get the standard name of the time zone and print it
	CTzLocalizedTimeZone* tz = localizer->GetLocalizedTimeZoneL(timeZoneId);
	_LIT(KConsoleMessage,"\nThe cities in the %S time zone are :");
	TPtrC tzName = tz->StandardName();
	console->Printf(KConsoleMessage,&tzName);
	delete tz;
	
	TInt tzCount=tzCities->Count();
	for(TInt i=0; i<tzCount; ++i)
		{
		console->Printf(KConsoleNewLine);
		console->Printf(tzCities->At(i).Name());
		}

	CleanupStack::PopAndDestroy(tzCities);
 	console->Printf(KMsgPressAnyKey);
 	console->Getch();
	}
	
// print out all the cities in the city group
static void GetCitiesInGroupL()
	{
	CTzLocalizedCityArray* groupCities = localizer->GetCitiesInGroupL(cityGroupId);
	CleanupStack::PushL(groupCities);
	
	// get the city group name
	CTzLocalizedCityGroup* cityGroup=localizer->GetCityGroupL(cityGroupId);	
	_LIT(KConsoleMessage,"\nThe cities in the %S city group are :");	
	TPtrC groupName = cityGroup->Name();
	console->Printf(KConsoleMessage,&groupName);
	delete cityGroup;
	
	TInt groupCount=groupCities->Count();
	for(TInt i=0; i<groupCount; ++i)
			{
			console->Printf(KConsoleNewLine);
			console->Printf(groupCities->At(i).Name());
			}
	CleanupStack::PopAndDestroy(groupCities); 
	}
	

static void LocalizeTimeZonesL()
	{
	// Create an instance of CTzLocalizer
	localizer = CTzLocalizer::NewL();
	CleanupStack::PushL(localizer);
	GetAllTimeZonesL(); 
	AddCityL();
	GetCitiesInTimeZoneL();
	GetCitiesInGroupL();
	CleanupStack::PopAndDestroy(localizer);
	}
	

static void DoExampleL()
    {
	// Create the console to print the messages to. 
	_LIT(KConsoleMessageDisplay, "Time zone localisation example");
	_LIT(KConsoleStars,"\n*************************\n");
	console = Console::NewL(KConsoleMessageDisplay,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	console->Printf(KConsoleMessageDisplay);
	console->Printf(KConsoleStars);

   	TRAPD(err,LocalizeTimeZonesL());
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
  


