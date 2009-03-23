// PointerMoveBuffer.H
//
// Copyright (c) 2005 Symbian Softwares Ltd.  All rights reserved.
//


#if !defined(__WINORD1_H__)
#define __WINORD1_H__

#include "Base.h"

//////////////////////////////////////////////////////////////////////////
//						Derived window classes
//////////////////////////////////////////////////////////////////////////

class CMainWindow : public CWindow
	{
public:
	CMainWindow (CWsClient* aClient);
	~CMainWindow ();
	void Draw (const TRect& aRect);
	void HandlePointerEvent (TPointerEvent& aPointerEvent);
	void HandlePointerMoveBufferReady ();
	};

//////////////////////////////////////////////////////////////////////////
//						Derived client class
//////////////////////////////////////////////////////////////////////////

class CExampleWsClient : public CWsClient
	{
public:
	static CExampleWsClient* NewL(const TRect& aRect);
	~CExampleWsClient ();
private:
	CExampleWsClient (const TRect& aRect);
	void ConstructMainWindowL();
	void RunL ();
	void HandleKeyEventL (TKeyEvent& aKeyEvent);
private:
	CMainWindow*	iMainWindow;
	const TRect& iRect;
	};

#endif
