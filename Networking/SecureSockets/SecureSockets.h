// SecureSockets.h
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
//

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

#include <SecureSockets.rsg>
#include "secengine.h"

#ifndef _SECAPP_H_
#define _SECAPP_H_


/** Application UID */
const TUid KUidSecApp = {0xE8000068};

/**
	Console-type control
*/
class CConsoleControl : public CCoeControl
	{
public:
	static CConsoleControl* NewL(const TRect& aClientRect);
	~CConsoleControl();
	void ConstructL(const TRect& aClientRect);
	CConsoleBase& Console();

	// Override CCoeControl
    void ActivateL();

protected:
	CConsoleControl() {}

private:
	CEikConsoleScreen* iConsole;	// Standard console control
	};

/**
	Dialog that allows user to specify web page to securely connect to
 */
class CSettingsDialog: public CEikDialog
	{
public:
	/**
	Creates and runs the settings dialog.
	@return True if settings confirmed
	@param aConnectSettings The settings, on return the dialog values
	*/
	static TBool RunDlgLD(TConnectSettings& aConnectSettings);

private:
	/**
	Constructor.
	@param aConnectSettings The settings
	*/
	CSettingsDialog(TConnectSettings& aConnectSettings);

	// From CEikDialog
	/** Populate dialog with initial settings */
	void PreLayoutDynInitL();
	/** Store dialog input */
	TBool OkToExitL(TInt aButtonId);

private:
	/** The settings */
	TConnectSettings& iConnectSettings;
	};

/**
	User interface command handling
*/
class CSecAppUi : public CEikAppUi
    {
public:
	CSecAppUi(CSecEngine* aSecEngine);
    void ConstructL();
	~CSecAppUi();

private:
	// Override CEikAppUi
    void HandleCommandL(TInt aCommand);
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

private:
	CConsoleControl* iConsoleControl;
	CSecEngine* iSecEngine;
	TConnectSettings iConnectSettings;

	RFile iOutputFile;
	TFileName iOutputFileName;
	};

/**
	Document class, which owns the engine
*/
class CSecDocument : public CEikDocument
	{
public:
	CSecDocument(CEikApplication& aApp);
	static CSecDocument* NewL(CEikApplication& aApp);
	~CSecDocument();
	void ConstructL();
private:
	// Override CApaDocument
	CEikAppUi* CreateAppUiL();
private:
	CSecEngine* iSecEngine; // Document owns the engine
	};

/**
	Application class
*/
class CSecApplication : public CEikApplication
	{
private: // from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};
#endif



