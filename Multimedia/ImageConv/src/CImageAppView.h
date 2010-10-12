// CImageAppView.H
//
// Copyright (c) 2001 Symbian Ltd.  All rights reserved.
//

#ifndef CIMAGEAPPVIEW
#define CIMAGEAPPVIEW

#include <coecntrl.h> 
#include <fbs.h>

#include "CImageAppUi.h"

// Application view: displays the bitmap
class CImageAppView : public CCoeControl, public MCoeControlBrushContext
    {
public:
    void ConstructL(const TRect& aRect);

    // Get display mode
    TDisplayMode DisplayMode() const;
    // Draw view
    void DrawBitmapNow();
    // Clear view
    void Clear();
    // Set bitmap to draw
    void SetBitmap(CFbsBitmap* aBitmap);

private:
    // from CCoeControl
    void Draw(const TRect& /*aRect*/) const;

private:
    CFbsBitmap* iBitmap;
    };

#endif

