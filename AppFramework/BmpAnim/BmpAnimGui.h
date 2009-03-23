// BmpAnimGui.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#ifndef __BMPANIMGUI_H
#define __BMPANIMGUI_H

#include <coeccntx.h>

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>
#include <eikstart.h> 
#include <math.h>

#include <bmpancli.h>
#include <eikon.hrh>

#include <BmpAnimGui.rsg>
#include "BmpAnimGui.hrh"

const TInt KColorBallsMbmMaskIndex = 4;
const TInt KColorBallsMbmLastBallIndex = 3;
const TInt KColorBallsMbmFirstBallIndex = 0;
const TInt KColorBallsAnimFrameDelay = 150; // milliseconds
const TInt KColorBallsAnimXDeflectionIncrement = 20;
const TInt KColorBallsAnimXDeflectionStart = 0;
const TInt KColorBallsAnimXDeflectionTerminalValue = 600;
const TInt KColorBallsAnimYDeflectionOffset = 100;
const TInt KColorBallsAnimYDeflectionMultiplier = 50;


_LIT( KColorBallsMbmPath, "Z:\\resource\\apps\\BmpAnimGui\\ColorBalls.mbm"  );

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

public:
	RBitmapAnim iBitmapAnimPlayer;

private:
	void Draw( const TRect& /*aRect*/ ) const;
	void LoadMbmL( const TDesC& aMbmFile );

private:
	HBufC*  iExampleText;
	CBitmapAnimClientData* iAnimFrames;
	RAnimDll iAnimDll;
	TInt iX;
	TInt iY;
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


#endif // __BMPANIMGUI_H

