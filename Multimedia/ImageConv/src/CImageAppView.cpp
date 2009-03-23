//
// CImageAppView
//
// Copyright (c) 2001-2005 Symbian Software Ltd.  All rights reserved.

#include "CImageAppView.h"

// Construct empty window
void CImageAppView::ConstructL(const TRect& /*aRect*/)
    {
	CreateWindowL();
#if defined(__WINS__)
	Window().SetRequiredDisplayMode(EColor256);
#endif
	Window().SetBackgroundColor(KRgbDarkBlue);
    SetExtentToWholeScreen();
	ActivateL();
	}

// Get display mode
TDisplayMode CImageAppView::DisplayMode() const
	{
	return Window().DisplayMode();
	}

// Draw view
void CImageAppView::Draw(const TRect& /*aRect*/) const
	{
	CWindowGc& gc = SystemGc();

	if (iBitmap && iBitmap->Handle())
		{
		TSize bitmapSize(iBitmap->SizeInPixels());
		TPoint pt;
		pt.iX = (Rect().Width() - bitmapSize.iWidth) / 2;
		pt.iY = (Rect().Height() - bitmapSize.iHeight) / 2;
		gc.BitBlt(pt,iBitmap);
		}
	else
		gc.Clear();
	}

// Draw view on demand
void CImageAppView::DrawBitmapNow()
	{
	CWindowGc& gc = SystemGc();
	gc.Activate(Window());
	Draw(Rect());
	gc.Deactivate();
	ControlEnv()->WsSession().Flush();
	}

// Clear view
void CImageAppView::Clear()
	{
	CWindowGc& gc = SystemGc();
	gc.Activate(Window());
	gc.Clear();
	gc.Deactivate();
	ControlEnv()->WsSession().Flush();
	}

// Set bitmap to draw
void CImageAppView::SetBitmap(CFbsBitmap* aBitmap)
	{
	iBitmap = aBitmap;
	}
