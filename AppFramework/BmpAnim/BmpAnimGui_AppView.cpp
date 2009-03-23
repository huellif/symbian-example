// BmpAnimGuiAppView.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include "BmpAnimGui.h"

	
// Animate the balls such that they move across the screen in a sinusoidal pattern
void CExampleAppView::LoadMbmL( const TDesC& aMbmFile )
	{
	// Construct BitmapAnimClientData 
	if( !iAnimFrames )
		{
		CFbsBitmap* bitmap = NULL;
		CFbsBitmap* mask = NULL;
		CBitmapFrameData* frameGeneral = NULL;
		iAnimFrames = CBitmapAnimClientData::NewL();
		CleanupStack::PushL( iAnimFrames );
		
		iX = KColorBallsAnimXDeflectionStart;
		TInt ballIndex = KColorBallsMbmFirstBallIndex;
		
		while( iX < KColorBallsAnimXDeflectionTerminalValue )
			{
			// Load the image
			bitmap = new (ELeave) CFbsBitmap();
			CleanupStack::PushL( bitmap );
			User::LeaveIfError( bitmap->Load(aMbmFile, ballIndex) );		
			ballIndex = ( KColorBallsMbmLastBallIndex == ballIndex ) ? KColorBallsMbmFirstBallIndex : ballIndex + 1;
				
			// Load the mask
			mask = new (ELeave) CFbsBitmap();
			CleanupStack::PushL( mask );
			User::LeaveIfError( mask->Load(aMbmFile, KColorBallsMbmMaskIndex) );		
				
			// Calculate the y deflection
			iY =TInt( KColorBallsAnimYDeflectionOffset + KColorBallsAnimYDeflectionMultiplier*sin( iX ));	
				
			// Construct the frame
			frameGeneral = CBitmapFrameData::NewL( bitmap, mask, KColorBallsAnimFrameDelay, TPoint(iX, iY) );
			CleanupStack::PushL( frameGeneral );
				
			// Add it to the sequence
			iAnimFrames->AppendFrameL( frameGeneral );
				
			// Popping order- mask, bitmap, iFrameGeneral
			CleanupStack::Pop( 3 );	
				
			iX += KColorBallsAnimXDeflectionIncrement;
			}
			
		iBitmapAnimPlayer.SetBitmapAnimDataL( *iAnimFrames );
		CleanupStack::Pop( iAnimFrames );
		}
	}	
	
CExampleAppView::CExampleAppView()
: iBitmapAnimPlayer( iAnimDll ),iAnimDll( iEikonEnv->WsSession() )
  
	{
	}

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
	iBitmapAnimPlayer.Close();
	delete( iAnimFrames );
	iAnimDll.Close();
	delete iExampleText;
	}


// Standard initialisation for a window-owning control.
void CExampleAppView::ConstructL(const TRect& aRect)
	{
	// Fetch the text from the resource file.
	iExampleText = iEikonEnv->AllocReadResourceL( R_EXAMPLE_TEXT_BB );
	
	// The control is window-owning.
	CreateWindowL();
	
	// Extent of the control. 
	SetRect(aRect);
	
	// The control is ready to draw, so notify the UI framework.
	ActivateL();
	
	// Some animation preamble.
	_LIT(KDllName, "BMPANSRV.DLL" );
	User::LeaveIfError( iAnimDll.Load( KDllName ) );
	iBitmapAnimPlayer.ConstructL( Window() );
	
	LoadMbmL( KColorBallsMbmPath );	
	}	
	
					  

// Draws the view with a simple outline rectangle 
void CExampleAppView::Draw(const TRect& /*aRect*/) const
	{
	// Window graphics context
	CWindowGc& gc = SystemGc();
	
	// Area in which we shall draw
	TRect drawRect = Rect();
	
	// The font used for drawing the text
	const CFont* fontUsed;
	
	// Clear the screen
	gc.Clear();
	    
	// Use the title font supplied by the UI
	fontUsed = iEikonEnv->TitleFont();
	gc.UseFont(fontUsed);
	
	// Draw the text in the rectangle and discard the font.
	TInt   baselineOffset=(drawRect.Height() - fontUsed->HeightInPixels())/5; 
	gc.DrawText(*iExampleText,drawRect,baselineOffset,CGraphicsContext::ECenter, 0);
	gc.DiscardFont();		
	}

	
	







	
	





