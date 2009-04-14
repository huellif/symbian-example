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

void CBitmapControl::CaseBitBltPiece(CWindowGc& aGc) const
	{
	// draw a rectangular piece of a bitmap, centered on the screen,
	// using bitmap block transfer
	// calculate bitmap piece, half size from center of source bitmap
	TSize bmpSize = iBitmap->SizeInPixels();
	TSize bmpPieceSize(bmpSize.iWidth * 2 / 3, bmpSize.iHeight * 2 / 3);

	TPoint pos = Position() + DeltaPos(bmpPieceSize, Size());
	TRect bmpPieceRect(TPoint(0, 0), bmpPieceSize);

	aGc.BitBlt(pos, iBitmap, bmpPieceRect); // using bitmap piece
	}

void CBitmapControl::CaseDrawBitmapTopLeft(CWindowGc& aGc) const
	{
	// draw a bitmap to a defined size in twips
	// in the top left corner the rectangle,
	// using the GDI DrawBitmap() function
	TSize bmpSizeInTwips(600, 600); // must set twips size, default (0,0)
	iBitmap->SetSizeInTwips(bmpSizeInTwips);
	aGc.DrawBitmap(Position(), iBitmap);
	}

void CBitmapControl::CaseDrawBitmapRect(CWindowGc& aGc) const
	{
	// draw a stretched bitmap inside the rectangle,
	// using the GDI DrawBitmap() function
	aGc.DrawBitmap(Rect(), iBitmap);
	}

void CBitmapControl::CaseBrushPattern(CWindowGc& aGc) const
	{
	// use bitmap as brush pattern, tiling from top left of rectangle
	// set brush pattern and style to use the bitmap
	aGc.UseBrushPattern(iBitmap);
	aGc.SetBrushStyle(CGraphicsContext::EPatternedBrush);
	aGc.DrawRect(Rect());
	aGc.DiscardBrushPattern();
	}

void CBitmapControl::CaseBrushPatternOrigin(CWindowGc& aGc) const
	{
	// use bitmap as brush pattern, tiling around center of screen
	// set brush pattern and style to use the bitmap'
	aGc.SetBrushOrigin(Rect().Center());
	aGc.UseBrushPattern(iBitmap);
	aGc.SetBrushStyle(CGraphicsContext::EPatternedBrush);
	aGc.DrawRect(Rect());
	aGc.DiscardBrushPattern();
	}

void CBitmapControl::CaseDiffBackground(CWindowGc& aGc) const
	{
	// bisect screen into two different colourred recsts
	TSize biSize;
	biSize.iWidth = Size().iWidth / 2;
	biSize.iHeight = Size().iHeight;

	TRect leftRect(Position(), biSize);
	TRect rightRect((Position() + TPoint(biSize.iWidth, 0)), biSize);
	
	aGc.SetBrushColor(KRgbDarkGray);
	aGc.Clear(leftRect);

	aGc.SetBrushColor(KRgbBlack);
	aGc.Clear(rightRect);
	
	// center bitmap on left
	TPoint pos1 = leftRect.iTl + DeltaPos(iBitmap->SizeInPixels(), leftRect.Size());
	aGc.BitBlt(pos1, iBitmap);

	// center bitmap on right
	TPoint pos2 = rightRect.iTl + DeltaPos(iBitmap->SizeInPixels(), rightRect.Size());
	aGc.BitBlt(pos2, iBitmap);
	}

void CBitmapControl::CaseDiffBackgroundDetailed(CWindowGc& aGc) const
	{
	// bisect screen into two different colourred recsts
	TSize biSize;
	biSize.iWidth = Size().iWidth / 2;
	biSize.iHeight = Size().iHeight;
	
	TRect leftRect((Position() + TPoint(0, 50)), biSize);
	TRect rightRect((Position() + TPoint(0, 50)) + TPoint(biSize.iWidth, 0), biSize);
	
	aGc.SetBrushColor(KRgbDarkGray);
	aGc.Clear(leftRect);
	
	aGc.SetBrushColor(KRgbBlack);
	aGc.Clear(rightRect);
	
	TRect bmpRect(TPoint(0, 0), iBitmap->SizeInPixels());

	// center bitmap on left
	TPoint pos1 = leftRect.iTl + DeltaPos(iBitmap->SizeInPixels(), leftRect.Size());
	aGc.BitBltMasked(pos1, iBitmap, bmpRect, iMaskBitmap, EFalse);
	
	// center bitmap on right
	TPoint pos2 = rightRect.iTl + DeltaPos(iBitmap->SizeInPixels(), rightRect.Size());
	aGc.BitBltMasked(pos2, iBitmap, bmpRect, iMaskBitmap, EFalse);

	_LIT(KTxtTheBitmap, "The bitmap:");
	_LIT(KTxtBitmapMask, "The bitmap's mask:");

	aGc.DrawText(KTxtTheBitmap, TPoint(5, 20));
	aGc.BitBlt(TPoint(130, 0), iBitmap);
	aGc.DrawText(KTxtBitmapMask, TPoint(197, 20));
	aGc.BitBlt(TPoint(400, 0), iMaskBitmap);
	}
