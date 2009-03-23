// PngConvert.H
//
// Copyright (c) 1999-2005 Symbian Software Ltd.  All rights reserved.
//

#ifndef __PNGCONVERT_H__
#define __PNGCONVERT_H__

#include <ICL/ImagePlugin.h>

#include "PngCodec.h"

//
// PNG decoder class
//
class CPngDecoder : public CImageDecoderPlugin
	{
public:
	// Construction
 	static CPngDecoder* NewL();
	~CPngDecoder();

private:
	// from CImageDecoderPlugin
	
	// Gets the image type and sub-type for a given frame of the image that
	// has just been decoded.
	virtual void ImageType(TInt aFrameNumber, TUid& aImageType, TUid& aImageSubType) const;	
	// Gets text descriptions of image properties
	CFrameInfoStrings* FrameInfoStringsL(RFs& aFs, TInt aFrameNumber);
	// Scans the image header.
	void ScanDataL();

private:
	CPngDecoder();
	void ReadFormatL();
	};

//
// PNG encoder class
//
class CPngEncoder : public CImageEncoderPlugin
	{
public:
	// Construction
	static CPngEncoder* NewL();
	 ~CPngEncoder();

private:
	// from CImageEncoderPlugin

	// Sets up the codec to encode the frame
	void PrepareEncoderL(const CFrameImageData* aFrameImageData);
	// Update header (not used)
	void UpdateHeaderL() {};

private:
	CPngEncoder();
	};

#endif // __PNGCONVERT_H__
