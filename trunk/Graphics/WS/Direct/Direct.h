// Direct.h
//
// Copyright (c) 2005 Symbian Softwares Ltd.  All rights reserved.
//

// Application framework classes for Direct Screen Area (DSA) API example

#ifndef __DIRECT_H
#define __DIRECT_H

#include <coecntrl.h>
#include <coeccntx.h>
#include <coemain.h>

#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <uikon.hrh>

#include <Direct.rsg>
#include "Direct.hrh"

const TUid KUidExample =
	{
	0xE8000054
	};

class CDirectDisplayLife;
class CLifeEngine;

// View, a window containing a DSA
class CExampleAppView : public CCoeControl
	{
public:
	// View state
	enum
		{
		EDirectNotStarted = 0, // DSA not used
		EDirectStarted, // DSA in use
		EDirectPaused
		// Use of DSA temporarily halted
		};

public:
	// Construction & destruction
	CExampleAppView(CLifeEngine& aLifeEngine);
	~CExampleAppView();
	void ConstructL(const TRect& aRect);

	// Start using the DSA
	void StartDirectL();
	// Pause use of the DSA
	void PauseDirect();
	// Restart use of the DSA after pausing
	void RestartDirect();

	// Gets the view state
	TInt State() const;

private:
	// from CCoeControl
	void Draw(const TRect&) const;

private:
	// The object that handles the DSA
	CDirectDisplayLife* iDirectDisplayLife;
	// The data the view displays
	CLifeEngine& iLifeEngine;
	// View state
	TInt iState;
	};

// App UI, handles user commands
class CExampleAppUi : public CEikAppUi
	{
public:
	// Construction & destruction
	void ConstructL();
	~CExampleAppUi();

private:
	// from CEikAppUi
	void HandleCommandL(TInt aCommand);

private:
	class COverlayDialog : public CActive
		{
	public:
		COverlayDialog();
		~COverlayDialog();
		void ShowDialog();

	private:
		void RunL();
		void DoCancel();

	private:
		RNotifier iNotifier;
		TInt iR;
		};

private:
	// The app view
	CExampleAppView* iAppView;
	COverlayDialog* iOverlayDialog;
	};

// App document. Owns the engine.
class CExampleDocument : public CEikDocument
	{
public:
	// Construction & destruction
	CExampleDocument(CEikApplication& aApp);
	~CExampleDocument();

	// Gets the engine
	CLifeEngine& LifeEngine() const;

private:
	// from CEikDocument
	CEikAppUi* CreateAppUiL();

private:
	// The engine
	CLifeEngine* iLifeEngine;
	};

// Application class
class CExampleApplication : public CEikApplication
	{
private:
	// from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};

#endif
