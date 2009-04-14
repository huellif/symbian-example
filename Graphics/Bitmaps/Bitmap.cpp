// Bitmap.cpp
//
// Copyright (c) 2000-2005 Symbian Software Ltd.  All rights reserved.
#include "BitmapsGraphicsControl.h"
#include <grbmap.mbg>

_LIT(KTxtCDrive,"C:");
_LIT(KTxtZDrive,"Z:");

void CBitmapControl::LoadBitmapL(CFbsBitmap* aBitMap,
		const TDesC& aPathAndFile, TInt aId, TBool aShareIfLoaded)
	{
	TParse mbfn;

	mbfn.Set(aPathAndFile, &KTxtCDrive, NULL);
	if (!aBitMap->Load(mbfn.FullName(), aId, aShareIfLoaded))
		{
		return;
		}

	mbfn.Set(aPathAndFile, &KTxtZDrive, NULL);
	User::LeaveIfError(aBitMap->Load(mbfn.FullName(), aId, aShareIfLoaded));
	return;
	}

// Text printed to the console
_LIT(KTxtCase1, "draw piece of bitmap using block transfer");
_LIT(KTxtCase2, "draw bitmap described in twips using DrawBitmap()");
_LIT(KTxtCase3, "draw stretched bitmap");
_LIT(KTxtCase4, "tile rectangle, using bitmap as the brush pattern");
_LIT(KTxtCase5, "tile rectangle, tiling around center of screen");
_LIT(KTxtCase6, "masks: the problem of drawing a bitmap on different backgrounds");
_LIT(KTxtCase7, "masks: using a mask to give a bitmap a transparent background");

// The name of the multi-bitmap file containing the bitmap
// and bitmap mask files.
_LIT(KMbmName, "\\resource\\apps\\grbmap.mbm");

void CBitmapControl::UpdateModelL()
	{
	// set up name for bitmap sharing
	TBool shareIfLoaded(ETrue);

	switch (Phase())
		{
		case 0:
			{
			// load the bitmap and mask bitmap
			iBitmap = new (ELeave) CFbsBitmap();
			LoadBitmapL(iBitmap, KMbmName, EMbmGrbmapSmiley, shareIfLoaded);
			iMaskBitmap = new (ELeave) CFbsBitmap();
			LoadBitmapL(iMaskBitmap, KMbmName, EMbmGrbmapSmilmask,
					shareIfLoaded);

			_LIT(KTxtCase0, "draw bitmap, centered on screen using block transfer");
			iGraphObserver->NotifyStatus(KTxtCase0);
			}
			break;
		
		case 1:
			{
			iGraphObserver->NotifyStatus(KTxtCase1);
			}
			break;
		
		case 2:
			{
			iGraphObserver->NotifyStatus(KTxtCase2);
			}
			break;
		
		case 3:
			{
			iGraphObserver->NotifyStatus(KTxtCase3);
			}
			break;
		
		case 4:
			{
			iGraphObserver->NotifyStatus(KTxtCase4);
			}
			break;
		
		case 5:
			{
			iGraphObserver->NotifyStatus(KTxtCase5);
			}
			break;
			
		case 6:
			{
			iGraphObserver->NotifyStatus(KTxtCase6);
			}
			break;
		
		case 7:
			{
			iGraphObserver->NotifyStatus(KTxtCase7);
			}
			break;
		}
	}

void CBitmapControl::Draw(const TRect& /* aRect */) const
	{
	// draw surrounding rectangle
	CWindowGc& gc = SystemGc(); // graphics context we draw to
	gc.UseFont(iMessageFont); // use the system message font
	gc.Clear(); // clear the area to be drawn to
	gc.DrawRect(Rect()); // surrounding rectangle to draw into

	switch (Phase())
		{
		case 0:
			{
			CaseBitBlt(gc);
			}
			break;

		case 1:
			{
			CaseBitBltPiece(gc);
			}
			break;

		case 2:
			{
			CaseDrawBitmapTopLeft(gc);
			}
			break;

		case 3:
			{
			CaseDrawBitmapRect(gc);
			}
			break;

		case 4:
			{
			CaseBrushPattern(gc);
			}
			break;

		case 5:
			{
			CaseBrushPatternOrigin(gc);
			}
			break;

		case 6:
			{
			CaseDiffBackground(gc);
			}
			break;

		case 7:
			{
			CaseDiffBackgroundDetailed(gc);
			}
			break;

		default:
			break;
		}
	}

TPoint CBitmapControl::DeltaPos(TSize aBmpSize, TSize aCtrlSize) const
	{
	// calculate position for top left of bitmap so it is centered
	TInt xDelta = (aCtrlSize.iWidth - aBmpSize.iWidth) / 2;
	TInt yDelta = (aCtrlSize.iHeight - aBmpSize.iHeight) / 2;
	return TPoint(xDelta, yDelta);
	}

void CBitmapControl::CaseBitBlt(CWindowGc &aGc) const
	{
	// draw a whole bitmap centered on the screen,
	// using bitmap block transfer
	TPoint pos = Position() + DeltaPos(iBitmap->SizeInPixels(), Size()); 
	aGc.BitBlt(pos, iBitmap);
	}

void CBitmapControl::CaseBitBltPiece(CWindowGc & gc) const
	{
	// draw a rectangular piece of a bitmap, centered on the screen,
	// using bitmap block transfer
	// calculate bitmap piece, half size from center of source bitmap
	TSize bmpSize = iBitmap->SizeInPixels();
	TSize bmpPieceSize(bmpSize.iWidth * 2 / 3, bmpSize.iHeight * 2 / 3);

	TPoint bmpPieceTopLeft(0, 0);
	TRect bmpPieceRect;
	bmpPieceRect.SetRect(bmpPieceTopLeft, bmpPieceSize);

	TPoint pos = Position() + DeltaPos(bmpPieceSize, Size());
	gc.BitBlt(pos, iBitmap, bmpPieceRect); // using bitmap piece
	}

void CBitmapControl::CaseDrawBitmapTopLeft(CWindowGc & gc) const
	{
	// draw a bitmap to a defined size in twips
	// in the top left corner the rectangle,
	// using the GDI DrawBitmap() function
	TSize bmpSizeInTwips(600, 600); // must set twips size, default (0,0)
	iBitmap->SetSizeInTwips(bmpSizeInTwips);
	TRect rect = Rect();
	gc.DrawBitmap(rect.iTl, iBitmap);
	}

void CBitmapControl::CaseDrawBitmapRect(CWindowGc & gc) const
	{
	// draw a stretched bitmap inside the rectangle,
	// using the GDI DrawBitmap() function
	TRect rect = Rect();
	gc.DrawBitmap(rect, iBitmap);
	}

void CBitmapControl::CaseBrushPattern(CWindowGc & gc) const
	{
	// use bitmap as brush pattern, tiling from top left of rectangle
	// set brush pattern and style to use the bitmap
	gc.UseBrushPattern(iBitmap);
	gc.SetBrushStyle(CGraphicsContext::EPatternedBrush);
	TRect rect = Rect();
	gc.DrawRect(rect);
	gc.DiscardBrushPattern();
	}

void CBitmapControl::CaseBrushPatternOrigin(CWindowGc & gc) const
	{
	// use bitmap as brush pattern, tiling around center of screen
	// set brush pattern and style to use the bitmap'
	TPoint screenCenterPoint = Rect().Center();
	gc.SetBrushOrigin(screenCenterPoint);
	gc.UseBrushPattern(iBitmap);
	gc.SetBrushStyle(CGraphicsContext::EPatternedBrush);
	TRect rect = Rect();
	gc.DrawRect(rect);
	gc.DiscardBrushPattern();
	}

void CBitmapControl::CaseDiffBackground(CWindowGc & gc) const
	{
	// bisect screen into two different coloured rects
	TRect screenRect = Rect();
	TInt bisect = (screenRect.iBr.iX - screenRect.iTl.iX) / 2
			+ screenRect.iTl.iX;
	TRect leftRect(screenRect.iTl, TPoint(bisect, screenRect.iBr.iY));
	TRect rightRect(TPoint(bisect, screenRect.iTl.iY), screenRect.iBr);
	TRgb darkGray(85, 85, 85);
	gc.SetBrushColor(darkGray);
	gc.Clear(leftRect);
	TRgb black(0, 0, 0);
	gc.SetBrushColor(black);
	gc.Clear(rightRect);
	TSize bmpSizeInPixels = iBitmap->SizeInPixels();
	TSize bmpPieceSize(bmpSizeInPixels.iWidth, bmpSizeInPixels.iHeight);
	TPoint bmpPieceTopLeft(0, 0);
	TRect bmpPieceRect;
	bmpPieceRect.SetRect(bmpPieceTopLeft, bmpPieceSize);
	// center bitmap on left
	TInt xDelta = (leftRect.Width() - bmpPieceRect.Width()) / 2;
	TInt yDelta = (leftRect.Height() - bmpPieceRect.Height()) / 2;
	TPoint pos = TPoint(xDelta, yDelta); // displacement vector
	pos += leftRect.iTl; // bitmap piece top left corner position
	gc.BitBlt(pos, iBitmap);
	// center bitmap on right
	xDelta = (rightRect.Width() - bmpPieceRect.Width()) / 2;
	yDelta = (rightRect.Height() - bmpPieceRect.Height()) / 2;
	TPoint pos2 = TPoint(xDelta, yDelta); // displacement vector
	pos2 += rightRect.iTl; // bitmap piece top left corner position
	gc.BitBlt(pos2, iBitmap);
	}

void CBitmapControl::CaseDiffBackgroundDetailed(CWindowGc & gc) const
	{

	// bisect screen into two different coloured rects
	TRect screenRect = Rect();
	TInt bisect = (screenRect.iBr.iX - screenRect.iTl.iX) / 2
			+ screenRect.iTl.iX;
	TRect leftRect(TPoint(screenRect.iTl.iX, screenRect.iTl.iY + 50), TPoint(
			bisect, screenRect.iBr.iY));
	TRect rightRect(TPoint(bisect, screenRect.iTl.iY + 50), screenRect.iBr);
	TRgb darkGray(85, 85, 85);
	gc.SetBrushColor(darkGray);
	gc.Clear(leftRect);
	TRgb black(0, 0, 0);
	gc.SetBrushColor(black);
	gc.Clear(rightRect);

	TSize bmpSizeInPixels = iBitmap->SizeInPixels();
	TSize bmpPieceSize(bmpSizeInPixels.iWidth, bmpSizeInPixels.iHeight);
	TPoint bmpPieceTopLeft(0, 0);
	TRect bmpPieceRect;
	bmpPieceRect.SetRect(bmpPieceTopLeft, bmpPieceSize);

	// center bitmap on left
	TInt xDelta = (leftRect.Width() - bmpPieceRect.Width()) / 2;
	TInt yDelta = (leftRect.Height() - bmpPieceRect.Height()) / 2;
	TPoint pos = TPoint(xDelta, yDelta); // displacement vector
	pos += leftRect.iTl; // bitmap piece top left corner position
	gc.BitBltMasked(pos, iBitmap, bmpPieceRect, iMaskBitmap, EFalse); // CWindowGc member function

	// center bitmap on right
	xDelta = (rightRect.Width() - bmpPieceRect.Width()) / 2;
	yDelta = (rightRect.Height() - bmpPieceRect.Height()) / 2;
	TPoint pos2 = TPoint(xDelta, yDelta); // displacement vector
	pos2 += rightRect.iTl; // bitmap piece top left corner position
	gc.BitBltMasked(pos2, iBitmap, bmpPieceRect, iMaskBitmap, EFalse); // CWindowGc member function

	_LIT(KTxtTheBitmap, "The bitmap:");
	_LIT(KTxtBitmapMask, "The bitmap's mask:");

	gc.DrawText(KTxtTheBitmap, TPoint(5, 20));
	gc.BitBlt(TPoint(130, 0), iBitmap);
	gc.DrawText(KTxtBitmapMask, TPoint(197, 20));
	gc.BitBlt(TPoint(400, 0), iMaskBitmap);
	}
