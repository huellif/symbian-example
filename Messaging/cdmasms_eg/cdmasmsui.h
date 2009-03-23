//
//	cdmasmsui.h
//
//	Copyright (c) 2004 Symbian Software Ltd.  All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#include <basched.h>
#include <eikenv.h>
#include <coecntrl.h>
#include <eikappui.h>
#include <e32keys.h>
#include <eikconso.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikon.rsg>
#include <eikinfo.h>
#include <eikon.hrh>
#include <cdmasms_eg.rsg>

#ifndef _SMSEGAPP_H_
#define _SMSEGAPP_H__

// App UID
const TUid KUidSmsEgApp = {0xE800005B};

class CMenuHandler;

/**
 Console-type control for input and output.
*/
class CConsoleControl : public CCoeControl
	{
public:
	static CConsoleControl* NewL();
	~CConsoleControl();

	// Begin: initialise engine, and put up first menu
	void StartL();

	// Following override CCoeControl

	// The key event handler: accepts menu choices
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	// Mark control ready to draw
    void ActivateL();

protected:
	CConsoleControl(){};
	void ConstructL();

private:
	/** Standard console screen */
	CEikConsoleScreen* iConsole;
	/** Input handler */
	CMenuHandler* iMenuHandler;
	/** Input buffer */
	TBuf<80> iBuffer;
	};

/**
 Standard minimal app UI class.
*/
class CSmsEgAppUi : public CEikAppUi
    {
public:
    void ConstructL();
	void CreateConsoleL();
	~CSmsEgAppUi();

private:
	// Override CEikAppUi
    void HandleCommandL(TInt aCommand);
private:
	CConsoleControl* iConsoleControl;	// Console control wrapper. Owned by object
	};

/**
 Standard minimal document class.
*/
class CSmsEgDocument : public CEikDocument
	{
public:
	CSmsEgDocument(CEikApplication& aApp);
	static CSmsEgDocument* NewL(CEikApplication& aApp);
	~CSmsEgDocument();
	void ConstructL();
private:
	// Override CApaDocument
	CEikAppUi* CreateAppUiL();
	};

/**
 Standard minimal application class.
*/
class CSmsEgApplication : public CEikApplication
	{
private: // from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};

#endif




