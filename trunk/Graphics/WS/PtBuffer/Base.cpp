// Base.CPP
//
// Copyright (c) 2005 Symbian Softwares Ltd.  All rights reserved.
//

#include <w32std.h>
#include "Base.h"

///////////////////////////////////////////////////////////////////////////////
//							CWindow implementation
///////////////////////////////////////////////////////////////////////////////

CWindow::CWindow(CWsClient* aClient)
	: iClient(aClient)
	{
	}

void CWindow::ConstructL (const TRect& aRect, const TRgb& aColor, CWindow* aParent)
	{
	// If a parent window was specified, use it; if not, use the window group
	// (aParent defaults to 0).
	RWindowTreeNode* parent= aParent ? (RWindowTreeNode*) &(aParent->Window()) : &(iClient->iGroup);
	// Allocate and construct the window
	iWindow=RWindow(iClient->iWs);
	User::LeaveIfError(iWindow.Construct(*parent,(TUint32)this));
	// Store the window's extent
	iRect = aRect;
	// Set up the new window's extent
	iWindow.SetExtent(iRect.iTl, iRect.Size());
	// Set its background color
	iWindow.SetBackgroundColor (aColor);
	// Set pointer grab to enable drag and drop
	iWindow.SetPointerGrab (ETrue);
	// Allocate pointer move buffer for freehand drawing
	iWindow.AllocPointerMoveBuffer(KPointerMoveBufferSize, 0);
	iWindow.DisablePointerMoveBuffer();
	// Tell the window server to deliver drag events
	iWindow.PointerFilter(EPointerFilterDrag, 0);
	// Activate the window
	iWindow.Activate();
	}

CWindow::~CWindow()
	{
	iWindow.FreePointerMoveBuffer();
	iWindow.Close();
	}

RWindow& CWindow::Window()
	{
	return iWindow;
	}

CWindowGc* CWindow::SystemGc()
	{
	return iClient->iGc;
	}

CWsScreenDevice* CWindow::Screen()
	{
	return iClient->iScreen;
	}

///////////////////////////////////////////////////////////////////////////////
//							CWsRedrawer implementation
///////////////////////////////////////////////////////////////////////////////

CWsRedrawer::CWsRedrawer()
	: CActive(CActive::EPriorityLow)
	{
	}

void CWsRedrawer::ConstructL(CWsClient* aClient)
	{
	iClient=aClient; // remember WsClient that owns us
	CActiveScheduler::Add(this); // add ourselves to the scheduler
	IssueRequest(); // issue request to draw
	}

CWsRedrawer::~CWsRedrawer()
	{
	Cancel();
	}

void CWsRedrawer::IssueRequest()
	{
	iClient->iWs.RedrawReady(&iStatus);
	SetActive();
	}

void CWsRedrawer::DoCancel()
	{
	iClient->iWs.RedrawReadyCancel();
	}

void CWsRedrawer::RunL()
	{	
	// find out what needs to be done in response to the event
	TWsRedrawEvent redrawEvent;
    iClient->iWs.GetRedraw(redrawEvent); // get event
	CWindow* window=(CWindow*)(redrawEvent.Handle()); // get window
	if (window)
		{
		TRect rect=redrawEvent.Rect(); // and rectangle that needs redrawing
		// now do drawing
		iClient->iGc->Activate(window->Window());
		window->Window().BeginRedraw();
		window->Draw(rect);
		window->Window().EndRedraw();
		iClient->iGc->Deactivate();
		}
	// maintain outstanding request
	IssueRequest();
	}


/////////////////////////////////////////////////////////////////////////////////////
//								CWsClient implementation
/////////////////////////////////////////////////////////////////////////////////////
CWsClient::CWsClient()
	: CActive(CActive::EPriorityStandard)
	{
	}

void CWsClient::ConstructL()
	{
	// add ourselves to scheduler
	CActiveScheduler::Add(this);
	// get a session going
	User::LeaveIfError(iWs.Connect());
	// construct our one and only window group
	iGroup=RWindowGroup(iWs);
	User::LeaveIfError(iGroup.Construct(2,ETrue)); // '2' is a meaningless handle
	// construct screen device and graphics context
	iScreen=new (ELeave) CWsScreenDevice(iWs); // make device for this session
	User::LeaveIfError(iScreen->Construct()); // and complete its construction
	User::LeaveIfError(iScreen->CreateContext(iGc)); // create graphics context
	// construct redrawer
	iRedrawer=new (ELeave) CWsRedrawer;
	iRedrawer->ConstructL(this);
	// construct main window
	ConstructMainWindowL();
	// request first event and start scheduler
	IssueRequest();
	}

CWsClient::~CWsClient()
	{
	// neutralize us as an active object
	Deque(); // cancels and removes from scheduler
	// get rid of everything we allocated
	delete iGc;
	delete iScreen;
	delete iRedrawer;
	// destroy window group
	iGroup.Close();
	// finish with window server
	iWs.Close();
	}

void CWsClient::IssueRequest()
	{
	iWs.EventReady(&iStatus); // request an event
	SetActive(); // so we're now active
	}

void CWsClient::DoCancel()
	{
	iWs.EventReadyCancel(); // cancel event request
	}

void CWsClient::ConstructMainWindowL()
	{
	}

