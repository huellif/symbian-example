// Copyright (c) 2001-2005 Symbian Software Ltd.  All rights reserved.

#include "CImageHandler.h"

CImageHandler::CImageHandler(CFbsBitmap& aBitmap, RFs& aFs, MImageHandlerCallback& aCallback)
	:	CActive(CActive::EPriorityStandard), iBitmap(aBitmap), iFs(aFs), iCallback(aCallback)
	{}

void CImageHandler::ConstructL()
	{
	iBitmapRotator = CBitmapRotator::NewL();
	iBitmapScaler = CBitmapScaler::NewL();
	CActiveScheduler::Add(this);
	}

CImageHandler::~CImageHandler()
	{
	delete iLoadUtil;
	delete iSaveUtil;
	delete iBitmapRotator;
	delete iBitmapScaler;
	}

void CImageHandler::LoadFileL(const TFileName& aFileName, TInt aSelectedFrame)
	{
	__ASSERT_ALWAYS(!IsActive(),User::Invariant());
	// create a CImageDecoder to read the specified file
	delete iLoadUtil;
	iLoadUtil = NULL;
	iLoadUtil = CImageDecoder::FileNewL(iFs, aFileName);
	// store the frame information and frame count
	iFrameInfo = iLoadUtil->FrameInfo(aSelectedFrame);
	iFrameCount = iLoadUtil->FrameCount();
	// resize the destination bitmap to fit the required size
	TRect bitmapSize = iFrameInfo.iFrameCoordsInPixels;
	iBitmap.Resize(bitmapSize.Size());
	// start reading the bitmap: RunL called when complete
	iLoadUtil->Convert(&iStatus, iBitmap, aSelectedFrame);
	SetActive();
	}

void CImageHandler::SaveFileL(const TFileName& aFileName, const TUid& aImageType, const TUid& aImageSubType)
	{
	__ASSERT_ALWAYS(!IsActive(),User::Invariant());
	// create a CImageEncoder to save the bitmap to the specified file in the specified format
	delete iSaveUtil;
	iSaveUtil = NULL;
	iSaveUtil = CImageEncoder::FileNewL(iFs, aFileName, CImageEncoder::EOptionNone, aImageType, aImageSubType);
	// start saving the bitmap: RunL called when complete
	iSaveUtil->Convert(&iStatus, iBitmap);
	SetActive();
	}

void CImageHandler::Mirror()
	{
	__ASSERT_ALWAYS(!IsActive(),User::Invariant());
	// start rotating the bitmap: RunL called when complete
	iBitmapRotator->Rotate(&iStatus, iBitmap, CBitmapRotator::EMirrorVerticalAxis);
	SetActive();
	}

void CImageHandler::Flip()
	{
	__ASSERT_ALWAYS(!IsActive(),User::Invariant());
	// start rotating the bitmap: RunL called when complete
	iBitmapRotator->Rotate(&iStatus, iBitmap, CBitmapRotator::EMirrorHorizontalAxis);
	SetActive();
	}

void CImageHandler::FrameRotate(TBool aClockwise)
	{
	__ASSERT_ALWAYS(!IsActive(),User::Invariant());
	// start rotating the bitmap: RunL called when complete
	if (aClockwise)
		iBitmapRotator->Rotate(&iStatus, iBitmap, CBitmapRotator::ERotation90DegreesClockwise);
	else
		iBitmapRotator->Rotate(&iStatus, iBitmap, CBitmapRotator::ERotation270DegreesClockwise);
	SetActive();
	}

void CImageHandler::ZoomFrame(TBool aZoomIn)
	{
	__ASSERT_ALWAYS(!IsActive(),User::Invariant());
	// Determine target zooming size
	TSize size(iBitmap.SizeInPixels());
	const TSize adjust(size.iWidth/2, size.iHeight/2);
	if (aZoomIn)
		size += adjust;
	else
		size -= adjust;
	// Don't let it go too small
	if (size.iWidth <= 10) size.iWidth = 10;
	if (size.iHeight <= 10) size.iHeight = 10;

	// start scaling the bitmap: RunL called when complete
	iBitmapScaler->Scale(&iStatus, iBitmap, size);
	SetActive();
	}

void CImageHandler::RunL()
	{
	// Operation complete, call back caller
	iCallback.ImageOperationComplete(iStatus.Int());
	}
	 
void CImageHandler::DoCancel()
	{
	// Cancel everything possible
	if (iLoadUtil) iLoadUtil->Cancel();
	if (iSaveUtil) iSaveUtil->Cancel();
	iBitmapRotator->Cancel();
	iBitmapScaler->Cancel(); 
	}
