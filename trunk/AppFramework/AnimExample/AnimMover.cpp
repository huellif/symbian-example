// AnimMover.cpp
//
//  Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//


#include "AnimMover.h"

// Sprite Animation Mover

CSpriteAnimMover::CSpriteAnimMover( TInt aPriority, TInt aMicroSeconds, CSpriteAnimation* aSpriteAnim )
: CActive( aPriority ), iMicroSeconds( aMicroSeconds ), iSpriteAnim( aSpriteAnim )
	{
		
	CActiveScheduler::Add( this );
	
	iTimer.CreateLocal();
	iTimer.After( iStatus, aMicroSeconds );
	SetActive();
	}
	
		
CSpriteAnimMover::~CSpriteAnimMover()
	{
	iTimer.Close();	
	}

	
void CSpriteAnimMover::DoCancel()
	{
	iTimer.Cancel();		
	}

	
void CSpriteAnimMover::RunL()
	{
	
	if( !iSpriteAnim )
		{
		User::Leave( KErrNotReady );	
		}

	if( iX++ <= KXDeflectionTerminalValue )
		{
		iSpriteAnim->SetPosition( TPoint(iX, iY) );	
		}
	else
		{
		
		Cancel();
		iSpriteAnim->SetPosition( TPoint(KAnimExSpriteInitPosX, KAnimExSpriteInitPosY ) );	
		}
		
	iTimer.After( iStatus, iMicroSeconds );	
	SetActive();
	}
	
	
	
// Basic Animation Mover

CBasicAnimMover::CBasicAnimMover( TInt aPriority, TInt aMicroSeconds, CBasicAnimation* aBasicAnim )
: CActive( aPriority ), iMicroSeconds( aMicroSeconds ), iBasicAnim( aBasicAnim ), iX( 0 ), iY( 0 )
	{
		
	CActiveScheduler::Add( this );
	
	iTimer.CreateLocal();
	iTimer.After( iStatus, aMicroSeconds );
	SetActive();
	}
	
	CBasicAnimMover::~CBasicAnimMover()
	{
	
	iTimer.Close();	
	}

void CBasicAnimMover::DoCancel()
	{
	
	iTimer.Cancel();		
	}

void CBasicAnimMover::RunL()
	{
	
	if( !iBasicAnim )
		{
		User::Leave( KErrNotReady );	
		}
	
	
	if( iX < KXDeflectionTerminalValue )
		{
		iY =TInt(KYDeflectionOffset + KYDeflectionMultiplier*sin( iX++ ));	
		iBasicAnim->SetPosition( TPoint(iX, iY) );
		
		iX += KXDeflectionIncrement;	
		}
	else
		{
		
		Cancel();
		iBasicAnim->SetPosition( TPoint(KAnimExBasicInitPosX, KAnimExBasicInitPosY) );			
		}
		
		
	iTimer.After( iStatus, iMicroSeconds );	
	SetActive();
	}
