// serv_app_minimal.h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef __SERV_APP_MINIMAL_H__
#define __SERV_APP_MINIMAL_H__

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>

#include "server.h"
#include "session.h"

//Minimal session class
class CMinimalSession : public CServAppSession
	{
public:
	~CMinimalSession();
	CMinimalSession(); 
private:
	virtual void Display(const TDesC & aMessage);
	};

// Minimal application class
class CExampleApplication : public CEikApplication
	{
private:
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	TFileName ResourceFileName() const;
	void NewAppServerL(CApaAppServer*& aAppServer);
	};

// Minimal view class
class CExampleAppView : public CCoeControl
    {
public:
	static CExampleAppView* NewL(const TRect& aRect);
	CExampleAppView();
	~CExampleAppView();
    void ConstructL(const TRect& aRect);

private:
	void Draw(const TRect& /*aRect*/) const;
    };

// Minimal application UI class
class CExampleAppUi : public CEikAppUi
    {
public:
    void ConstructL();
	~CExampleAppUi();

private:
	void HandleCommandL(TInt aCommand);

private:
	CCoeControl* iAppView;
	};

// Minimal document class
class CExampleDocument : public CEikDocument
	{
public:
	static CExampleDocument* NewL(CEikApplication& aApp);
	CExampleDocument(CEikApplication& aApp);
	void ConstructL();
private:
	CEikAppUi* CreateAppUiL();
	};

#endif // __SERV_APP_MINIMAL_H__

