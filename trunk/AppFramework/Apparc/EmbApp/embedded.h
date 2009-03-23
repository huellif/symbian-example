// embedded.h
//
// Copyright (c) Symbian Software Ltd 2005. All rights reserved.
//

#ifndef __EMBEDDED_H__
#define __EMBEDDED_H__

#include <coeccntx.h>

#include <apgtask.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>

#include <ecom.h>
#include <implementationproxy.h>

_LIT(KExampleText, "Welcome to the embedded application example");

// Embedded application class
class CExampleApplication : public CEikApplication
	{
private:
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
private:
	TFileName ResourceFileName() const;
private:
	CApaDocument* CreateDocumentL(CApaProcess* a) { return CEikApplication::CreateDocumentL(a); }
	};

// Embedded view class
class CExampleAppView : public CCoeControl
    {
public:
	static CExampleAppView* NewL(const TRect& aRect);
	CExampleAppView();
	~CExampleAppView();
    void ConstructL(const TRect& aRect);

private:
	void Draw(const TRect& /*aRect*/) const;
private:
	HBufC*  iExampleText;
    };

// Embedded application UI class
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

// Embedded document class
class CExampleDocument : public CEikDocument
	{
public:
	static CExampleDocument* NewL(CEikApplication& aApp);
	CExampleDocument(CEikApplication& aApp);
	void ConstructL();
private:
	CEikAppUi* CreateAppUiL();
	};

#endif //__EMBEDDED_H__

