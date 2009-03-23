// MenuApp.h
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#ifndef __MENUAPP_H__
#define __MENUAPP_H__

#include <coeccntx.h>

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>


#include <eikon.hrh>
#include <eikon.rsg>
#include <menuapp.rsg>

#include <apgcli.h>


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
	static CExampleAppView* NewL(const TRect& aRect);
	CExampleAppView();
	~CExampleAppView();
	void ConstructL(const TRect& aRect);
private:
	void Draw(const TRect& /*aRect*/) const;	
	};


// Application UI class
class CExampleAppUi : public CEikAppUi
	{
public:
	void ConstructL();
	~CExampleAppUi();
private:
	void HandleCommandL(TInt aCommand);	
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
private:
	CCoeControl* iAppView;
public:	
	RApaLsSession iApaLsSession;	
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
	
#endif //__MENUAPP_H__

