// AnimExample.h
//
//  Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//


#ifndef __ANIMEXAMPLE_H
#define __ANIMEXAMPLE_H

#include <coeccntx.h>

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>
#include <eikstart.h> 
#include <eikon.hrh>
#include <gdi.h>

#include <SpriteAnimation.h>
#include <ICLAnimationDataProvider.h>
#include <BasicAnimation.h>
#include "AnimMover.h"
#include <AnimExample.rsg>
#include "AnimExample.hrh"

_LIT(KAnimExGuitarPlayer,"Z:\\resource\\apps\\AnimExample\\GuitarPlayer.gif");

_LIT( KAnimExSpriteLabel, "Sprite Animation" );
_LIT( KAnimExBasicLabel, "Basic Animation" );

const TInt KBasicOffset = 135;
const TInt KSpriteOffset = 25;

const TInt KspriteInterval = 10000;  //Microseconds
const TInt KBasicInterval = 99999; //Microseconds

const TInt KAnimExBasicPositionX = 300;
const TInt KAnimExBasicPositionY = 100;
const TInt KAnimExSpritePositionX = 60;
const TInt KAnimExSpritePositionY = 70;	


enum TAnimOperation
	{
	EAnimPause,
	EAnimResume,	
	EAnimHold,
	EAnimUnhold,
	EAnimFreeze,
	EAnimUnfreeze,
	EAnimStart,
	EAnimStop		
	};


class CAnimationApplication : public CEikApplication
	{
private: 
	// Inherited from class CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};

class CAnimationAppView : public CCoeControl
    {
public:
	static CAnimationAppView* NewL( const TRect& aRect );
	CAnimationAppView();
	~CAnimationAppView();
    void ConstructL( const TRect& aRect );
    
    void ProduceSpriteAnimL();
    void ProduceBasicAnimL();
    void MoveAnimsL();
    void DoSpriteAnimOperationL( TAnimOperation aOperation );
    void DoBasicAnimOperationL( TAnimOperation aOperation );
    void ResetSpriteAnimAndMover();
    void ResetBasicAnimAndMover();

    CSpriteAnimation* iSpriteAnim;
	CBasicAnimation* iBasicAnim;
	
private:
	// From CCoeControl
	void Draw(const TRect& /*aRect*/) const;

private:
	HBufC*  iAnimTestText;
	CSpriteAnimMover* iSpriteAnimMover;
	CBasicAnimMover* iBasicAnimMover;
	
    };

class CAnimationAppUi : public CEikAppUi
    {
public:
    void ConstructL();
	~CAnimationAppUi();

private:
    // Inherited from class CEikAppUi
	void HandleCommandL(TInt aCommand);

private:
	CAnimationAppView* iAppView;
	};


class CAnimationDocument : public CEikDocument
	{
public:
	static CAnimationDocument* NewL(CEikApplication& aApp);
	CAnimationDocument(CEikApplication& aApp);
	void ConstructL();

private: 
	// Inherited from CEikDocument
	CEikAppUi* CreateAppUiL();
	};


#endif


