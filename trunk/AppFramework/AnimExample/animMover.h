// AnimMover.h
//
//  Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//


#ifndef __ANIMMOVER_H__
#define __ANIMMOVER_H__

#include <e32std.h>
#include <SpriteAnimation.h>
#include <BasicAnimation.h>
#include <math.h>

const TInt KXDeflectionIncrement = 5;
const TInt KXDeflectionTerminalValue = 475;
const TInt KYDeflectionOffset = 125;
const TInt KYDeflectionMultiplier = 12;

const TInt KAnimExBasicInitPosX = 300;
const TInt KAnimExBasicInitPosY = 100;
const TInt KAnimExSpriteInitPosX = 60;
const TInt KAnimExSpriteInitPosY = 70;	
	

// Basic Animation Mover
class CSpriteAnimMover : public CActive
	{
public:
	CSpriteAnimMover( TInt aPriority, TInt aMicroSeconds, CSpriteAnimation* aSpriteAnim );
	~CSpriteAnimMover();
	
	
	
protected:		
	void DoCancel();	
	void RunL();
		
private:
	RTimer iTimer;
	TInt iMicroSeconds;
	CSpriteAnimation* iSpriteAnim; // Caller owns.
	TInt iX;
	TInt iY;	
	
	};


// Sprite Animation Mover
class CBasicAnimMover : public CActive
	{
public:
	CBasicAnimMover( TInt aPriority, TInt aMicroSeconds, CBasicAnimation* aBasicAnim );
	~CBasicAnimMover();
		
protected:		
	void DoCancel();	
	void RunL();
		
private:
	RTimer iTimer;
	TInt iMicroSeconds;
	CBasicAnimation* iBasicAnim; // Caller owns.
	TInt iX;
	TInt iY;
	};
	
#endif // __ANIMMOVER_H__
