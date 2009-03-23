// PNGSCANLINEDECODER.CPP
//
// Copyright (c) 1997-2002 Symbian Ltd.  All rights reserved.
//

#include "PNGCodec.h"

const TInt KPngScanlineFilterTypeLength = 1;

const TInt KColStart[KPngNumInterlacedPasses] = { 0, 4, 0, 2, 0, 1, 0, 0 };
const TInt KRowStart[KPngNumInterlacedPasses] = { 0, 0, 4, 0, 2, 0, 1, 0 };
const TInt KColIncrement[KPngNumInterlacedPasses] = { 8, 8, 4, 4, 2, 2, 1, 0 };
const TInt KRowIncrement[KPngNumInterlacedPasses] = { 8, 8, 8, 4, 4, 2, 2, 0 };
const TInt KBlockWidth[KPngNumInterlacedPasses] = { 8, 4, 4, 2, 2, 1, 1, 0 };
const TInt KBlockHeight[KPngNumInterlacedPasses] = { 8, 8, 4, 4, 2, 2, 1, 0 };

//
// These classes specialise the PNG reader to read
// scanlines with different bitmap depths/colour types
//

class CBitDepth1Decoder : public CPngReadSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit);
	};

class CBitDepth2Decoder : public CPngReadSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit);
	};

class CBitDepth4Decoder : public CPngReadSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit);
	};

class CBitDepth8Decoder : public CPngReadSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit);
	};

class CBitDepth8ColorType2Decoder : public CPngReadSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit);
	};

class CBitDepth8ColorType4Decoder : public CPngReadSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit);
	};

class CBitDepth8ColorType6Decoder : public CPngReadSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit);
	};

class CBitDepth16ColorType0Decoder : public CPngReadSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit);
	};

class CBitDepth16ColorType2Decoder : public CPngReadSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit);
	};

class CBitDepth16ColorType4Decoder : public CPngReadSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit);
	};

class CBitDepth16ColorType6Decoder : public CPngReadSubCodec
	{
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength);
	virtual void DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit);
	};

//
// CPngReadSubCodec: base class for classes that read scanlines
//

// Factory function
CPngReadSubCodec* CPngReadSubCodec::NewL(CImageProcessor* aImageProc,CImageProcessor* aMaskProc,const TPngImageInformation& aInfo)
	{
	CPngReadSubCodec* self = NULL;

	switch (aInfo.iBitDepth)
		{
	case 1:
		self = new(ELeave) CBitDepth1Decoder;
		break;
	case 2:
		self = new(ELeave) CBitDepth2Decoder;
		break;
	case 4:
		self = new(ELeave) CBitDepth4Decoder;
		break;
	case 8:
		switch (aInfo.iColorType)
			{
		case TPngImageInformation::EGrayscale:
		case TPngImageInformation::EIndexedColor:
			self = new(ELeave) CBitDepth8Decoder;
			break;
		case TPngImageInformation::EDirectColor:
			self = new(ELeave) CBitDepth8ColorType2Decoder;
			break;
		case TPngImageInformation::EAlphaGrayscale:
			self = new(ELeave) CBitDepth8ColorType4Decoder;
			break;
		case TPngImageInformation::EAlphaDirectColor:
			self = new(ELeave) CBitDepth8ColorType6Decoder;
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
			}
		break;
	case 16:
		switch (aInfo.iColorType)
			{
		case TPngImageInformation::EGrayscale:
			self = new(ELeave) CBitDepth16ColorType0Decoder;
			break;
		case TPngImageInformation::EDirectColor:
			self = new(ELeave) CBitDepth16ColorType2Decoder;
			break;
		case TPngImageInformation::EAlphaGrayscale:
			self = new(ELeave) CBitDepth16ColorType4Decoder;
			break;
		case TPngImageInformation::EAlphaDirectColor:
			self = new(ELeave) CBitDepth16ColorType6Decoder;
			break;
		case TPngImageInformation::EIndexedColor:
		default:
			User::Leave(KErrNotSupported);
			break;
			}
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}

	CleanupStack::PushL(self);
	self->ConstructL(aImageProc,aMaskProc,aInfo);
	CleanupStack::Pop(); // self
	return self;
	}

CPngReadSubCodec::CPngReadSubCodec():
	iScanlineDes1(NULL,0),
	iScanlineDes2(NULL,0)
	{}

CPngReadSubCodec::~CPngReadSubCodec()
	{
	delete iScanlineBuffer1;
	delete iScanlineBuffer2;
	}

void CPngReadSubCodec::ConstructL(CImageProcessor* aImageProc,CImageProcessor* aMaskProc,const TPngImageInformation& aInfo)
	{
	iImageProc = aImageProc;
	iMaskProc = aMaskProc;
	iInfo = aInfo;
	iScanlineBufferSize = ScanlineBufferSize(iInfo.iSize.iWidth);

	DoConstructL();

	iScanlineBuffer1 = HBufC8::NewMaxL(iScanlineBufferSize);
	iScanlineBuffer2 = HBufC8::NewMaxL(iScanlineBufferSize);

	if (iInfo.iInterlaceMethod == TPngImageInformation::EAdam7Interlace)
		{
		iInterlacedScanlineBufferSize[0] = ScanlineBufferSize((iInfo.iSize.iWidth + 7) >> 3);
		iInterlacedScanlineBufferSize[1] = ScanlineBufferSize((iInfo.iSize.iWidth + 3) >> 3);
		iInterlacedScanlineBufferSize[2] = ScanlineBufferSize((iInfo.iSize.iWidth + 3) >> 2);
		iInterlacedScanlineBufferSize[3] = ScanlineBufferSize((iInfo.iSize.iWidth + 1) >> 2);
		iInterlacedScanlineBufferSize[4] = ScanlineBufferSize((iInfo.iSize.iWidth + 1) >> 1);
		iInterlacedScanlineBufferSize[5] = ScanlineBufferSize(iInfo.iSize.iWidth >> 1);
		iInterlacedScanlineBufferSize[6] = iScanlineBufferSize;
		iInterlacedScanlineBufferSize[7] = 0;
		iPass = 0;

		iScanlineDes1.Set(&(iScanlineBuffer1->Des())[0],iInterlacedScanlineBufferSize[0],iInterlacedScanlineBufferSize[0]);
		iScanlineDes2.Set(&(iScanlineBuffer2->Des())[0],iInterlacedScanlineBufferSize[0],iInterlacedScanlineBufferSize[0]);
		}
	else
		{
		iScanlineDes1.Set(&(iScanlineBuffer1->Des())[0],iScanlineBufferSize,iScanlineBufferSize);
		iScanlineDes2.Set(&(iScanlineBuffer2->Des())[0],iScanlineBufferSize,iScanlineBufferSize);
		}
	}

TDes8& CPngReadSubCodec::FirstBuffer()
	{
	iScanlineDes1.FillZ();
	iCurrentScanlineBuffer = 2;
	return iScanlineDes2;
	}

TDes8& CPngReadSubCodec::DecodeL()
	{
	TUint8* dataPtr = (iCurrentScanlineBuffer == 1) ? &iScanlineDes1[1] : &iScanlineDes2[1];
	const TUint8* dataPtrLimit = dataPtr + iScanlineDes1.Length() - 1;

	FilterScanlineDataL(dataPtr,dataPtrLimit);
	DoDecode(dataPtr,dataPtrLimit);
	UpdatePos();

	if (iCurrentScanlineBuffer == 1)
		{
		iCurrentScanlineBuffer = 2;
		return iScanlineDes2;
		}
	else
		{
		iCurrentScanlineBuffer = 1;
		return iScanlineDes1;
		}
	}

void CPngReadSubCodec::FilterScanlineDataL(TUint8* aDataPtr,const TUint8* aDataPtrLimit)
	{
	TInt filterType = (iCurrentScanlineBuffer == 1) ? iScanlineDes1[0] : iScanlineDes2[0];

	switch (filterType)
		{
	case 0: // None
		break;
	case 1: // Sub
		{
		aDataPtr += iBytesPerPixel;

		while (aDataPtr < aDataPtrLimit)
			{
			aDataPtr[0] = TUint8(aDataPtr[0] + aDataPtr[-iBytesPerPixel]);
			aDataPtr++;
			}
		}
		break;
	case 2: // Up
		{
		TUint8* altDataPtr = (iCurrentScanlineBuffer == 1) ? &iScanlineDes2[1] : &iScanlineDes1[1];

		while (aDataPtr < aDataPtrLimit)
			*aDataPtr++ = TUint8(*aDataPtr + *altDataPtr++);
		}
		break;
	case 3: // Average
		{
		const TUint8* tempDataPtrLimit = Min<const TUint8*>(aDataPtr + iBytesPerPixel,aDataPtrLimit);
		TUint8* altDataPtr = (iCurrentScanlineBuffer == 1) ? &iScanlineDes2[1] : &iScanlineDes1[1];

		while (aDataPtr < tempDataPtrLimit)
			{
			aDataPtr[0] = TUint8(aDataPtr[0] + (altDataPtr[0] / 2));
			aDataPtr++;
			altDataPtr++;
			}

		while (aDataPtr < aDataPtrLimit)
			{
			aDataPtr[0] = TUint8(aDataPtr[0] + ((altDataPtr[0] + aDataPtr[-iBytesPerPixel]) / 2));
			aDataPtr++;
			altDataPtr++;
			}
		}
		break;
	case 4: // Paeth
		{
		const TUint8* tempDataPtrLimit = Min<const TUint8*>(aDataPtr + iBytesPerPixel,aDataPtrLimit);
		TUint8* altDataPtr = (iCurrentScanlineBuffer == 1) ? &iScanlineDes2[1] : &iScanlineDes1[1];

		while (aDataPtr < tempDataPtrLimit)
			{
			aDataPtr[0] = TUint8(aDataPtr[0] + altDataPtr[0]);
			aDataPtr++;
			altDataPtr++;
			}

		while (aDataPtr < aDataPtrLimit)
			{
			aDataPtr[0] = TUint8(aDataPtr[0] + PaethPredictor(aDataPtr[-iBytesPerPixel],altDataPtr[0],altDataPtr[-iBytesPerPixel]));
			aDataPtr++;
			altDataPtr++;
			}
		}
		break;
	default: // Error
		User::Leave(KErrCorrupt);
		break;
		}
	}

TInt CPngReadSubCodec::PaethPredictor(TInt aLeft,TInt aAbove,TInt aAboveLeft)
	{
	TInt p = aLeft + aAbove - aAboveLeft;
	TInt pa = Abs(p - aLeft);
	TInt pb = Abs(p - aAbove);
	TInt pc = Abs(p - aAboveLeft);

	if (pa <= pb && pa <= pc)
		return aLeft;
	else if (pb <= pc)
		return aAbove;
	else
		return aAboveLeft;
	}

void CPngReadSubCodec::WritePixel(TRgb aPixelColor)
	{
	if (iInfo.iInterlaceMethod == TPngImageInformation::EAdam7Interlace)
		{
		const TInt width = Min(KBlockWidth[iPass],iInfo.iSize.iWidth - iPos.iX);
		const TInt endY = Min(iPos.iY + KBlockHeight[iPass],iInfo.iSize.iHeight);

		TPoint pos(iPos);

		while (pos.iY < endY)
			{
			iImageProc->SetPos(pos);

			for (TInt col = 0; col < width; col++)
				iImageProc->SetPixel(aPixelColor);

			pos.iY++;
			}

		iPos.iX += KColIncrement[iPass];
		}
	else
		iImageProc->SetPixel(aPixelColor);
	}

void CPngReadSubCodec::WritePixel(TRgb aPixelColor,TUint8 aAlphaValue)
	{
	ASSERT(iMaskProc);

	TRgb maskColor(TRgb::Gray256(aAlphaValue));

	if (iInfo.iInterlaceMethod == TPngImageInformation::EAdam7Interlace)
		{
		iImageProc->SetPos(iPos);
		iMaskProc->SetPos(iPos);
		iImageProc->SetPixel(aPixelColor);
		iMaskProc->SetPixel(maskColor);

		iPos.iX += KColIncrement[iPass];
		}
	else
		{
		iImageProc->SetPixel(aPixelColor);
		iMaskProc->SetPixel(maskColor);
		}
	}

void CPngReadSubCodec::UpdatePos()
	{
	if (iInfo.iInterlaceMethod == TPngImageInformation::EAdam7Interlace)
		{
		ASSERT(iPass <= 7);

		iPos.iX = KColStart[iPass];
		iPos.iY += KRowIncrement[iPass];

		while (iPos.iX >= iInfo.iSize.iWidth || iPos.iY >= iInfo.iSize.iHeight)
			{
			iPass++;
			iPos.iX = KColStart[iPass];
			iPos.iY = KRowStart[iPass];
			iScanlineDes1.Set(&(iScanlineBuffer1->Des())[0],iInterlacedScanlineBufferSize[iPass],iInterlacedScanlineBufferSize[iPass]);
			iScanlineDes2.Set(&(iScanlineBuffer2->Des())[0],iInterlacedScanlineBufferSize[iPass],iInterlacedScanlineBufferSize[iPass]);
			iScanlineDes1.FillZ();
			iScanlineDes2.FillZ();
			}
		}
	}

//
// CBitDepth1Decoder: specialised reader for 1bpp scanlines
//
void CBitDepth1Decoder::DoConstructL()
	{
	if (!(iInfo.iColorType & TPngImageInformation::EPaletteUsed))
		{ // Set up palette to be grayscale values
		iInfo.iPalette[0] = KRgbBlack;
		iInfo.iPalette[1] = KRgbWhite;


		if (iInfo.iTransparencyPresent)
			{
			if (iInfo.iTransparentGray <= 1)
				iInfo.iTransparencyValue[iInfo.iTransparentGray] = 0;
			}
		}

	// Replicate values to avoid shifts when decoding
	iInfo.iPalette[2] = iInfo.iPalette[1];
	iInfo.iPalette[4] = iInfo.iPalette[1];
	iInfo.iPalette[8] = iInfo.iPalette[1];
	iInfo.iPalette[16] = iInfo.iPalette[1];
	iInfo.iPalette[32] = iInfo.iPalette[1];
	iInfo.iPalette[64] = iInfo.iPalette[1];
	iInfo.iPalette[128] = iInfo.iPalette[1];

	if (iInfo.iTransparencyPresent && iInfo.iTransparencyValue[1] != 255)
		{
		iInfo.iTransparencyValue[2] = iInfo.iTransparencyValue[1];
		iInfo.iTransparencyValue[4] = iInfo.iTransparencyValue[1];
		iInfo.iTransparencyValue[8] = iInfo.iTransparencyValue[1];
		iInfo.iTransparencyValue[16] = iInfo.iTransparencyValue[1];
		iInfo.iTransparencyValue[32] = iInfo.iTransparencyValue[1];
		iInfo.iTransparencyValue[64] = iInfo.iTransparencyValue[1];
		iInfo.iTransparencyValue[128] = iInfo.iTransparencyValue[1];
		}

	iBytesPerPixel = 1;
	if (iInfo.iInterlaceMethod == TPngImageInformation::ENoInterlace)
		{
		TInt pixelPadding = ((iInfo.iSize.iWidth + 7) & ~7) - iInfo.iSize.iWidth;
		iImageProc->SetPixelPadding(pixelPadding);
		if (iMaskProc)
			iMaskProc->SetPixelPadding(pixelPadding);
		}
	}

TInt CBitDepth1Decoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return ((aPixelLength + 7) / 8) + KPngScanlineFilterTypeLength;
	}

void CBitDepth1Decoder::DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit)
	{
	if (iMaskProc && iInfo.iTransparencyPresent)
		{
		while (aDataPtr < aDataPtrLimit)
			{
			TInt dataValue = *aDataPtr++;

			for (TUint mask=0x80; mask!=0; mask>>=1) // iterate with 0x80, 0x40 .. 0x01
				WritePixel(iInfo.iPalette[dataValue & mask],iInfo.iTransparencyValue[dataValue & mask]);
			}
		}
	else
		{
		while (aDataPtr < aDataPtrLimit)
			{
			TInt dataValue = *aDataPtr++;

			for (TUint mask=0x80; mask!=0; mask>>=1) // iterate with 0x80, 0x40 .. 0x01
				WritePixel(iInfo.iPalette[dataValue & mask]);
			}
		}
	}


//
// CBitDepth2Decoder: specialised reader for 2bpp scanlines
//
void CBitDepth2Decoder::DoConstructL()
	{
	if (!(iInfo.iColorType & TPngImageInformation::EPaletteUsed))
		{ // Set up palette to be grayscale values
		iInfo.iPalette[0] = KRgbBlack;
		iInfo.iPalette[1] = KRgbDarkGray;
		iInfo.iPalette[2] = KRgbGray;
		iInfo.iPalette[3] = KRgbWhite;

		if (iInfo.iTransparencyPresent)
			{
			if (iInfo.iTransparentGray <= 3)
				iInfo.iTransparencyValue[iInfo.iTransparentGray] = 0;
			}
		}

	// Replicate values to avoid shifts when decoding
	iInfo.iPalette[4] = iInfo.iPalette[1];
	iInfo.iPalette[8] = iInfo.iPalette[2];
	iInfo.iPalette[12] = iInfo.iPalette[3];

	iInfo.iPalette[16] = iInfo.iPalette[1];
	iInfo.iPalette[32] = iInfo.iPalette[2];
	iInfo.iPalette[48] = iInfo.iPalette[3];

	iInfo.iPalette[64] = iInfo.iPalette[1];
	iInfo.iPalette[128] = iInfo.iPalette[2];
	iInfo.iPalette[192] = iInfo.iPalette[3];

	if (iInfo.iTransparencyPresent)
		{
		iInfo.iTransparencyValue[4] = iInfo.iTransparencyValue[1];
		iInfo.iTransparencyValue[8] = iInfo.iTransparencyValue[2];
		iInfo.iTransparencyValue[12] = iInfo.iTransparencyValue[3];

		iInfo.iTransparencyValue[16] = iInfo.iTransparencyValue[1];
		iInfo.iTransparencyValue[32] = iInfo.iTransparencyValue[2];
		iInfo.iTransparencyValue[48] = iInfo.iTransparencyValue[3];

		iInfo.iTransparencyValue[64] = iInfo.iTransparencyValue[1];
		iInfo.iTransparencyValue[128] = iInfo.iTransparencyValue[2];
		iInfo.iTransparencyValue[192] = iInfo.iTransparencyValue[3];
		}

	iBytesPerPixel = 1;
	if (iInfo.iInterlaceMethod == TPngImageInformation::ENoInterlace)
		{
		TInt pixelPadding = ((iInfo.iSize.iWidth + 3) & ~3) - iInfo.iSize.iWidth;
		iImageProc->SetPixelPadding(pixelPadding);
		if (iMaskProc)
			iMaskProc->SetPixelPadding(pixelPadding);
		}
	}

TInt CBitDepth2Decoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return ((aPixelLength + 3) / 4) + KPngScanlineFilterTypeLength;
	}

void CBitDepth2Decoder::DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit)
	{
	if (iMaskProc && iInfo.iTransparencyPresent)
		{
		while (aDataPtr < aDataPtrLimit)
			{
			TInt dataValue = *aDataPtr++;

			for (TInt mask=0xc0; mask!=0; mask>>=2) // iterate through 0xc0, 0x30, 0x0c and 0x03
				WritePixel(iInfo.iPalette[dataValue & mask],iInfo.iTransparencyValue[dataValue & mask]);
			}
		}
	else
		{
		while (aDataPtr < aDataPtrLimit)
			{
			TInt dataValue = *aDataPtr++;

			for (TInt mask=0xc0; mask!=0; mask>>=2) // iterate through 0xc0, 0x30, 0x0c and 0x03
				WritePixel(iInfo.iPalette[dataValue & mask]);
			}
		}
	}


//
// CBitDepth4Decoder: specialised reader for 4bpp scanlines
//

void CBitDepth4Decoder::DoConstructL()
	{
	if (!(iInfo.iColorType & TPngImageInformation::EPaletteUsed))
		{ // Set up palette to be grayscale values
		for (TInt index = 0; index < 16; index++)
			iInfo.iPalette[index] = TRgb::Gray16(index);

		if (iInfo.iTransparencyPresent)
			{
			if (iInfo.iTransparentGray <= 15)
				iInfo.iTransparencyValue[iInfo.iTransparentGray] = 0;
			}
		}

	iBytesPerPixel = 1;
	if (iInfo.iInterlaceMethod == TPngImageInformation::ENoInterlace)
		{
		TInt pixelPadding = ((iInfo.iSize.iWidth + 1) & ~1) - iInfo.iSize.iWidth;
		iImageProc->SetPixelPadding(pixelPadding);
		if (iMaskProc)
			iMaskProc->SetPixelPadding(pixelPadding);
		}
	}

TInt CBitDepth4Decoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return ((aPixelLength + 1) / 2) + KPngScanlineFilterTypeLength;
	}

void CBitDepth4Decoder::DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit)
	{
	if (iMaskProc && iInfo.iTransparencyPresent)
		{
		while (aDataPtr < aDataPtrLimit)
			{
			TInt dataValue = *aDataPtr++;

			WritePixel(iInfo.iPalette[dataValue >> 4],iInfo.iTransparencyValue[dataValue >> 4]);
			WritePixel(iInfo.iPalette[dataValue & 0x0f],iInfo.iTransparencyValue[dataValue & 0x0f]);
			}
		}
	else
		{
		while (aDataPtr < aDataPtrLimit)
			{
			TInt dataValue = *aDataPtr++;

			WritePixel(iInfo.iPalette[dataValue >> 4]);
			WritePixel(iInfo.iPalette[dataValue & 0x0f]);
			}
		}
	}


//
// CBitDepth8Decoder: specialised reader for 8bpp scanlines
//

void CBitDepth8Decoder::DoConstructL()
	{
	if (!(iInfo.iColorType & TPngImageInformation::EPaletteUsed))
		{ // Set up palette to be grayscale values
		for (TInt index = 0; index < 256; index++)
			iInfo.iPalette[index] = TRgb::Gray256(index);

		if (iInfo.iTransparencyPresent)
			{
			if (iInfo.iTransparentGray <= 255)
				iInfo.iTransparencyValue[iInfo.iTransparentGray] = 0;
			}
		}

	iBytesPerPixel = 1;
	}

TInt CBitDepth8Decoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return aPixelLength + KPngScanlineFilterTypeLength;
	}

void CBitDepth8Decoder::DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit)
	{
	if (iMaskProc && iInfo.iTransparencyPresent)
		{
		while (aDataPtr < aDataPtrLimit)
			{
			WritePixel(iInfo.iPalette[aDataPtr[0]],iInfo.iTransparencyValue[aDataPtr[0]]);
			aDataPtr++;
			}
		}
	else
		{
		while (aDataPtr < aDataPtrLimit)
			WritePixel(iInfo.iPalette[*aDataPtr++]);
		}
	}


//
// CBitDepth8ColorType2Decoder: specialised reader for 8bpp scanlines, direct colour
//

void CBitDepth8ColorType2Decoder::DoConstructL()
	{
	iBytesPerPixel = 3;
	}

TInt CBitDepth8ColorType2Decoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return (aPixelLength * 3) + KPngScanlineFilterTypeLength;
	}

void CBitDepth8ColorType2Decoder::DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit)
	{
	if (iMaskProc && iInfo.iTransparencyPresent)
		{
		while (aDataPtr < aDataPtrLimit)
			{
			TInt red = aDataPtr[0];
			TInt green = aDataPtr[1];
			TInt blue = aDataPtr[2];
			TRgb pixelColor(red,green,blue);
			if (red == iInfo.iTransparentRed && green == iInfo.iTransparentGreen && blue == iInfo.iTransparentBlue)
				WritePixel(pixelColor,0);
			else
				WritePixel(pixelColor,255);
			aDataPtr += 3;
			}
		}
	else
		{
		while (aDataPtr < aDataPtrLimit)
			{
			WritePixel(TRgb(aDataPtr[0],aDataPtr[1],aDataPtr[2]));
			aDataPtr += 3;
			}
		}
	}


//
// CBitDepth8ColorType4Decoder: specialised reader for 8bpp scanlines, alpha greyscale
//

void CBitDepth8ColorType4Decoder::DoConstructL()
	{
	iBytesPerPixel = 2;
	}

TInt CBitDepth8ColorType4Decoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return (aPixelLength * 2) + KPngScanlineFilterTypeLength;
	}

void CBitDepth8ColorType4Decoder::DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit)
	{
	if (iMaskProc)
		{
		while (aDataPtr < aDataPtrLimit)
			{
			WritePixel(TRgb::Gray256(aDataPtr[0]),aDataPtr[1]);
			aDataPtr += 2;
			}
		}
	else
		{
		while (aDataPtr < aDataPtrLimit)
			{
			WritePixel(TRgb::Gray256(aDataPtr[0]));
			aDataPtr += 2;
			}
		}
	}


//
// CBitDepth8ColorType4Decoder: specialised reader for 8bpp scanlines, alpha colour
//

void CBitDepth8ColorType6Decoder::DoConstructL()
	{
	iBytesPerPixel = 4;
	}

TInt CBitDepth8ColorType6Decoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return (aPixelLength * 4) + KPngScanlineFilterTypeLength;
	}

void CBitDepth8ColorType6Decoder::DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit)
	{
	if (iMaskProc)
		{
		while (aDataPtr < aDataPtrLimit)
			{
			WritePixel(TRgb(aDataPtr[0],aDataPtr[1],aDataPtr[2]),aDataPtr[3]);
			aDataPtr += 4;
			}
		}
	else
		{
		while (aDataPtr < aDataPtrLimit)
			{
			WritePixel(TRgb(aDataPtr[0],aDataPtr[1],aDataPtr[2]));
			aDataPtr += 4;
			}
		}
	}


//
// CBitDepth16ColorType0Decoder: specialised reader for 16bpp scanlines, greyscale
//

void CBitDepth16ColorType0Decoder::DoConstructL()
	{
	iBytesPerPixel = 2;
	}

TInt CBitDepth16ColorType0Decoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return (aPixelLength * 2) + KPngScanlineFilterTypeLength;
	}

void CBitDepth16ColorType0Decoder::DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit)
	{
	if (iMaskProc && iInfo.iTransparencyPresent)
		{
		while (aDataPtr < aDataPtrLimit)
			{
			TInt gray = (aDataPtr[0] << 8) | aDataPtr[1];
			TRgb pixelColor(TRgb::Gray256(aDataPtr[0]));
			if (gray == iInfo.iTransparentGray)
				WritePixel(pixelColor,0);
			else
				WritePixel(pixelColor,255);
			aDataPtr += 2;
			}
		}
	else
		{
		while (aDataPtr < aDataPtrLimit)
			{
			WritePixel(TRgb::Gray256(aDataPtr[0]));
			aDataPtr += 2;
			}
		}
	}


//
// CBitDepth16ColorType2Decoder: specialised reader for 16bpp scanlines, RGB colour
//

void CBitDepth16ColorType2Decoder::DoConstructL()
	{
	iBytesPerPixel = 6;
	}

TInt CBitDepth16ColorType2Decoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return (aPixelLength * 6) + KPngScanlineFilterTypeLength;
	}

void CBitDepth16ColorType2Decoder::DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit)
	{
	if (iMaskProc && iInfo.iTransparencyPresent)
		{
		while (aDataPtr < aDataPtrLimit)
			{
			TInt red = (aDataPtr[0] << 8) | aDataPtr[1];
			TInt green = (aDataPtr[2] << 8) | aDataPtr[3];
			TInt blue = (aDataPtr[4] << 8) | aDataPtr[5];
			TRgb pixelColor(aDataPtr[0],aDataPtr[2],aDataPtr[4]);
			if (red == iInfo.iTransparentRed && green == iInfo.iTransparentGreen && blue == iInfo.iTransparentBlue)
				WritePixel(pixelColor,0);
			else
				WritePixel(pixelColor,255);
			aDataPtr += 6;
			}
		}
	else
		{
		while (aDataPtr < aDataPtrLimit)
			{
			WritePixel(TRgb(aDataPtr[0],aDataPtr[2],aDataPtr[4]));
			aDataPtr += 6;
			}
		}
	}


//
// CBitDepth16ColorType4Decoder: specialised reader for 16bpp scanlines, alpha greyscale
//

void CBitDepth16ColorType4Decoder::DoConstructL()
	{
	iBytesPerPixel = 4;
	}

TInt CBitDepth16ColorType4Decoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return (aPixelLength * 4) + KPngScanlineFilterTypeLength;
	}

void CBitDepth16ColorType4Decoder::DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit)
	{
	if (iMaskProc)
		{
		while (aDataPtr < aDataPtrLimit)
			{
			WritePixel(TRgb::Gray256(aDataPtr[0]),aDataPtr[2]);
			aDataPtr += 4;
			}
		}
	else
		{
		while (aDataPtr < aDataPtrLimit)
			{
			WritePixel(TRgb::Gray256(aDataPtr[0]));
			aDataPtr += 4;
			}
		}
	}


//
// CBitDepth16ColorType6Decoder: specialised reader for 16bpp scanlines, alpha colour
//

void CBitDepth16ColorType6Decoder::DoConstructL()
	{
	iBytesPerPixel = 8;
	}

TInt CBitDepth16ColorType6Decoder::ScanlineBufferSize(TInt aPixelLength)
	{
	return (aPixelLength * 8) + KPngScanlineFilterTypeLength;
	}

void CBitDepth16ColorType6Decoder::DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit)
	{
	if (iMaskProc)
		{
		while (aDataPtr < aDataPtrLimit)
			{
			WritePixel(TRgb(aDataPtr[0],aDataPtr[2],aDataPtr[4]),aDataPtr[6]);
			aDataPtr += 8;
			}
		}
	else
		{
		while (aDataPtr < aDataPtrLimit)
			{
			WritePixel(TRgb(aDataPtr[0],aDataPtr[2],aDataPtr[4]));
			aDataPtr += 8;
			}
		}
	}

