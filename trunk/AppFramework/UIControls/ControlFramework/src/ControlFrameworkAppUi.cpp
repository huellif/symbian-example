// ControlFrameworkAppUi.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//

#include "ControlFrameworkAppUi.h"
#include "ControlFrameworkView.h"
#include "ControlFramework.hrh"
#include "uikon.hrh"


CControlFrameworkAppUi::CControlFrameworkAppUi()
	{
	}

CControlFrameworkAppUi::~CControlFrameworkAppUi()
	{
	// Deregister the view(s) from the view server, remove the view(s) from the control stack
	// and delete them
	TInt view = iAppViews.Count();
	while (view--)
		{
		CControlFrameworkView* v = iAppViews[view];
		RemoveFromStack(v);
		DeregisterView(*v);
		}
	iAppViews.ResetAndDestroy();
	}

// Second phase constructor of the application UI.
// It creates and owns a single view. The view is not fully constructed 
// (via ViewConstructL()) until the first time the view is activated,
// to improve application startup time.
void CControlFrameworkAppUi::ConstructL()
	{
	BaseConstructL(CEikAppUi::EStandardApp);
	
   	// Create the view
	CControlFrameworkView* view = CControlFrameworkView::NewLC(*this);
	AddViewL(view);
	CleanupStack::Pop(view);
	}


// Called by the UI framework when a command has been issued 
// by one of the toolbar buttons.
void CControlFrameworkAppUi::HandleCommandL(TInt aCommandId)
	{
	CControlFrameworkView& view = *iAppViews[EExampleView];
	switch (aCommandId)
		{
		// command IDs are defined in the hrh file.
		case ECmdToggleBoldFont:
			view.ToggleBold();
			break;
		
		case ECmdToggleItalicFont:	
			view.ToggleItalic();
			break;
		
		case ECmdToggleUnderline: 
			view.ToggleUnderline();
			break;
			
		case ECmdToggleStrikethrough: 
			view.ToggleStrikethrough();
			break;
				
		default:				
		case EEikCmdExit:
			Exit();
		}
	}


static void CleanupRemoveFromStack(TAny* aView)
	{ 
	static_cast<CEikAppUi*>(CCoeEnv::Static()->AppUi())->RemoveFromStack(static_cast<CControlFrameworkView*>(aView)); 
	}

static void CleanupDeregisterView(TAny* aView) 
	{ 
	static_cast<CEikAppUi*>(CCoeEnv::Static()->AppUi())->DeregisterView(*static_cast<CControlFrameworkView*>(aView)); 
	}

// Adds the view to the app UI's control stack and registers it with the view server. 
// Takes ownership of the view.
void CControlFrameworkAppUi::AddViewL(CControlFrameworkView* aView)
	{
	ASSERT(aView);
	ASSERT(KErrNotFound == iAppViews.Find(aView));
		
	RegisterViewL(*aView); // Call RegisterViewL before AddToStackL to avoid panic in destructor if AddToStackL leaves 
	CleanupStack::PushL(TCleanupItem(CleanupDeregisterView,aView)); 
	
	AddToStackL(*aView, aView); 
	CleanupStack::PushL(TCleanupItem(CleanupRemoveFromStack, aView)); 
	
	iAppViews.AppendL(aView); 
	CleanupStack::Pop(2); 
	}
