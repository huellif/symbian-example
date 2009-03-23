// ClockExample_AppView.cpp
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#include "ClockExample.h"


// Menu operations
void CExampleAppView::DoExperimentL( TInt aCommand )
	{

	if( aCommand < ETestDo10 )
		{
		// Digital clock commands
		switch( aCommand )
			{
		case ETestDo0:
			iDigitalClockPosition.iX -= 5;
			iDigitalClock->SetPosition( iDigitalClockPosition );
			break;
			
		case ETestDo1:
			iDigitalClockPosition.iX += 5;
			iDigitalClock->SetPosition( iDigitalClockPosition );
			break;
			
		case ETestDo2:
			iDigitalClock->SetVisible( EFalse );
			break;

		case ETestDo3:
			iAnalogClock->SetVisible( EFalse );
			iDigitalClock->SetVisible( ETrue );			
			break;

		case ETestDo4:
			iDigitalClockUniversalTimeOffset += ( EUTOffsetOneHour );
			iDigitalClock->SetUniversalTimeOffset( iDigitalClockUniversalTimeOffset );
			break;
		case ETestDo5:
			iDigitalClockUniversalTimeOffset += ( EUTOffsetHalfHour );
			iDigitalClock->SetUniversalTimeOffset( iDigitalClockUniversalTimeOffset );
			break;
			
		case ETestDo6:
			iDigitalClockUniversalTimeOffset += ( EUTOffsetOneMinute );
			iDigitalClock->SetUniversalTimeOffset( iDigitalClockUniversalTimeOffset );
			break;

		case ETestDo7:
			iDigitalClockUniversalTimeOffset += ( EUTOffsetOneSecond );
			iDigitalClock->SetUniversalTimeOffset( iDigitalClockUniversalTimeOffset );
			break;
									
		default:
			break;	
			}
		}
	else
		{
		// Analogue clock commands
		switch( aCommand )
			{
				
		case ETestDo10:
			iAnalogClockPosition.iX -= 5;
			iAnalogClock->SetPosition( iAnalogClockPosition );		
			break;
			
		case ETestDo11:
			iAnalogClockPosition.iX += 5;
			iAnalogClock->SetPosition( iAnalogClockPosition );		
			break;
			
		case ETestDo12:	
			iAnalogClock->SetVisible( EFalse );
			break;

		case ETestDo13:
			iDigitalClock->SetVisible( EFalse );
			iAnalogClock->SetVisible( ETrue );
			break;

		case ETestDo14:
			iAnalogClockUniversalTimeOffset += ( EUTOffsetOneHour );
			iAnalogClock->SetUniversalTimeOffset( iAnalogClockUniversalTimeOffset );		
			break;

		case ETestDo15:
			iAnalogClockUniversalTimeOffset += ( EUTOffsetHalfHour );
			iAnalogClock->SetUniversalTimeOffset( iAnalogClockUniversalTimeOffset );		
			break;
			
		case ETestDo16:
			iAnalogClockUniversalTimeOffset += ( EUTOffsetOneMinute );
			iAnalogClock->SetUniversalTimeOffset( iAnalogClockUniversalTimeOffset );		
			break;
			
		case ETestDo17:
			iAnalogClockUniversalTimeOffset += ( EUTOffsetOneSecond );
			iAnalogClock->SetUniversalTimeOffset( iAnalogClockUniversalTimeOffset );		
			break;			
			
		default:
			break;	
			}
		}
	}

// Construct an Analog clock
void CExampleAppView::ConstructAnalogClockL()
	{
	
	iAnalogClockPosition.SetXY( 150, 20 );
	iAnalogClockSize.SetSize( 300, 222 );
	iAnalogClockUniversalTimeOffset = EUTOffsetBangalore;
	
	CWsScreenDevice* screenDevice = iCoeEnv->ScreenDevice();
	
	// Set the margins for cropping the picture
	TMargins margins;
	margins.iLeft = 50;
	margins.iRight = 50;
	margins.iTop = 50;
	margins.iBottom = 50;
	
	// Set shadow for analog clock
	STimeDeviceShadow  analogClockShadow;
	analogClockShadow.iIsOn = ETrue;
	analogClockShadow.iColor = KRgbGray;
	analogClockShadow.iOffset.SetXY( 3, 3 );

	// Load the bitmap image of an analog clock 
	CFbsBitmap* analogClockDial=new( ELeave ) CFbsBitmap;
	CleanupStack::PushL( analogClockDial );
	User::LeaveIfError( analogClockDial->Load( KMyMBS, CLOCK ) );
	analogClockDial->SetSizeInTwips( screenDevice );
	
	
	CFbsBitmap* analogClockDialMask = new( ELeave ) CFbsBitmap;
	CleanupStack::PushL( analogClockDialMask );
	User::LeaveIfError( analogClockDialMask->Load(KMyMBS, CLOCKMSK) );
	analogClockDialMask->SetSizeInTwips( screenDevice );

	// Create an analog clock
	iAnalogClock=new( ELeave ) RAnalogClock( iAnimDll, Window() );
	iAnalogClock->ConstructL( TTimeIntervalSeconds( iAnalogClockUniversalTimeOffset ), iAnalogClockPosition, 
							  iAnalogClockSize, margins, analogClockShadow,
							  analogClockDial->Handle(), analogClockDialMask->Handle(), 3 );


	// Set the hour hand 
	TAnalogDisplayHand analogClockHourHand(EAnalogDisplayHandOneRevPer12Hours);
	analogClockHourHand.AddLine(CGraphicsContext::ESolidPen, KRgbBlack, TSize(4, 4), TPoint(0, 10), TPoint(0, -55));
	iAnalogClock->AddHandL(analogClockHourHand);

	// Set the minute hand
	TAnalogDisplayHand analogClockMinuteHand(EAnalogDisplayHandOneRevPerHour);
	analogClockMinuteHand.AddLine(CGraphicsContext::ESolidPen, KRgbBlack, TSize(3, 3), TPoint(0, 10), TPoint(0, -87));
	iAnalogClock->AddHandL(analogClockMinuteHand);

	// Set the second hand
	TAnalogDisplayHand analogClockSecondsHand(EAnalogDisplayHandOneRevPerMinute);
	analogClockSecondsHand.AddLine( CGraphicsContext::ESolidPen, KRgbBlack, TSize(1, 1), TPoint(0, 10), TPoint(0, -90) );
	iAnalogClock->AddHandL(analogClockSecondsHand);	
	
	CleanupStack::PopAndDestroy(); // analogClockDialMask
	CleanupStack::PopAndDestroy(); // analogClockDial
		
	iAnalogClock->SetVisible( ETrue );	
	}
	
	
// Construct a Digital clock
void CExampleAppView::ConstructDigitalClockL()
	{
	iDigitalClockPosition.SetXY( 200, 100 );
	iDigitalClockSize.SetSize( 200, 70 );
	iDigitalClockUniversalTimeOffset = EUTOffsetBangalore;
		
	CWsScreenDevice* screenDevice = iCoeEnv->ScreenDevice();
	
	// Set the margins for cropping the picture
	TMargins margins;
	margins.iLeft = 50;
	margins.iRight = 50;
	margins.iTop = 50;
	margins.iBottom = 50;
	
	// Set shadow for digital clock
	STimeDeviceShadow digitalClockShadow;
	digitalClockShadow.iIsOn = ETrue;
	digitalClockShadow.iColor = KRgbGray;
	digitalClockShadow.iOffset.SetXY( 3, 3 );

	iDigitalClock = new(ELeave) RDigitalClock( iAnimDll, Window() );
	iDigitalClock->ConstructL( TTimeIntervalSeconds( iDigitalClockUniversalTimeOffset ), iDigitalClockPosition, iDigitalClockSize, margins, digitalClockShadow, TRgb(170, 170, 170),/*1*/ 2 );

	CDestructableFont* digitalClockFont = new( ELeave ) CDestructableFont( screenDevice );
	CleanupStack::PushL( digitalClockFont );
	
	_LIT(KArial,"arial");
	TFontSpec digitalClockFontSpec( KArial, screenDevice->VerticalPixelsToTwips(25) );
	digitalClockFontSpec.iFontStyle.SetPosture( EPostureItalic );
	digitalClockFontSpec.iFontStyle.SetStrokeWeight( EStrokeWeightBold );
	
	digitalClockFont->ConstructL( digitalClockFontSpec );
	
	// Set the time format
	_LIT(KDigitalClockTimeFormat,"%-B%J%:1%T:%S% %+B");
	iDigitalClock->AddTextSectionL( TDigitalDisplayTextSection(digitalClockFont->Handle(), TRgb(0, 0, 0),
																EDigitalDisplayHorizontalTextAlignmentLeft,
																EDigitalDisplayVerticalTextAlignmentTop,
																30, 15, KDigitalClockTimeFormat) );

	// Set the date format
	_LIT(KDigitalClockDateFormat,"%*E %F%*D %Y");
	iDigitalClock->AddTextSectionL( TDigitalDisplayTextSection(digitalClockFont->Handle(), TRgb(0, 0, 0),
																EDigitalDisplayHorizontalTextAlignmentLeft,
																EDigitalDisplayVerticalTextAlignmentBottomExclDescent,
																30, 15, KDigitalClockDateFormat) );


	CleanupStack::PopAndDestroy(); // digitalClockFont
	
	iDigitalClock->SetBackgroundColor( KRgbDarkGray, KRgbBlack );
	iDigitalClock->SetTextColor( KRgbWhite );
	iDigitalClock->SetVisible( EFalse );
	}


CExampleAppView::CExampleAppView()
: iAnimDll( iCoeEnv->WsSession() )
	{}

// Static function wraps up two-phase construction for the view.
CExampleAppView* CExampleAppView::NewL(const TRect& aRect)
	{
	CExampleAppView* self = new(ELeave) CExampleAppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}

CExampleAppView::~CExampleAppView()
	{
	delete iTextBuffer;
	delete iAnalogClock;
	delete iDigitalClock;
		
	iAnimDll.Close();
	}

// Standard initialisation for a window-owning control.
void CExampleAppView::ConstructL( const TRect& aRect )
    {
	// Fetch the text from the resource file.
	iTextBuffer = iEikonEnv->AllocReadResourceL( R_TITLE_TEXT_CLOCK_EXAMPLE );

   
	// Create the window owned by the view.
	CreateWindowL();
	// Set the view's size and position.
	SetRect(aRect);
	// Activate the view.
	ActivateL();
	
	_LIT(KClockDll,"CLOCKA.DLL");
	User::LeaveIfError( iAnimDll.Load(KClockDll) );
	
	ConstructDigitalClockL();
	ConstructAnalogClockL();
	}

// Draws the view with a simple outline rectangle and then
// draws the welcome text centred.
void CExampleAppView::Draw(const TRect& /*aRect*/) const
	{
	const TRect rect = Rect();
	CWindowGc& gc = SystemGc();
		
	// Font used for drawing text
	const CFont*     fontUsed;
	// Use the title font supplied by the UI
	fontUsed = iEikonEnv->TitleFont();
	gc.UseFont( fontUsed );

	// Draw the title text.
	gc.DrawText( *iTextBuffer, rect, 
				 0+ fontUsed->HeightInPixels(), 
				 CGraphicsContext::ECenter, 0 );
	
	gc.DiscardFont();
	}


CDestructableFont::CDestructableFont(CWsScreenDevice* aScreenDevice)
	:iScreenDevice(aScreenDevice)
	{}

void CDestructableFont::ConstructL(const TFontSpec& aFontSpec)
	{
	User::LeaveIfError(iScreenDevice->GetNearestFontInTwips((CFont*&)iFont, aFontSpec));
	}

CDestructableFont::~CDestructableFont()
	{
	iScreenDevice->ReleaseFont(iFont); 
	}

TInt CDestructableFont::Handle() const
	{
	return iFont->Handle();
	}
