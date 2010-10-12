// CImageAppView.H
//
// Copyright (c) 2002-2005 Symbian Software Ltd.  All rights reserved.
//

#ifndef CIMAGEHANDLER_H
#define CIMAGEHANDLER_H

#include <e32base.h> 
#include <fbs.h>

// Image converter library API header
#include <ImageConversion.h>
// Bitmap transforms API header
#include <BitmapTransforms.h>

// Callback from image handler
class MImageHandlerCallback
    {
public:
    // Handler calls when image manipulation is completed
    virtual void ImageOperationComplete(TInt aError) = 0;
    };

// Loads, saves and manipulates a bitmap
class CImageHandler : public CActive
    {
public:
    // Construction
    CImageHandler(CFbsBitmap& aBitmap, RFs& aFs,
            MImageHandlerCallback& aCallback);
    void ConstructL();
    ~CImageHandler();

    // Load/save operations
    void LoadFileL(const TFileName& aFileName, TInt aSelectedFrame = 0);
    void SaveFileL(const TFileName& aFileName, const TUid& aImageType,
            const TUid& aImageSubType);

    // Image command handling functions
    void FrameRotate(TBool aClockwise);
    void Mirror();
    void Flip();
    void ZoomFrame(TBool aZoomIn);

    // Frame information
    const TFrameInfo& FrameInfo() const
        {
        return iFrameInfo;
        }
    // Frame count
    TInt FrameCount() const
        {
        return iFrameCount;
        }

private:
    // Active object interface
    void RunL();
    void DoCancel();

private:
    // Bitmap
    CFbsBitmap& iBitmap;
    // File server handle
    RFs& iFs;
    // Callback interface
    MImageHandlerCallback& iCallback;
    // Frame information
    TFrameInfo iFrameInfo;
    // Frame count
    TInt iFrameCount;
    // Image file loader
    CImageDecoder* iLoadUtil;
    // Image files saver
    CImageEncoder* iSaveUtil;
    // Bitmap rotator
    CBitmapRotator* iBitmapRotator;
    // Bitmap zoomer
    CBitmapScaler* iBitmapScaler;
    };

#endif
