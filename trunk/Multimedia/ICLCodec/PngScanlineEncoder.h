// PngScanlineEncoder.h
//
// Copyright (c) 1997-2005 Symbian Software Ltd.  All rights reserved.
//

#ifndef __PNGSCANLINEENCODER_H__
#define __PNGSCANLINEENCODER_H__

#include "PngCodec.h"
#include "ImageUtils.h"

//
// These classes specialise the PNG writer to write
// scanlines with different bitmap depths/colour types
//

const TInt KPngScanlineFilterTypeLength = 1;

//
// CBitDepth1Encoder: specialised writer for 1bpp
//

const TInt KPngDepth1PixelsPerByte = 8;
const TInt KPngDepth1RoundUpValue = 7;
const TInt KPngDepth1ShiftValue = 1;

class CBitDepth1Encoder : public CPngWriteSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoEncode(const CFbsBitmap* aSource, const TInt aScanline,
					TUint8* aDataPtr, const TUint8* aDataPtrLimit);
	};

//
// CBitDepth2Encoder: specialised writer for 2bpp
//

const TInt KPngDepth2PixelsPerByte = 4;
const TInt KPngDepth2RoundUpValue = 3;
const TInt KPngDepth2ShiftValue = 2;
class CBitDepth2Encoder : public CPngWriteSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoEncode(const CFbsBitmap* aSource, const TInt aScanline,
					TUint8* aDataPtr, const TUint8* aDataPtrLimit);
	};

//
// CBitDepth4Encoder: specialised writer for 4bpp
//

const TInt KPngDepth4PixelsPerByte = 2;
const TInt KPngDepth4RoundUpValue = 1;
const TInt KPngDepth4ShiftValue = 4;
class CBitDepth4Encoder : public CPngWriteSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoEncode(const CFbsBitmap* aSource, const TInt aScanline,
					TUint8* aDataPtr, const TUint8* aDataPtrLimit);
	};

//
// CBitDepth8ColorType2Encoder: specialised writer for 8bpp, RGB colour
//
const TInt KPngDepth8RgbBytesPerPixel = 3;
class CBitDepth8ColorType2Encoder : public CPngWriteSubCodec
	{
private:
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoEncode(const CFbsBitmap* aSource, const TInt aScanline,
					TUint8* aDataPtr, const TUint8* aDataPtrLimit);
	};

//
// CBitDepth8Encoder: specialised writer for 8bpp
//

const TInt KPngDepth8PixelsPerByte = 1;
const TInt KPngDepth8ShiftValue = 0;
class CBitDepth8Encoder : public CPngWriteSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoEncode(const CFbsBitmap* aSource, const TInt aScanline,
					TUint8* aDataPtr, const TUint8* aDataPtrLimit);
	};

#endif // __PNGSCANLINEENCODER_H__
