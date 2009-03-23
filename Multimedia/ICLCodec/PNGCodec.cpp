// PngCodec.CPP
//
// Copyright (c) 1999-2005 Symbian Software Ltd.  All rights reserved.
//

#include <fbs.h>

#include "ImageUtils.h"
#include "PngCodec.h"

// Constants.
const TInt KTwipsPerMeter = 56693;

// 
// TPngImageInformation: PNG image information
//

// Initialise default PNG image information
TPngImageInformation::TPngImageInformation()
	{
	iSize.SetSize(0,0);
	iBitDepth = 0;
	iColorType = EGrayscale;
	iCompressionMethod = EDeflateInflate32K;
	iFilterMethod = EAdaptiveFiltering;
	iInterlaceMethod = ENoInterlace;
	iPalettePresent = EFalse;

#if defined(_DEBUG)
	// as an optimisation, we are going to set iPalette to all zeros, instead of setting
	// each element to KRgbBlack. This assumes that KRbgBlack is itself zero.
	ASSERT(sizeof(TRgb)==sizeof(TUint32)); // ie no new fields
	ASSERT(KRgbBlack.Value()==0); // ie the one value is zero
#endif // defined(_DEBUG)

	Mem::FillZ(iPalette, KPngMaxPLTESize*sizeof(TRgb));

	iBackgroundPresent = EFalse;
	iBackgroundColor = KRgbWhite;
	iPhysicalPresent = EFalse;
	iPhysicalUnits = EUnknownUnits;
	iPhysicalSize.SetSize(0,0);
	iTransparencyPresent = EFalse;
	Mem::Fill(iTransparencyValue,KPngMaxPLTESize,0xff);
	}


//
// CPngReadCodec: reads a PNG image
//

CPngReadCodec::~CPngReadCodec()
	{
	delete iDecoder;
	delete iDecompressor;
	}

// Called by framework when a Convert operation begins
void CPngReadCodec::InitFrameL(TFrameInfo& /*aFrameInfo*/, CFrameImageData& /*aFrameImageData*/, 
	TBool aDisableErrorDiffusion, CFbsBitmap& aDestination, CFbsBitmap* aDestinationMask)
	{
	CFbsBitmap& newFrame = aDestination;

	TPoint& pos = Pos();
	pos.SetXY(0,0);
	iChunkBytesRemaining = 0;

	// Use the supplied image processor
	CImageProcessor* imageProc = ImageProcessorUtility::NewImageProcessorL(newFrame, iImageInfo.iSize, ERgb, aDisableErrorDiffusion);
	SetImageProcessor(imageProc);
	imageProc->PrepareL(newFrame,iImageInfo.iSize);

	CImageProcessor* maskProc = NULL;
	SetMaskProcessor(NULL);

	// If transparency is being used, create a bitmap mask as well
	if ((iImageInfo.iTransparencyPresent || (iImageInfo.iColorType & TPngImageInformation::EAlphaChannelUsed))
		&& aDestinationMask)
		{
		maskProc = ImageProcessorUtility::NewImageProcessorL(*aDestinationMask, iImageInfo.iSize, ERgb, aDisableErrorDiffusion);
		SetMaskProcessor(maskProc);
		maskProc->PrepareL(*aDestinationMask,iImageInfo.iSize);
		// set mask to black so that unknown parts on streamed image are not drawn
		ClearBitmapL(*aDestinationMask, KRgbBlack);
		}

	// Create a helper to read the scan lines
	delete iDecoder;
	iDecoder = NULL;
	iDecoder = CPngReadSubCodec::NewL(imageProc,maskProc,iImageInfo);

	// And a unzipper to decompress image data
	if (!iDecompressor)
		iDecompressor = CEZDecompressor::NewL(*this);
	else
		iDecompressor->ResetL(*this);

	if (maskProc==NULL)
		{
		// if no mask, clear destination for sensible behaviour on streamed partial images
		TRgb background = iImageInfo.iBackgroundPresent ? iImageInfo.iBackgroundColor : KRgbWhite;
		ClearBitmapL(aDestination, background);
		}
	}

// Called by framework to initialise image frame header
void CPngReadCodec::InitFrameHeader(TFrameInfo& aFrameSettings, CFrameImageData& /* aFrameImageData */)
	{
	ASSERT(aFrameSettings.CurrentFrameState() == TFrameInfo::EFrameInfoUninitialised);
	iFrameInfo = &aFrameSettings;
	iFrameInfo->SetCurrentFrameState(TFrameInfo::EFrameInfoProcessingFrameHeader);
	}

// Called by framework to process a header for a frame
TFrameState CPngReadCodec::ProcessFrameHeaderL(TBufPtr8& aData)
	{
	const TUint8* startDataPtr = aData.Ptr();
	const TUint8* dataPtr = startDataPtr;
	const TUint8* dataPtrLimit = startDataPtr + aData.Length();

	// Process the mandatory PNG header chunk: sets up iImageInfo
	if (iFrameInfo->CurrentFrameState() == TFrameInfo::EFrameInfoProcessingFrameHeader)
		{
		if (dataPtr + KPngChunkLengthSize + KPngChunkIdSize + KPngIHDRChunkSize + KPngChunkCRCSize > dataPtrLimit)
			User::Leave(KErrUnderflow);

		TInt chunkLength = PtrReadUtil::ReadBigEndianUint32Inc(dataPtr);
		TPtrC8 chunkId(dataPtr,KPngChunkIdSize);

		if (chunkLength != KPngIHDRChunkSize || chunkId != KPngIHDRChunkId)
			User::Leave(KErrNotFound);

		dataPtr += KPngChunkIdSize;

		DoProcessIHDRL(dataPtr,chunkLength);

		dataPtr += KPngIHDRChunkSize + KPngChunkCRCSize;
		}

	// Process any optional PNG header chunks
	TRAPD(err, DoProcessInfoL(dataPtr, dataPtrLimit));
	if (err != KErrNone)
		{
		if (err == KErrNotFound)
			return EFrameComplete;

		User::Leave(err); // A real error occured
		}

	// Having read the header, can initialise the frame information
	aData.Shift(dataPtr - startDataPtr);

	iFrameInfo->iFrameCoordsInPixels.SetRect(TPoint(0,0),iImageInfo.iSize);
	iFrameInfo->iOverallSizeInPixels = iImageInfo.iSize;
	if (iImageInfo.iPhysicalPresent && iImageInfo.iPhysicalUnits == TPngImageInformation::EMeters)
		iFrameInfo->iFrameSizeInTwips = iImageInfo.iPhysicalSize;
	else
		iFrameInfo->iFrameSizeInTwips.SetSize(0,0);

	iFrameInfo->iBitsPerPixel = iImageInfo.iBitDepth;
	if (iImageInfo.iColorType & TPngImageInformation::EColorUsed)
		iFrameInfo->iBitsPerPixel *= 3;
	
	iFrameInfo->iDelay = 0;
	iFrameInfo->iFlags = TFrameInfo::ECanDither;
	
	if (iImageInfo.iColorType & (TPngImageInformation::EPaletteUsed | TPngImageInformation::EColorUsed))
		iFrameInfo->iFlags |= TFrameInfo::EColor;
	
	if (iImageInfo.iColorType & TPngImageInformation::EAlphaChannelUsed)
		{
		iFrameInfo->iFlags |= TFrameInfo::ETransparencyPossible;
		iFrameInfo->iFlags |= TFrameInfo::EAlphaChannel;
		}
	else if (iImageInfo.iTransparencyPresent)
		iFrameInfo->iFlags |= TFrameInfo::ETransparencyPossible;

	switch (iFrameInfo->iBitsPerPixel)
		{
	case 1:
		iFrameInfo->iFrameDisplayMode = EGray2;
		break;

	case 2:
		iFrameInfo->iFrameDisplayMode = EGray4;
		break;

	case 4:
		iFrameInfo->iFrameDisplayMode = (iFrameInfo->iFlags & TFrameInfo::EColor) ? EColor16 : EGray16;
		break;

	case 8:
		iFrameInfo->iFrameDisplayMode = (iFrameInfo->iFlags & TFrameInfo::EColor) ? EColor256 : EGray256;
		break;

	case 12:
		iFrameInfo->iFrameDisplayMode = EColor4K;
		break;

	case 16:
		iFrameInfo->iFrameDisplayMode = EColor64K;
		break;

	case 24:
		iFrameInfo->iFrameDisplayMode = EColor16M;
		break;

	default:
		User::Leave(KErrCorrupt);
		}

	if (iImageInfo.iBackgroundPresent)
		iFrameInfo->iBackgroundColor = iImageInfo.iBackgroundColor;

	iFrameInfo->SetCurrentFrameState(TFrameInfo::EFrameInfoProcessingComplete);
	return EFrameComplete;
	}

// Called by the framework to process frame data
TFrameState CPngReadCodec::ProcessFrameL(TBufPtr8& aSrc)
	{
	CImageProcessor*const imageProc = ImageProcessor();
	CImageProcessor*const maskProc = MaskProcessor();

	TUint8* startDataPtr = const_cast<TUint8*>(aSrc.Ptr());
	TUint8* dataPtr = startDataPtr;
	const TUint8* dataPtrLimit = dataPtr + aSrc.Length();
	while (dataPtr < dataPtrLimit)
		{
		// If at the end of a PNG chunk
		if (iChunkBytesRemaining == 0)
			{
			if (iChunkId == KPngIDATChunkId) // Need to skip IDAT chunk CRCs
				{
				if (dataPtr + KPngChunkCRCSize + KPngChunkLengthSize + KPngChunkIdSize > dataPtrLimit)
					break;

				dataPtr += KPngChunkCRCSize;
				}
			else
				{
				if (dataPtr + KPngChunkLengthSize + KPngChunkIdSize > dataPtrLimit)
					break;
				}

			iChunkBytesRemaining = PtrReadUtil::ReadBigEndianUint32Inc(const_cast<const TUint8*&>(dataPtr));
			iChunkId = TPtr8(dataPtr,KPngChunkIdSize,KPngChunkIdSize);
			dataPtr += KPngChunkIdSize;
			}
		// Process an image data chunk
		if (iChunkId == KPngIDATChunkId)
			DoProcessDataL(const_cast<const TUint8*&>(dataPtr),dataPtrLimit);
		// Process an END chunk -- frame is complete
		else if (iChunkId == KPngIENDChunkId)
			{
			iDecompressor->InflateL();
			imageProc->FlushPixels();
			if (maskProc)
				maskProc->FlushPixels();
			return EFrameComplete;
			}
		else 
		// Skip other chunks
			{
			TInt bytesLeft = dataPtrLimit - dataPtr;
			if (bytesLeft >= iChunkBytesRemaining + KPngChunkCRCSize)
				{
				dataPtr += iChunkBytesRemaining + KPngChunkCRCSize;
				iChunkBytesRemaining = 0;
				}
			else
				{
				dataPtr += bytesLeft;
				iChunkBytesRemaining -= bytesLeft;
				}
			}
		}

	aSrc.Shift(dataPtr - startDataPtr);
	return EFrameIncomplete;
	}

// Process any optional PNG header chunks
void CPngReadCodec::DoProcessInfoL(const TUint8*& aDataPtr,const TUint8* aDataPtrLimit)
	{
	FOREVER
		{
		if (aDataPtr + KPngChunkLengthSize + KPngChunkIdSize > aDataPtrLimit) // Check there is enough data to read the chunk length
			User::Leave(KErrUnderflow);

		TInt chunkLength = PtrReadUtil::ReadBigEndianUint32Inc(aDataPtr);
		TPtrC8 chunkId (&aDataPtr[0],KPngChunkIdSize);

		if (chunkId == KPngIDATChunkId)
			{
			aDataPtr -= KPngChunkLengthSize; // Rewind to start of chunkLength
			break;
			}

		if (aDataPtr + KPngChunkIdSize + chunkLength + KPngChunkCRCSize > aDataPtrLimit) // Check there is enough data to read the whole chunk
			{
			aDataPtr -= KPngChunkLengthSize; // Rewind to start of chunkLength
			User::Leave(KErrUnderflow);
			}

		aDataPtr += KPngChunkIdSize;

		if (chunkId == KPngPLTEChunkId)
			DoProcessPLTEL(aDataPtr,chunkLength);
		else if (chunkId == KPngbKGDChunkId)
			DoProcessbKGDL(aDataPtr,chunkLength);
		else if (chunkId == KPngpHYsChunkId)
			DoProcesspHYsL(aDataPtr,chunkLength);
		else if (chunkId == KPngtRNSChunkId)
			DoProcesstRNSL(aDataPtr,chunkLength);
		else if (chunkId == KPngIHDRChunkId || chunkId == KPngIENDChunkId)
			User::Leave(KErrCorrupt);

		aDataPtr += chunkLength;
		PtrReadUtil::ReadBigEndianUint32Inc(aDataPtr); // Skip crc value
		}
	}

// Process the mandatory PNG header chunk
void CPngReadCodec::DoProcessIHDRL(const TUint8* aDataPtr,TInt aChunkLength)
	{
	if (aChunkLength != KPngIHDRChunkSize)
		User::Leave(KErrCorrupt);

	iImageInfo.iSize.iWidth = PtrReadUtil::ReadBigEndianUint32Inc(aDataPtr);
	iImageInfo.iSize.iHeight = PtrReadUtil::ReadBigEndianUint32Inc(aDataPtr);
	iImageInfo.iBitDepth = aDataPtr[0];
	iImageInfo.iColorType = TPngImageInformation::TColorType(aDataPtr[1]);
	iImageInfo.iCompressionMethod = TPngImageInformation::TCompressionMethod(aDataPtr[2]);
	iImageInfo.iFilterMethod = TPngImageInformation::TFilterMethod(aDataPtr[3]);
	iImageInfo.iInterlaceMethod = TPngImageInformation::TInterlaceMethod(aDataPtr[4]);

	// Check is one of the PNG formats we support
	if (iImageInfo.iSize.iWidth < 1 || iImageInfo.iSize.iHeight < 1
		|| iImageInfo.iCompressionMethod != TPngImageInformation::EDeflateInflate32K
		|| iImageInfo.iFilterMethod != TPngImageInformation::EAdaptiveFiltering
		|| (iImageInfo.iInterlaceMethod != TPngImageInformation::ENoInterlace &&
			iImageInfo.iInterlaceMethod != TPngImageInformation::EAdam7Interlace))
		User::Leave(KErrCorrupt);
	}

// Process a PNG PLTE (palette) chunk
void CPngReadCodec::DoProcessPLTEL(const TUint8* aDataPtr,TInt aChunkLength)
	{
	if (aChunkLength % 3 != 0)
		User::Leave(KErrCorrupt);

	iImageInfo.iPalettePresent = ETrue;

	const TUint8* dataPtrLimit = aDataPtr + aChunkLength;
	TRgb* palettePtr = iImageInfo.iPalette;

	while (aDataPtr < dataPtrLimit)
		{
		*palettePtr++ = TRgb(aDataPtr[0],aDataPtr[1],aDataPtr[2]);
		aDataPtr += 3;
		}
	}

// Process a PNG bKGD (background color) chunk
void CPngReadCodec::DoProcessbKGDL(const TUint8* aDataPtr,TInt aChunkLength)
	{
	iImageInfo.iBackgroundPresent = ETrue;

	if (iImageInfo.iColorType == TPngImageInformation::EIndexedColor) // 3
		{
		if (aChunkLength < 1)
			User::Leave(KErrCorrupt);

		iImageInfo.iBackgroundColor = iImageInfo.iPalette[aDataPtr[0]];
		}
	else if (iImageInfo.iColorType & TPngImageInformation::EMonochrome) // 0 & 4
		{
		if (aChunkLength < 2)
			User::Leave(KErrCorrupt);

		TInt grayLevel = PtrReadUtil::ReadBigEndianInt16(aDataPtr);
		ASSERT(iImageInfo.iBitDepth<8);
		grayLevel <<= (7-iImageInfo.iBitDepth);
		iImageInfo.iBackgroundColor = TRgb::Gray256(grayLevel);
		}
	else if (iImageInfo.iColorType & TPngImageInformation::EColorUsed) // 2 & 6
		{
		if (aChunkLength < 6)
			User::Leave(KErrCorrupt);

		TInt red = PtrReadUtil::ReadBigEndianInt16(&aDataPtr[0]);
		TInt green = PtrReadUtil::ReadBigEndianInt16(&aDataPtr[2]);
		TInt blue = PtrReadUtil::ReadBigEndianInt16(&aDataPtr[4]);
		ASSERT (iImageInfo.iBitDepth<8);
		const TInt offset = 7-iImageInfo.iBitDepth;
		red <<= offset;
		green <<= offset;
		blue <<= offset;
		iImageInfo.iBackgroundColor = TRgb(red,green,blue);
		}
	}

// Process a PNG pHYs (Physical pixel dimensions) chunk
void CPngReadCodec::DoProcesspHYsL(const TUint8* aDataPtr,TInt aChunkLength)
	{
	if (aChunkLength != KPngpHYsChunkSize)
		User::Leave(KErrCorrupt);

	iImageInfo.iPhysicalUnits = TPngImageInformation::TPhysicalUnits(aDataPtr[8]);

	if (iImageInfo.iPhysicalUnits == TPngImageInformation::EMeters)
		{
		iImageInfo.iPhysicalPresent = ETrue;

		TInt horzPixelsPerMeter = PtrReadUtil::ReadBigEndianUint32Inc(aDataPtr);
		TInt vertPixelsPerMeter = PtrReadUtil::ReadBigEndianUint32Inc(aDataPtr);

		if (horzPixelsPerMeter > 0)
			iImageInfo.iPhysicalSize.iWidth = iImageInfo.iSize.iWidth * KTwipsPerMeter / horzPixelsPerMeter;
		if (vertPixelsPerMeter > 0)
			iImageInfo.iPhysicalSize.iHeight = iImageInfo.iSize.iHeight * KTwipsPerMeter / vertPixelsPerMeter;
		}
	}

// Process a PNG tRNS (Transparency) chunk
void CPngReadCodec::DoProcesstRNSL(const TUint8* aDataPtr,TInt aChunkLength)
	{
	iImageInfo.iTransparencyPresent = ETrue;

	if (iImageInfo.iColorType == TPngImageInformation::EIndexedColor) // 3
		{
		if (aChunkLength < 1)
			User::Leave(KErrCorrupt);

		Mem::Copy(iImageInfo.iTransparencyValue,aDataPtr,aChunkLength);
		}
	else if (iImageInfo.iColorType == TPngImageInformation::EGrayscale) // 0
		{
		if (aChunkLength < 2)
			User::Leave(KErrCorrupt);

		iImageInfo.iTransparentGray = TUint16((aDataPtr[0] << 8) | aDataPtr[1]);
		}
	else if (iImageInfo.iColorType == TPngImageInformation::EDirectColor) // 2
		{
		if (aChunkLength < 6)
			User::Leave(KErrCorrupt);

		iImageInfo.iTransparentRed = TUint16((aDataPtr[0] << 8) | aDataPtr[1]);
		iImageInfo.iTransparentGreen = TUint16((aDataPtr[2] << 8) | aDataPtr[3]);
		iImageInfo.iTransparentBlue = TUint16((aDataPtr[4] << 8) | aDataPtr[5]);
		}
	}

// Process a PNG image data
void CPngReadCodec::DoProcessDataL(const TUint8*& aDataPtr,const TUint8* aDataPtrLimit)
	{
	// Data is passed to the decompressor
	TInt bytesToProcess = Min(aDataPtrLimit - aDataPtr,iChunkBytesRemaining);
	iDataDes.Set(aDataPtr,bytesToProcess);
	iDecompressor->SetInput(iDataDes);

	while (iDecompressor->AvailIn() > 0)
		iDecompressor->InflateL();

	aDataPtr += bytesToProcess;
	iChunkBytesRemaining -= bytesToProcess;
	}

// From MEZBufferManager: manage decompressor stream
void CPngReadCodec::InitializeL(CEZZStream& aZStream)
	{
	aZStream.SetOutput(iDecoder->FirstBuffer());
	}

void CPngReadCodec::NeedInputL(CEZZStream& /*aZStream*/)
	{
	}

void CPngReadCodec::NeedOutputL(CEZZStream& aZStream)
	{
	aZStream.SetOutput(iDecoder->DecodeL());
	}

void CPngReadCodec::FinalizeL(CEZZStream& /*aZStream*/)
	{
	iDecoder->DecodeL();
	}



//
// CPngWriteCodec: writes a PNG image
//


CPngWriteCodec::CPngWriteCodec(TInt aBpp, TBool aColor, TBool aPaletted, TInt aCompressionLevel)
	: iCompressionLevel(aCompressionLevel), iCompressorPtr(NULL, 0)
	{
	// Set bpp
	iImageInfo.iBitsPerPixel = aBpp;
	switch (aBpp)
		{
		case 1:
			iImageInfo.iBitDepth = 1;
			break;
		case 2:
			iImageInfo.iBitDepth = 2;
			break;
		case 4:
			iImageInfo.iBitDepth = 4;
			break;
		case 8:
		case 24:
			iImageInfo.iBitDepth = 8;
			break;
		default:
			break;
		}

	// Set color type
	if (aColor && aPaletted)
		iImageInfo.iColorType = TPngImageInformation::EIndexedColor;
	else if (aColor)
		iImageInfo.iColorType = TPngImageInformation::EDirectColor;
	else
		iImageInfo.iColorType = TPngImageInformation::EGrayscale;
	}

CPngWriteCodec::~CPngWriteCodec()
	{
	delete iCompressor;
	delete iEncoder;
	}

// Called by framework at start of conversion operation
void CPngWriteCodec::InitFrameL(TBufPtr8& aDst, const CFbsBitmap& aSource)
	{
	if (aDst.Length() == 0)
		User::Leave(KErrArgument);	// Not enough length for anything

	SetSource(&aSource);
	iDestStartPtr = const_cast<TUint8*>(aDst.Ptr());
	iDestPtr = iDestStartPtr;
	iDestPtrLimit = iDestPtr + aDst.MaxLength();

	// Set image information
	const SEpocBitmapHeader& header = aSource.Header();
	iImageInfo.iSize = header.iSizeInPixels;

	switch (iImageInfo.iBitDepth)
		{
		case 1:
		case 2:
		case 4:
			if (iImageInfo.iColorType == TPngImageInformation::EDirectColor)
				{
				// Bit depths 1, 2 and 4 don't support RGB colour (color mode 2)
				// Must use paletted colour or greyscale
				User::Leave(KErrNotSupported);
				break;
				}
			// fall through to case 8
		case 8:
			break;
		default:
			User::Leave(KErrNotSupported);	// unsupported bit depth
			break;
		}

	iImageInfo.iCompressionMethod = TPngImageInformation::EDeflateInflate32K;
	iImageInfo.iFilterMethod = TPngImageInformation::EAdaptiveFiltering;
	iImageInfo.iInterlaceMethod = TPngImageInformation::ENoInterlace;

	// Create encoder
	if (iEncoder)
		{
		delete iEncoder;
		iEncoder = NULL;
		}
	iEncoder = CPngWriteSubCodec::NewL(iImageInfo, &aSource);

	// Create compressor
	if (iCompressor)
		{
		delete iCompressor;
		iCompressor = NULL;
		}
	iCompressor = CEZCompressor::NewL(*this, iCompressionLevel);

	// Initial encoder state
	if (iImageInfo.iColorType == TPngImageInformation::EIndexedColor)
		iEncoderState = EPngWritePLTE;	
	else
		iEncoderState = EPngDeflate;
	iCallAgain = ETrue;		// to make sure we call DeflateL

	// Write header
	User::LeaveIfError(WriteHeaderChunk(aDst));
	}

// Called by the framework to process frame data
TFrameState CPngWriteCodec::ProcessFrameL(TBufPtr8& aDst)
	{
	if (aDst.Length() == 0)
		User::Leave(KErrArgument);	// Not enough length for anything

	TFrameState state = EFrameIncomplete;
	iDestStartPtr = const_cast<TUint8*>(aDst.Ptr());
	iDestPtr = iDestStartPtr;
	iDestPtrLimit = iDestPtr + aDst.MaxLength();

	// Set return buffer length to 0 initially
	aDst.SetLength(0);

	while (aDst.Length() == 0 && state != EFrameComplete)
		{
		// Loop round until we have some data to return or
		// the image is encoded
		switch (iEncoderState)
			{
			case EPngWritePLTE:
				WritePLTEChunk(aDst);
				break;
			case EPngDeflate:
				DeflateEncodedDataL(aDst, state);
				break;
			case EPngWriteIDAT:
				WriteIDATChunk(aDst);
				break;
			case EPngFlush:
				FlushCompressedDataL(aDst, state);
				break;
			case EPngEndChunk:
				WriteEndChunk(aDst);
				state = EFrameComplete;
				break;
			default:
				break;
			}
		}

	return state;
	}

// Write a compressed image data chunk
void CPngWriteCodec::DeflateEncodedDataL(TBufPtr8& aDst, TFrameState& /*aState*/)
	{
	// Set ptr for compressed data
	const TInt dataLength = aDst.MaxLength() - KPngChunkLengthSize - KPngChunkIdSize - KPngChunkCRCSize;
	ASSERT(dataLength > 0);
	iCompressorPtr.Set(iDestPtr + KPngChunkIdSize + KPngChunkLengthSize, dataLength, dataLength);

	// Initialise input/output for compressor
	iCompressor->SetInput(iEncoder->EncodeL(iScanline));
	iScanline++;
	iCompressor->SetOutput(iCompressorPtr);

	while ((iEncoderState == EPngDeflate) && iCallAgain)
		iCallAgain = iCompressor->DeflateL();

	// Write the IDAT chunk
	WriteIDATChunk(aDst);
	iEncoderState = EPngFlush;
	}

void CPngWriteCodec::FlushCompressedDataL(TBufPtr8& aDst, TFrameState& /*aState*/)
	{
	if (iCallAgain)
		{
		iCallAgain = iCompressor->DeflateL();
		WriteIDATChunk(aDst);
		}
	else
		{
		iEncoderState = EPngEndChunk;
		}
	}

// Write a PLTE chunk
void CPngWriteCodec::WritePLTEChunk(TBufPtr8& aDst)
	{
	ASSERT(iEncoder->Palette() &&
		   (iImageInfo.iColorType == TPngImageInformation::EIndexedColor ||
		    iImageInfo.iColorType == TPngImageInformation::EDirectColor ||
		    iImageInfo.iColorType == TPngImageInformation::EAlphaDirectColor));	// allowed color types for PLTE chunk

	// Get palette entries
	CPalette* palette = iEncoder->Palette();
	ASSERT(palette);
	const TInt count = palette->Entries();
	TUint8* ptr = iDestPtr + KPngChunkIdSize + KPngChunkLengthSize;
	TInt length = count * 3;
	TPtr8 data(ptr, length, length);
	for (TInt i=0; i < count; i++)
		{
		TRgb rgb = palette->GetEntry(i);
		*ptr = TUint8(rgb.Red());
		ptr++;
		*ptr = TUint8(rgb.Green());
		ptr++;
		*ptr = TUint8(rgb.Blue());
		ptr++;
		}
	// Write PLTE chunk
	WritePngChunk(iDestPtr, KPngPLTEChunkId, data, length);
	ASSERT(length % 3 == 0);	// length must be divisible by 3
	aDst.SetLength(length);
	iEncoderState = EPngDeflate;
	}

// Write a data chunk
void CPngWriteCodec::WriteIDATChunk(TBufPtr8& aDst)
	{
	TPtrC8 ptr(iCompressor->OutputDescriptor());
	if (ptr.Length())
		{
		TInt length = 0;
		WritePngChunk(iDestPtr, KPngIDATChunkId, ptr, length);
		aDst.SetLength(length);

		// New output can write to the same compressor ptr
		iCompressor->SetOutput(iCompressorPtr);
		}

	if (iCallAgain)
		iEncoderState = EPngFlush;
	}

// Write an END chunk
void CPngWriteCodec::WriteEndChunk(TBufPtr8& aDst)
	{
	// Write IEND chunk
	TInt length = 0;
	WritePngChunk(iDestPtr, KPngIENDChunkId, KNullDesC8, length);
	aDst.SetLength(length);
	}

// Write a header chunk
TInt CPngWriteCodec::WriteHeaderChunk(TBufPtr8& aDst)
	{
	// Write signature
	Mem::Copy(iDestPtr, &KPngSignature[0], KPngFileSignatureLength);
	iDestPtr += KPngFileSignatureLength;

	// Write IHDR chunk
	TBuf8<KPngIHDRChunkSize> buffer;
	TUint8* ptr = const_cast<TUint8*>(buffer.Ptr());
	// Set length of data
	buffer.SetLength(KPngIHDRChunkSize);
	// Chunk data
	// width (4 bytes)
	if ((iImageInfo.iSize.iWidth == 0) ||
		(static_cast<TUint>(iImageInfo.iSize.iWidth) > KPngMaxImageSize))
		{
		return KErrArgument;	// invalid width
		}
	PtrWriteUtil::WriteBigEndianInt32(ptr, iImageInfo.iSize.iWidth);
	ptr += 4;
	// height (4 bytes)
	if ((iImageInfo.iSize.iHeight == 0) ||
		(static_cast<TUint>(iImageInfo.iSize.iHeight) > KPngMaxImageSize))
		{
		return KErrArgument;	// invalid height
		}
	PtrWriteUtil::WriteBigEndianInt32(ptr, iImageInfo.iSize.iHeight);
	ptr += 4;
	// bit depth (1 byte)
	PtrWriteUtil::WriteInt8(ptr, iImageInfo.iBitDepth);
	ptr++;
	// colour type (1 byte)
	PtrWriteUtil::WriteInt8(ptr, iImageInfo.iColorType);
	ptr++;
	// compression method (1 byte)
	PtrWriteUtil::WriteInt8(ptr, iImageInfo.iCompressionMethod);
	ptr++;
	// filter method (1 byte)
	PtrWriteUtil::WriteInt8(ptr, iImageInfo.iFilterMethod);
	ptr++;
	// interlace method (1 byte)
	PtrWriteUtil::WriteInt8(ptr, iImageInfo.iInterlaceMethod);
	ptr++;

	TInt length = 0;
	WritePngChunk(iDestPtr, KPngIHDRChunkId, buffer, length);
	aDst.SetLength(KPngFileSignatureLength + length);

	return KErrNone;
	}

// Chunk writing helper function
void CPngWriteCodec::WritePngChunk(TUint8*& aDestPtr, const TDesC8& aChunkId, const TDesC8& aData, TInt& aLength)
	{
	// Chunk length (4 bytes)
	PtrWriteUtil::WriteBigEndianInt32(aDestPtr, aData.Length());
	aDestPtr += KPngChunkLengthSize;
	TUint8* crcPtr = aDestPtr;	// start position for calculating CRC
	// Chunk type (4 bytes)
	Mem::Copy(aDestPtr, aChunkId.Ptr(), KPngChunkIdSize);
	aDestPtr += KPngChunkIdSize;
	// Chunk data (0...n bytes)
	Mem::Copy(aDestPtr, aData.Ptr(), aData.Length());
	aDestPtr += aData.Length();
	// CRC (4 bytes)
	TUint32 crc = KPngCrcMask;
	GetCrc(crc, crcPtr, KPngChunkIdSize + aData.Length());
	crc ^= KPngCrcMask;
	PtrWriteUtil::WriteBigEndianInt32(aDestPtr, crc);
	aDestPtr += KPngChunkCRCSize;
	// Length of chunk
	aLength = KPngChunkLengthSize + KPngChunkIdSize + aData.Length() + KPngChunkCRCSize;
	}

// from MEZBufferManager, manage data compressor
void CPngWriteCodec::InitializeL(CEZZStream& /*aZStream*/)
	{
	}

void CPngWriteCodec::NeedInputL(CEZZStream& aZStream)
	{
	// Give compressor more data from encoder
	aZStream.SetInput(iEncoder->EncodeL(iScanline));
	if (iCompressor->AvailIn() != 0)
		iScanline++;
	}

void CPngWriteCodec::NeedOutputL(CEZZStream& /*aZStream*/)
	{
	// Signal to write an IDAT chunk
	iEncoderState = EPngWriteIDAT;
	}

void CPngWriteCodec::FinalizeL(CEZZStream& /*aZStream*/)
	{
	}

// Calculate CRC for PNG chunks
void CPngWriteCodec::GetCrc(TUint32& aCrc, const TUint8* aPtr, const TInt aLength)
	{
	if (!iCrcTableCalculated)
		CalcCrcTable();
	TUint32 code = aCrc;
	for (TInt i=0; i < aLength; i++)
		code = iCrcTable[(code ^ aPtr[i]) & 0xff] ^ (code >> 8);
	aCrc = code;
	}

void CPngWriteCodec::CalcCrcTable()
	{
	for (TInt i=0; i < KPngCrcTableLength; i++)
		{
		TUint32 code = static_cast<TUint32>(i);

		for (TInt j = 0; j < 8; j++)
			{
			if (code & 1)
				code = 0xedb88320 ^ (code >> 1);
			else
				code = code >> 1;
			}
		iCrcTable[i] = code;
		}
	iCrcTableCalculated = ETrue;
	}
