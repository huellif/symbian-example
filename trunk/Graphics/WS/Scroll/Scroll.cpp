// WSSCROL1.CPP
//
// Copyright (c) 2005 Symbian Softwares Ltd.  All rights reserved.
//
#include <w32std.h>
#include "Base.h"
#include "Scroll.h"
_LIT(KString1,"1");
_LIT(KString2,"2");
_LIT(KString3,"3");
_LIT(KString4,"4");
_LIT(KString5,"5");

//////////////////////////////////////////////////////////////////////////////
//					 CNumberedWindow implementation
//////////////////////////////////////////////////////////////////////////////

/****************************************************************************\
|	Function:	Constructor/Destructor for CNumberedWindow
 |	Input:		aClient		Client application that owns the window
 \****************************************************************************/
CNumberedWindow::CNumberedWindow(CWsClient* aClient, TInt aNum) :
	CWindow(aClient),
	iNumber(aNum),
	iOldPos(0, 0),
	iOffsetPoint(0, 0),
	iRepeatRect(0, 0, 0, 0)
	{
	}

CNumberedWindow::~CNumberedWindow()
	{
	}

/****************************************************************************\
|	Function:	CNumberedWindow::Draw
 |	Purpose:	Redraws the contents of CNumberedWindow within a given
 |				rectangle.  CNumberedWindow displays a number in the window.
 |	Input:		aRect	Rectangle that needs redrawing
 |	Output:		None
 \****************************************************************************/
void CNumberedWindow::Draw(const TRect& aRect)
	{
	const TBufC<1> strings[5] =
		{
		*&KString1, *&KString2, *&KString3, *&KString4, *&KString5
		};

	CWindowGc* gc = SystemGc(); // get a graphics context
	gc->SetClippingRect(aRect); // clip outside the redraw area
	gc->Clear(aRect); // clear the redraw area

	gc->SetPenColor(TRgb(0, 0, 0)); // Set pen to black
	gc->UseFont(Font());
	gc->DrawText(strings[iNumber], TextBox(), BaselineOffset(),
			CGraphicsContext::ECenter);
	gc->DrawLine(TPoint(0, 0) + iOffsetPoint, TPoint(WinWidth(), WinHeight()) + iOffsetPoint);
	gc->DiscardFont();
	}

TInt CNumberedWindow::BaselineOffset()
	{
	// Calculate vertical text offset
	return (WinHeight() + (Font()->AscentInPixels() + Font()->DescentInPixels())) / 2;
	}

TRect CNumberedWindow::TextBox()
	{
	return TRect(TPoint(0, 0) + iOffsetPoint, iWindow.Size());
	}

/****************************************************************************\
|	Function:	CNumberedWindow::HandlePointerEvent
 |	Purpose:	Handles pointer events for CNumberedWindow.
 |	Input:		aPointerEvent	The pointer event
 |	Output:		None
 \****************************************************************************/
void CNumberedWindow::HandlePointerEvent(TPointerEvent& aPointerEvent)
	{
	switch (aPointerEvent.iType)
		{
		case TPointerEvent::EButton1Down:
			{
			Window().Scroll(TPoint(0, -2));
			iOffsetPoint += TPoint(0, -2);
			iRepeatRect.iTl = aPointerEvent.iPosition - TPoint(10, 10);
			iRepeatRect.iBr = aPointerEvent.iPosition + TPoint(10, 10);
			iScrollDir = Up;
			Window().RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(20000), iRepeatRect);
			break;
			}
		case TPointerEvent::EButtonRepeat:
			{
			if (iScrollDir == Up)
				{
				Window().Scroll(TPoint(0, -2));
				iOffsetPoint += TPoint(0, -2);
				}
			else
				{
				Window().Scroll(TPoint(0, 2));
				iOffsetPoint += TPoint(0, 2);
				}
			Window().RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(20000), iRepeatRect);
			break;
			}
		case TPointerEvent::EButton3Down:
			{
			Window().Scroll(TPoint(0, 2));
			iOffsetPoint += TPoint(0, 2);
			iRepeatRect.iTl = aPointerEvent.iPosition - TPoint(10, 10);
			iRepeatRect.iBr = aPointerEvent.iPosition + TPoint(10, 10);
			iScrollDir = Down;
			Window().RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(100000), iRepeatRect);
			break;
			}
		default:
			break;
		}
	}

TInt CNumberedWindow::WinHeight()
	{
	return iWindow.Size().iHeight;
	}

TInt CNumberedWindow::WinWidth()
	{
	return iWindow.Size().iWidth;
	}


//////////////////////////////////////////////////////////////////////////////
//					 CMainWindow implementation
//////////////////////////////////////////////////////////////////////////////


/****************************************************************************\
|	Function:	Constructor/Destructor for CMainWindow
 |	Input:		aClient		Client application that owns the window
 \****************************************************************************/
CMainWindow::CMainWindow(CWsClient* aClient) :
	CWindow(aClient)
	{
	}

CMainWindow::~CMainWindow()
	{
	iWindow.Close();
	}

/****************************************************************************\
|	Function:	CMainWindow::Draw
 |	Purpose:	Redraws the contents of CMainWindow within a given
 |				rectangle.
 |	Input:		aRect	Rectangle that needs redrawing
 |	Output:		None
 \****************************************************************************/

void CMainWindow::Draw(const TRect& aRect)
	{
	CWindowGc* gc = SystemGc(); // get a gc
	gc->SetClippingRect(aRect); // clip outside this rect
	gc->Clear(aRect); // clear
	}

/****************************************************************************\
|	Function:	CMainWindow::HandlePointerEvent
 |	Purpose:	Handles pointer events for CMainWindow.
 |	Input:		aPointerEvent	The pointer event!
 |	Output:		None
 \****************************************************************************/

void CMainWindow::HandlePointerEvent(TPointerEvent& aPointerEvent)
	{
	switch (aPointerEvent.iType)
		{
		case TPointerEvent::EButton1Down:
		case TPointerEvent::EButton1Up:
			break;
		default:
			break;
		}
	}

//////////////////////////////////////////////////////////////////////////////
//					 CExampleWsClient implementation
//////////////////////////////////////////////////////////////////////////////

CExampleWsClient* CExampleWsClient::NewL(const TRect& aRect)
	{
	// make new client
	CExampleWsClient* client = new (ELeave) CExampleWsClient(aRect);
	CleanupStack::PushL(client); // push, just in case
	client->ConstructL(); // construct and run
	CleanupStack::Pop();
	return client;
	}

/****************************************************************************\
|	Function:	Constructor/Destructor for CExampleWsClient
 |				Destructor deletes everything that was allocated by
 |				ConstructMainWindowL()
 \****************************************************************************/

CExampleWsClient::CExampleWsClient(const TRect& aRect) :
	iRect(aRect)
	{
	}

CExampleWsClient::~CExampleWsClient()
	{
	delete iMainWindow;
	delete iNumWin;
	}

/****************************************************************************\
|	Function:	CExampleWsClient::ConstructMainWindowL()
 |				Called by base class's ConstructL
 |	Purpose:	Allocates and creates all the windows owned by this client
 |				(See list of windows in CExampleWsCLient declaration).
 \****************************************************************************/

void CExampleWsClient::ConstructMainWindowL()
	{
	iMainWindow = new (ELeave) CMainWindow(this);
	iMainWindow->ConstructL(iRect, TRgb(255, 255, 255));
	iNumWin = new (ELeave) CNumberedWindow(this, 1);
	TRect rec(iRect);
	rec.Resize(-50, -50);
	iNumWin->ConstructL(rec, TRgb(200, 200, 200), iMainWindow);
	}

/****************************************************************************\
|	Function:	CExampleWsClient::RunL()
 |				Called by active scheduler when an even occurs
 |	Purpose:	Processes events according to their type
 |				For key events: calls HandleKeyEventL() (global to client)
 |				For pointer event: calls HandlePointerEvent() for window
 |                                  event occurred in.
 \****************************************************************************/
void CExampleWsClient::RunL()
	{
	// get the event
	iWs.GetEvent(iWsEvent);
	TInt eventType = iWsEvent.Type();
	// take action on it
	switch (eventType)
		{
		// events global within window group
		case EEventNull:
			break;
		case EEventKey:
			{
			TKeyEvent& keyEvent = *iWsEvent.Key(); // get key event
			HandleKeyEventL(keyEvent);
			break;
			}
		case EEventModifiersChanged:
			break;
		case EEventKeyUp:
		case EEventKeyDown:
		case EEventFocusLost:
		case EEventFocusGained:
		case EEventSwitchOn:
		case EEventPassword:
		case EEventWindowGroupsChanged:
		case EEventErrorMessage:
			break;
			// events local to specific windows
		case EEventPointer:
			{
			CWindow* window = (CWindow*) (iWsEvent.Handle()); // get window
			TPointerEvent& pointerEvent = *iWsEvent.Pointer();
			window->HandlePointerEvent(pointerEvent);
			}
			break;
		case EEventPointerExit:
		case EEventPointerEnter:
		case EEventPointerBufferReady:
		case EEventDragDrop:
			break;
		default:
			break;
		}
	IssueRequest(); // maintain outstanding request
	}

/****************************************************************************\
|	Function:	CExampleWsClient::HandleKeyEventL()
 |	Purpose:	Processes key events for CExampleWsClient
 \****************************************************************************/
void CExampleWsClient::HandleKeyEventL(TKeyEvent& /*aKeyEvent*/)
	{
	}

