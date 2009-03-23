// PngScanlineEncoder.cpp
//
// Copyright (c) 1997-2002 Symbian Ltd. All rights reserved.
//

#include "PngScanlineEncoder.h"

//
// CPngWriteSubCodec: base class for writing PNG scanlines
//

// Factory function
CPngWriteSubCodec* CPngWriteSubCodec::NewL(const TPngImageInformation& aInfo, const CFbsBitmap* aSource)
	{
	CPngWriteSubCodec* self = NULL;

	switch (aInfo.iBitDepth)
		{
		case 1:
			self = new(ELeave) CBitDepth1Encoder;
			break;
		case 2:
			self = new(ELeave) CBitDepth2Encoder;
			break;
		case 4:
			self = new(ELeave) CBitDepth4Encoder;
			break;
		case 8:
			switch (aInfo.iColorType)
				{
				case TPngImageInformation::EGrayscale:
				case TPngImageInformation::EIndexedColor:
					self = new(ELeave) CBitDepth8Encoder;
					break;
				case TPngImageInformation::EDirectColor:
					self = new(ELeave) CBitDepth8ColorType2Encoder;
					break;
				default:
					User::Leave(KErrNotSupported);
					break;
				}
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	ASSERT(self);
	CleanupStack::PushL(self);
	self->ConstructL(aInfo, aSource);
	CleanupStack::Pop(self);
	return self;
	}

CPngWriteSubCodec::CPngWriteSubCodec()
	: iScanlineDes(NULL, 0)
	{
	}

CPngWriteSubCodec::~CPngWriteSubCodec()
	{
	delete iScanlineBuffer;
	delete iPalette;
	}

void CPngWriteSubCodec::ConstructL(const TPngImageInformation& aInfo, const CFbsBitmap* aSource)
	{
	iInfo = aInfo;
	iSource = aSource;
	iScanlineBufferSize = ScanlineBufferSize(iInfo.iSize.iWidth);
	iScanlineBuffer = HBufC8::NewMaxL(iScanlineBufferSize);
	iScanlineDes.Set(&(iScanlineBuffer->Des())[0], iScanlineBufferSize, iScanlineBufferSize);

	// Init stuff specific to derived class
	DoConstructL();
	}

void CPngWriteSubCodec::DoConstructL()
	{
	}

TDes8& CPngWriteSubCodec::Buffer()
	{
	iScanlineDes.FillZ();
	return iScanlineDes;
	}

TDes8& CPngWriteSubCodec::EncodeL(const TInt aScanline)
	{
	if (aScanline < iInfo.iSize.iHeight)
		{
		TUint8* dataPtr = const_cast<TUint8*>(iScanlineDes.Ptr());
		const TUint8* dataPtrLimit = dataPtr + iScanlineBufferSize;

		DoEncode(iSource, aScanline, dataPtr, dataPtrLimit);
		}
	else
		{
		iScanlineDes.Set(NULL, 0, 0);
		}
	return iScanlineDes;
	}

TUint8 CPngWriteSubCodec::ReverseBits(const TUint8 aValue) const
	{
	TUint value = aValue;
	TUint reverseVal = 0;
	for (TInt i = 0; i < 8; i++)
		{
		reverseVal <<= 1;
		reverseVal |= value & 1;
		value >>= 1;
		}
	return TUint8(reverseVal);
	}

void CPngWriteSubCodec::EncodePalettedScanline(TUint8* aDataPtr, const CFbsBitmap* aSource, const TInt aScanline,
											   const TInt aPixelsPerByte, const TInt aShiftValue)
	{
	// Encode a single scanline with indexes into the current palette
	ASSERT(iInfo.iPalettePresent);
	TPoint pos(0, aScanline);
	const TInt scanLength = iInfo.iSize.iWidth;
	for (TInt i=0; i < scanLength; i += aPixelsPerByte)
		{
		// Pack each byte with 'aPixelsPerByte' index values
		TUint8 pixels = 0;
		for (TInt j=0; j < aPixelsPerByte; j++)
			{
			pixels <<= aShiftValue;
			TRgb rgb;
			aSource->GetPixel(rgb, pos);
			pixels |= TUint8(iPalette->NearestIndex(rgb));
			pos.iX++;
			}
		*aDataPtr = pixels;
		aDataPtr++;
		}
	}

//
// These classes specialise the PNG writer to write
// scanlines with different bitmap depths/colour types
//

//
// CBitDepth1Encoder
//

void CBitDepth1Encoder::DoConstructL()
	{
	if (iInfo.iColorType == TPngImageInformation::EIndexedColor)
		{
		// Setup palette
		iPalette = CPalette::NewL(2);
		iPalette->SetEntry(0, KRgbBlack);
		iPalette->SetEntry(1, KRgbWhite);
		iInfo.iPalettePresent = ETrue;
		}
	}

TInt CBitDepth1Encoder::ScanlineBufferSize(TInt aPixelLength)
	{
	// 8 pixels per byte
	return ((aPixelLength + KPngDepth1RoundUpValue) / KPngDepth1PixelsPerByte) + KPngScanlineFilterTypeLength;
	}

void CBitDepth1Encoder::DoEncode(const CFbsBitmap* aSource, const TInt aScanline,
								 TUint8* aDataPtr, const TUint8* aDataPtrLimit)
	{
	// Filter method
	PtrWriteUtil::WriteInt8(aDataPtr, iInfo.iFilterMethod);
	aDataPtr++;

	// Pixel data
	const TInt scanLength = iInfo.iSize.iWidth;
	if (iInfo.iColorType == TPngImageInformation::EIndexedColor)
		{
		// Write palette indexes
		EncodePalettedScanline(aDataPtr, aSource, aScanline, KPngDepth1PixelsPerByte, KPngDepth1ShiftValue);
		}
	else
		{
		// Write RGB data
		TInt dataLength = (scanLength + KPngDepth1RoundUpValue) / KPngDepth1PixelsPerByte;
		TPtr8 dataPtr(aDataPtr, dataLength, dataLength);

		aSource->GetScanLine(dataPtr, TPoint(0, aScanline), scanLength, EGray2);

		// Reverse the order of the bits
		while (aDataPtr < aDataPtrLimit)
			{
			aDataPtr[0] = ReverseBits(aDataPtr[0]);
			aDataPtr++;
			}
		}
	}

//
// CBitDepth2Encoder
//

void CBitDepth2Encoder::DoConstructL()
	{
	if (iInfo.iColorType == TPngImageInformation::EIndexedColor)
		{
		// Setup palette entries
		iPalette = CPalette::NewL(4);
		iPalette->SetEntry(0, KRgbBlack);
		iPalette->SetEntry(1, KRgbDarkGray);
		iPalette->SetEntry(2, KRgbGray);
		iPalette->SetEntry(3, KRgbWhite);
		iInfo.iPalettePresent = ETrue;
		}
	}

TInt CBitDepth2Encoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return ((aPixelLength + KPngDepth2RoundUpValue) / KPngDepth2PixelsPerByte) + KPngScanlineFilterTypeLength;
	}

void CBitDepth2Encoder::DoEncode(const CFbsBitmap* aSource, const TInt aScanline,
								 TUint8* aDataPtr, const TUint8* aDataPtrLimit)
	{
	// Filter method
	PtrWriteUtil::WriteInt8(aDataPtr, iInfo.iFilterMethod);
	aDataPtr++;

	// Pixel data
	const TInt scanLength = iInfo.iSize.iWidth;
	if (iInfo.iColorType == TPngImageInformation::EIndexedColor)
		{
		// Write palette indexes
		EncodePalettedScanline(aDataPtr, aSource, aScanline, KPngDepth2PixelsPerByte, KPngDepth2ShiftValue);
		}
	else
		{
		// RGB values
		TPtr8 dataPtr(aDataPtr, (scanLength + KPngDepth2RoundUpValue) / KPngDepth2PixelsPerByte);
		aSource->GetScanLine(dataPtr, TPoint(0, aScanline), scanLength, EGray4);

		// Reverse the order of the bits
		while (aDataPtr < aDataPtrLimit)
			{
			TUint8 value = aDataPtr[0];
			TUint8 reverse = 0;
			for (TInt i=0; i < KPngDepth2PixelsPerByte; i++)
				{
				reverse <<= 2;	// advance the bits for the reverse value
				reverse |= value & 0x3;	// mask off the 2 bits, then OR with existing reverse value
				value >>= 2;	// advance the bits for the actual value
				}
			aDataPtr[0] = reverse;
			aDataPtr++;
			}
		}
	}

//
// CBitDepth4Encoder
//

void CBitDepth4Encoder::DoConstructL()
	{
	if (iInfo.iColorType == TPngImageInformation::EIndexedColor)
		{
		// Calculate palette for image
		iPalette = CPalette::NewDefaultL(EColor16);
		iInfo.iPalettePresent = ETrue;
		}
	}

TInt CBitDepth4Encoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return ((aPixelLength + KPngDepth4RoundUpValue) / KPngDepth4PixelsPerByte) + KPngScanlineFilterTypeLength;
	}

void CBitDepth4Encoder::DoEncode(const CFbsBitmap* aSource, const TInt aScanline,
								 TUint8* aDataPtr, const TUint8* aDataPtrLimit)
	{
	// Filter method
	PtrWriteUtil::WriteInt8(aDataPtr, iInfo.iFilterMethod);
	aDataPtr++;

	// Pixel data
	const TInt scanLength = iInfo.iSize.iWidth;
	if (iInfo.iColorType == TPngImageInformation::EIndexedColor)
		{
		// Write palette indexes
		EncodePalettedScanline(aDataPtr, aSource, aScanline, KPngDepth4PixelsPerByte, KPngDepth4ShiftValue);
		}
	else
		{
		TPtr8 dataPtr(aDataPtr, (scanLength + KPngDepth4RoundUpValue) / KPngDepth4PixelsPerByte);
		aSource->GetScanLine(dataPtr, TPoint(0, aScanline), scanLength,
			(iInfo.iColorType == TPngImageInformation::EDirectColor) ? EColor16 : EGray16);

		// Swap order of the low/high bits
		while (aDataPtr < aDataPtrLimit)
			{
			TUint value = aDataPtr[0];
			TUint low = value << KPngDepth4ShiftValue;
			TUint high = value >> KPngDepth4ShiftValue;
			aDataPtr[0] = TUint8(low | high);
			aDataPtr++;
			}
		}
	}

//
// CBitDepth8ColorType2Encoder
//

TInt CBitDepth8ColorType2Encoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return (aPixelLength * KPngDepth8RgbBytesPerPixel) + KPngScanlineFilterTypeLength;
	}

void CBitDepth8ColorType2Encoder::DoEncode(const CFbsBitmap* aSource, const TInt aScanline,
										   TUint8* aDataPtr, const TUint8* aDataPtrLimit)
	{
	// Filter method
	PtrWriteUtil::WriteInt8(aDataPtr, iInfo.iFilterMethod);
	aDataPtr++;

	// Pixel data
	TPtr8 dataPtr(aDataPtr, iInfo.iSize.iWidth * KPngDepth8RgbBytesPerPixel);
	aSource->GetScanLine(dataPtr, TPoint(0, aScanline), iInfo.iSize.iWidth, EColor16M);

	while (aDataPtr < aDataPtrLimit)
		{
		// Swap the red and blue components of the image data
		TUint8 temp = aDataPtr[0];	// temp = Red
		aDataPtr[0] = aDataPtr[2];	// Red = Blue
		aDataPtr[2] = temp;			// Blue = temp
		aDataPtr += KPngDepth8RgbBytesPerPixel;
		}
	}

//
// CBitDepth8Encoder
//

void CBitDepth8Encoder::DoConstructL()
	{
	if (iInfo.iColorType == TPngImageInformation::EIndexedColor)
		{
		// Calculate palette for image
		iPalette = CPalette::NewDefaultL(EColor256);
		iInfo.iPalettePresent = ETrue;
		}
	}

TInt CBitDepth8Encoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return aPixelLength + KPngScanlineFilterTypeLength;
	}

void CBitDepth8Encoder::DoEncode(const CFbsBitmap* aSource, const TInt aScanline,
								 TUint8* aDataPtr, const TUint8* /*aDataPtrLimit*/)
	{
	// Filter method
	PtrWriteUtil::WriteInt8(aDataPtr, iInfo.iFilterMethod);
	aDataPtr++;

	const TInt scanLength = iInfo.iSize.iWidth;
	if (iInfo.iColorType == TPngImageInformation::EIndexedColor)
		{
		// Write palette indexes
		EncodePalettedScanline(aDataPtr, aSource, aScanline, KPngDepth8PixelsPerByte, KPngDepth8ShiftValue);
		}
	else
		{
		// Pixel data
		TPtr8 dataPtr(aDataPtr, scanLength);
		aSource->GetScanLine(dataPtr, TPoint(0, aScanline), scanLength, EGray256);
		}
	}
