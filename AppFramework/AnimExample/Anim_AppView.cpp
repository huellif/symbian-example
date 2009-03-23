// Anim_AppView.cpp
//
//  Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include "AnimExample.h"

CAnimationAppView::CAnimationAppView()
	{
	}

CAnimationAppView* CAnimationAppView::NewL(const TRect& aRect)
	{
	CAnimationAppView* self = new(ELeave) CAnimationAppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}

CAnimationAppView::~CAnimationAppView()
	{
	delete iAnimTestText;
	delete iSpriteAnim;
	delete iBasicAnim;
	
	if( iSpriteAnimMover )
		{
		iSpriteAnimMover->Cancel();	
		delete iSpriteAnimMover;
		}
			
	if( iBasicAnimMover )
		{
		iBasicAnimMover->Cancel();	
		delete iBasicAnimMover;
		}		
	}

// Standard initialisation for a window-owning control.
void CAnimationAppView::ConstructL( const TRect& aRect )
    {
    
	// Fetch the text from the resource file.
	iAnimTestText = iEikonEnv->AllocReadResourceL( R_EXAMPLE_TEXT_BASICANIM );
	
	// The control is window-owning.
	CreateWindowL();
   
	// Extent of the control.
	SetRect(aRect);
   
	// The control is ready to draw, so notify the UI framework.
	ActivateL();
	}


void CAnimationAppView::ProduceSpriteAnimL()
	{
	
	// Simple sprite animation
	CICLAnimationDataProvider* spriteDataProvider = new (ELeave)CICLAnimationDataProvider;
	CleanupStack::PushL( spriteDataProvider );
	spriteDataProvider->SetFileL( iEikonEnv->FsSession(), KAnimExGuitarPlayer() );
	CleanupStack::Pop( spriteDataProvider );
	
	iSpriteAnim = CSpriteAnimation::NewL( spriteDataProvider,TPoint(KAnimExSpritePositionX, KAnimExSpritePositionY), iEikonEnv->WsSession(), Window() );
	
	TAnimationConfig config;
	config.iFlags = TAnimationConfig::ELoop;
	config.iData = -1;
	
	iSpriteAnim->Start( config );
	}

void CAnimationAppView::ProduceBasicAnimL()
	{

	TAnimationConfig config;
	config.iFlags = TAnimationConfig::ELoop;
	config.iData = -1;

	// The basic animation. Drawn in ::Draw()
	CICLAnimationDataProvider* basicDataProvider = new (ELeave)CICLAnimationDataProvider;
	CleanupStack::PushL( basicDataProvider );
	basicDataProvider->SetFileL( iEikonEnv->FsSession(), KAnimExGuitarPlayer() );

	CleanupStack::Pop( basicDataProvider );
	
	iBasicAnim = CBasicAnimation::NewL( basicDataProvider,TPoint( KAnimExBasicPositionX, KAnimExBasicPositionY ), iEikonEnv->WsSession(), Window() );
		
	iBasicAnim->Start( config );	
	}

void CAnimationAppView::MoveAnimsL()
	{
	
	if( iSpriteAnimMover )
		{
		iSpriteAnimMover->Cancel();	
		delete iSpriteAnimMover;
		iSpriteAnimMover = NULL;	
		}
			
	iSpriteAnimMover = new (ELeave) CSpriteAnimMover( EPriorityNormal, KspriteInterval, iSpriteAnim );
	

	if( iBasicAnimMover )
		{
		iBasicAnimMover->Cancel();	
		delete iBasicAnimMover;
		iBasicAnimMover = NULL;	
		}
			
	iBasicAnimMover = new (ELeave) CBasicAnimMover( EPriorityNormal, KBasicInterval, iBasicAnim );

	}

void CAnimationAppView::DoSpriteAnimOperationL( TAnimOperation aOperation )
	{
	
	if( !iSpriteAnim )
		{	
		iEikonEnv->InfoMsg( _L("Not Initialised") );
		return;
		}
		
		switch( aOperation )
		{
		
		case EAnimPause:
			iSpriteAnim->Pause();
			break;
		
		case EAnimResume:
			iSpriteAnim->Resume();
			break;
	
		case EAnimStop:	
			iSpriteAnim->Stop();
			break;
		
		default:
			break;
			
		}
		
	}
	
	
void CAnimationAppView::DoBasicAnimOperationL( TAnimOperation aOperation )
	{
	
	if( !iBasicAnim )
		{	
		iEikonEnv->InfoMsg( _L("Not Initialised") );
		return;
		}
		
		switch( aOperation )
		{
		case EAnimPause:
			iBasicAnim->Pause();
			break;
		
		case EAnimResume:
			iBasicAnim->Resume();
			break;
		
		case EAnimStop:	
			iBasicAnim->Stop();
			break;
		
		default:
			break;	
		}
		
	}


 // Delete the current anims and movers, if instantiated. 
 // Redraw the window after deleting a CBasicAnimation and call Invalidate()
 	   
void CAnimationAppView::ResetSpriteAnimAndMover()
	{
	
	if( iSpriteAnimMover )
		{
		iSpriteAnimMover->Cancel();	
		delete iSpriteAnimMover;
		iSpriteAnimMover = NULL;	
		}
			
	if( iSpriteAnim )
		{
		iSpriteAnim->Stop();
		delete iSpriteAnim;	
		iSpriteAnim = NULL;
		}
	}
	
void CAnimationAppView::ResetBasicAnimAndMover()
	{
	if( iBasicAnimMover )
		{
		iBasicAnimMover->Cancel();	
		delete iBasicAnimMover;
		iBasicAnimMover = NULL;	
		}
					
	if( iBasicAnim )
		{
		iBasicAnim->Stop();
		delete iBasicAnim;	
		iBasicAnim = NULL;
		
		Window().Invalidate();
		}
		
	}
	
    
// Draw the view
void CAnimationAppView::Draw(const TRect& /*aRect*/) const
	{
	
	// Window graphics context
	CWindowGc& gc = SystemGc();
	
	// Area in which we shall draw
	TRect drawRect = Rect();
	
	// Font used for drawing text
	const CFont*  fontUsed;
	
	// Clear the screen
	gc.Clear();
	
	// Use the title font supplied by the UI
	fontUsed = iEikonEnv->TitleFont();
	gc.UseFont( fontUsed );

	// Draw the text in the rectangle 	
	TInt   baselineOffset=( drawRect.Height() - fontUsed->HeightInPixels() ); 
	gc.DrawText( *iAnimTestText,drawRect,0 + fontUsed->HeightInPixels(),CGraphicsContext::ECenter, 0 );
	
	gc.DrawText(  KAnimExBasicLabel, drawRect, baselineOffset - KBasicOffset, CGraphicsContext::ECenter, 0 );
	

	// CBasicAnimations need to be drawn by us.
	if( iBasicAnim )
		{
		iBasicAnim->Draw( gc );	
		}

	gc.DrawText( KAnimExSpriteLabel, drawRect, baselineOffset - KSpriteOffset, CGraphicsContext::ELeft, 0 );	
	
	// Discard the font.
	gc.DiscardFont();
	}
	



