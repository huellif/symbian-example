// CUSTOM.H
//
// 
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.
//
// 
// UI Control Framework example application
//

#if !defined(__COSMILEY_H__)
#define __COSMILEY_H__

//////////////////////////////////////////////////////////////////////////////
//
// Include files containing:
//
// 1. Symbol definitions used by C++ code and resource scripts (the *.hrh)
// 2. Resource ids generated by resource compilation of 
//    resource scripts (the *.rsg)
// 3. Class definitions required by this app (the *.h)
//    
//////////////////////////////////////////////////////////////////////////////

// 1.
#include <eikon.hrh>
#include "custom.hrh"
// 2.
#include <eikon.rsg>
#include <custom.rsg>
// 3.
#include <eikenv.h>
#include <eikappui.h>
#include <eikproc.h>
#include <eikdialg.h>
#include <eikmenub.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <coeutils.h>
#include <barsread.h>
#include <eikfctry.h>

  // The unique identifier for this application.
  // NOTE that the number has been arbitrarily chosen for the purpose of
  // of the example.
const TUid KUidExampleApp={0xE8000004};


//////////////////////////////////////////////////////////////////////////////
//
// -----> CSmileyDialog(definition)
//
//////////////////////////////////////////////////////////////////////////////
class CSmileyDialog : public CEikDialog
	{
public:
	// Construct and run
	static TBool RunDlgLD();
private:
	SEikControlInfo CreateCustomControlL(TInt aControlType);
	};	

//////////////////////////////////////////////////////////////////////////////
//
// -----> CSmiley (definition)
//
//////////////////////////////////////////////////////////////////////////////
class CSmiley : public CCoeControl
	{
public:
	CSmiley(TBool aSmiling);
	~CSmiley();
public:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	TBool IsSmiling() const;
	void SetSmiling(TBool aIsSmiling);
private:
	virtual void Draw(const TRect& aRect) const;
	virtual void SizeChanged();
	
	void DrawFace(CWindowGc &aGc) const;
	void DrawEyes(CWindowGc &aGc) const;
	void DrawEyebrow(CWindowGc& aGc, TBool bSmiling) const;
	void DrawMouth(CWindowGc& aGc, TBool bSmiling) const;
	
protected:
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void FocusChanged(TDrawNow aDrawNow);
private:
	TBool	iSmiling;
	TRect	iSmileyRect;
	TInt	iSmileyWidth;
	TInt	iSmileyHeight;
	TRect	iSmileRect;
	TRect	iFrownRect;
	};

//////////////////////////////////////////////////////////////////////////////
//
// -----> CSmileyContainer(definition)
//
//////////////////////////////////////////////////////////////////////////////
class CSmileyContainer : public CCoeControl, 
						 public MCoeControlObserver
	{
public:
	  // Construction
	CSmileyContainer();
	void ConstructL(const TRect& aRect);
	  // Destruction
	~CSmileyContainer();
	void ConstructFromResourceL(TResourceReader& aReader);
	void PrepareForFocusLossL();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
protected:
	void FocusChanged(TDrawNow aDrawNow);
private:
	  // Virtual, defined by CCoeControl; replaces the default implementation
	  // provided by CCoeControl.
	void         Draw(const TRect& aRect) const;
	
	  // Virtual, defined by CCoeControl; replaces the default implementation
	  // provided by CCoeControl. 
    TInt         CountComponentControls() const;

	  // Virtual, defined by CCoeControl; replaces the default implementation
	  // provided by CCoeControl.
	CCoeControl* ComponentControl(TInt aIndex) const;

	  // Virtual, defined by CCoeControl; empty implementation provided by
	  // CCoeControl; full implementation provided by this class
	void         SizeChanged();
	
	  // Defined as pure virtual by the mixin class MCoeControlObserver 
	  // inherited by CCoeControl. An empty implementation provided by 
	  // this class (its containees do not report events).
	void         HandleControlEventL(CCoeControl* aControl,
		                             TCoeEvent aEventType);
protected:
	virtual void HandlePointerEventL(const TPointerEvent &aPointerEvent);
private:
	  // Member functions defined and used by this class
	void		SwapFocus(CCoeControl* aControl);
private:
      // Data members defined and used by this class.
	CSmiley*	iSmiley1;
	CSmiley*	iSmiley2;
	};



//////////////////////////////////////////////////////////////////////////////
//
// -----> CMainWinControl (definition)
//
//////////////////////////////////////////////////////////////////////////////
class CMainWinControl : public CCoeControl
{
public:
	CMainWinControl();
	~CMainWinControl();
	void ConstructL(const TRect& rect);
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
private:
	virtual void HandlePointerEventL(const TPointerEvent &aPointerEvent);
	void Draw(const TRect& aRect) const;
private:
	CSmileyContainer*	iContainer;
};

//////////////////////////////////////////////////////////////////////////////
//
// -----> CExampleAppUi (definition)
//
//////////////////////////////////////////////////////////////////////////////
class CExampleAppUi : public CEikAppUi
	{
public:
	  // Destruction.
	~CExampleAppUi();

public:
	  // Virtual, defined by CEikAppUi; replaces the implementation 
	  // provided by CEikAppUi.
	void ConstructL();
	
private:
	  // Virtual, defined by CEikAppUi; empty implementation
	  // provided by CEikAppUi; full implementation provided
	  // by this class. 
	void  HandleCommandL(TInt aCommand);
	
	  // Virtual, defined by CEikAppUi; empty implementation
	  // provided by CEikAppUi; full implementation provided
	  // by this class. 
	void  HandleModelChangeL();
	
private:
	void         OnCmdExit();
private:
	// Data members defined by this class.
	CMainWinControl* iMainWinControl;
	};

//////////////////////////////////////////////////////////////////////////////
//
// -----> CExampleDocument (definition)
//
//////////////////////////////////////////////////////////////////////////////
class CExampleDocument : public CEikDocument
	{
public:
	  // Construction.
	static CExampleDocument* NewL(CEikApplication& aApp);
    CExampleDocument(CEikApplication& aApp);
	  // Destruction.
	~CExampleDocument();

private:
	  // Defined as pure virtual by CEikDocument; full implementation
	  // provided by this class
	CEikAppUi* CreateAppUiL();		// Construct an app.user interface

	  // Defined as pure virtual by CApaDocument; empty implementation 
	  // provided by CEikDocument; full implementation provided
	  // by this class.
	void       NewDocumentL();   	// Build a new document

	  // Defined as pure virtual by CApaDocument; empty implementation
	  // provided by CEikDocument; full implementation provided
	  // by this class.
	void       StoreL(CStreamStore& aStore,
		              CStreamDictionary& aStreamDic
					 ) const;
	void       RestoreL(const CStreamStore& aStore,
		                const CStreamDictionary& aStreamDic
		               );

public:
	  // Member functions defined by this class
	void       DoNewFileL(const TFileName& aFileName);
	void       DoOpenFileL(const TFileName& aFileName);
	void       DoSaveToNewFileL(const TFileName& aNewFileName);
	TBool      FileNameExists(const TFileName& aFileName) const;

private:
	  // Member functions defined by this class
	void       CreateModelL();
	void       ResetModelL();
	};

//////////////////////////////////////////////////////////////////////////////
//
// -----> CExampleApplication (definition)
//
//////////////////////////////////////////////////////////////////////////////
class CExampleApplication : public CEikApplication
	{
private:		
	  // Defined as pure virtual by CApaApplication; implementation
	  // provided by this class 
	TUid          AppDllUid() const; // Returns Uid associated with app

	  // Defined as pure virtual by CEikApplication; implementation
	  // provided by this class.
	CApaDocument* CreateDocumentL(); // Construct new document
	};



#endif
