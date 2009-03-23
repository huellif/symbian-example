// ClockExample.h
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#ifndef __CLOCKEXAMPLE_H__
#define __CLOCKEXAMPLE_H__


#include <coeccntx.h>

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>

#include <eikstart.h> 

#include <clock.h>

#include <ClockExample.rsg>
#include "ClockExample.hrh"

#include <MyMbs0.mbg>

_LIT(KMyMBS,"z:\\system\\data\\MYMBS0.MBM");

#define CLOCK 0  
#define CLOCKMSK 1 

enum TTimezoneOffset
	{
	EUTOffsetOneHour = ( 60 * 60 ),
	EUTOffsetHalfHour = ( 30 * 60 ),
	EUTOffsetQuarterHour = ( 15 * 60 ),
	EUTOffsetTenMinutes = ( 10 * 60 ),
	EUTOffsetFiveMinutes = ( 5 * 60 ),
	EUTOffsetOneMinute = ( 1 * 60 ),
	EUTOffsetThirtySeconds = ( 30 ),
	EUTOffsetFifteenSeconds = ( 15 ),
	EUTOffsetTenSeconds = ( 10 ),
	EUTOffsetFiveSeconds = ( 5 ),
	EUTOffsetOneSecond = ( 1 ),

	EUTOffsetLondon = 0,
	EUTOffsetLondonSummer = EUTOffsetLondon + EUTOffsetOneHour,
	EUTOffsetBangalore = 5 * EUTOffsetOneHour + EUTOffsetHalfHour		
	};

// Destructable font for Digital clock
class CDestructableFont : public CBase
	{
public:
	CDestructableFont(CWsScreenDevice* aScreenDevice);
	void ConstructL(const TFontSpec& aFontSpec);
	virtual ~CDestructableFont();
	TInt Handle() const;
private:
	CWsScreenDevice* iScreenDevice; // not owned by this pointer
	CFbsFont* iFont;
	};
	
		
// Application class	
class CExampleApplication : public CEikApplication
	{
private: 
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};

// View class
class CExampleAppView : public CCoeControl
    {
public:
	static CExampleAppView* NewL( const TRect& aRect );
	CExampleAppView();
	~CExampleAppView();
	void ConstructL( const TRect& aRect );
    
	void DoExperimentL( TInt aCommand );
	void ConstructAnalogClockL();
	void ConstructDigitalClockL();

private:
	void Draw(const TRect& /*aRect*/) const;

private:
	HBufC*  iTextBuffer;
	RAnalogClock* iAnalogClock;
	RDigitalClock* iDigitalClock;
	TPoint iDigitalClockPosition;
	TPoint iAnalogClockPosition;
	TSize iDigitalClockSize;
	TSize iAnalogClockSize;
	TInt iDigitalClockUniversalTimeOffset;
	TInt iAnalogClockUniversalTimeOffset;
	RAnimDll iAnimDll;
	
	TRgb iBackgroundColor;
	CFbsFont* iFont;
    };

// Application UI class
class CExampleAppUi : public CEikAppUi
    {
public:
	void ConstructL();
	~CExampleAppUi();

private:
	void HandleCommandL(TInt aCommand);

private:
	CExampleAppView* iAppView;
	};

// Document class
class CExampleDocument : public CEikDocument
	{
public:
	static CExampleDocument* NewL(CEikApplication& aApp);
	CExampleDocument(CEikApplication& aApp);
	void ConstructL();
private: 
	CEikAppUi* CreateAppUiL();
	};

#endif // __CLOCKEXAMPLE_H__
