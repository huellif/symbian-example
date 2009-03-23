// PNGConvert.CPP
//
// Copyright (c) 1999-2005 Symbian Software Ltd.  All rights reserved.
//

// General BAFL headers
#include <barsc.h>
#include <barsread.h>
#include <bautils.h>
// ICL headers
#include <ImageConversion.h>

#include "ImageUtils.h"
#include <101F4122_extra.rsg>
#include "UIDs.h"
#include "PNGCodec.h"

//
// PNG decoder class
//

// Simple factory function
CPngDecoder* CPngDecoder::NewL()
	{
	return new(ELeave) CPngDecoder;
	}

CPngDecoder::CPngDecoder()
	{
	}

// Destructor calls base class cleanup
CPngDecoder::~CPngDecoder()
	{
	Cleanup();
	}

// Gets the image type: always PNG, no sub type
void CPngDecoder::ImageType(TInt aFrameNumber, TUid& aImageType, TUid& aImageSubType) const
	{
	__ASSERT_ALWAYS(aFrameNumber == 0, Panic(KErrArgument));
	aImageType = KImageTypePNGUid;
	aImageSubType = KNullUid;
	}

// Scans the image header.
void CPngDecoder::ScanDataL()
	{
	// Validate that format is correct
	ReadFormatL();
	ASSERT(ImageReadCodec() == NULL);

	// Create a codec to read the PNG image
	CPngReadCodec* imageReadCodec;
	imageReadCodec = new(ELeave) CPngReadCodec;
	// Let the framework takes ownership of the codec
	SetImageReadCodec(imageReadCodec);
	imageReadCodec->ConstructL();

	// Fill in image information for all frames
	ReadFrameHeadersL();
	}

// Validate that the file is PNG format
void CPngDecoder::ReadFormatL()
	{
	// Read initial data block
	TPtrC8 bufferDes;
	ReadDataL(0, bufferDes, KPngFileSignatureLength);

	// Validate the header.
	if (bufferDes.Length() < KPngFileSignatureLength)
		User::Leave(KErrUnderflow);

	const TUint8* ptr = bufferDes.Ptr();
	if (Mem::Compare(ptr, KPngFileSignatureLength, KPngSignature, KPngFileSignatureLength)!=0)
		User::Leave(KErrCorrupt);

	// Set start position of image data following the header
	SetStartPosition(KPngFileSignatureLength);

	// Set maximum data length as don't know exactly
	SetDataLength(KMaxTInt);
	}

// Gets text descriptions of image properties
CFrameInfoStrings* CPngDecoder::FrameInfoStringsL(RFs& aFs, TInt aFrameNumber)
	{
	const TUid KPngCodecDllUid = {KExPNGCodecDllUidValue};

	// Strings are read from 101F4122_extra.rss
	RResourceFile resourceFile;
	OpenExtraResourceFileLC(aFs,KPngCodecDllUid,resourceFile);
	HBufC8* resourceInfo = resourceFile.AllocReadLC(THEDECODERINFO);
	TResourceReader resourceReader;
	resourceReader.SetBuffer(resourceInfo);

	TBuf<KCodecResourceStringMax> info;
	TBuf<KCodecResourceStringMax> templte;

	const TFrameInfo& frameInfo = FrameInfo(aFrameNumber);
	CFrameInfoStrings* frameInfoStrings = CFrameInfoStrings::NewLC();

	// Set decoder name
	info = resourceReader.ReadTPtrC();
	frameInfoStrings->SetDecoderL(info);
	// Set image format name
	info = resourceReader.ReadTPtrC();
	frameInfoStrings->SetFormatL(info);
	// Set image dimensions
	TInt width = frameInfo.iOverallSizeInPixels.iWidth;
	TInt height = frameInfo.iOverallSizeInPixels.iHeight;
	TInt depth = frameInfo.iBitsPerPixel;
	templte = resourceReader.ReadTPtrC();
	info.Format(templte, width, height);
	frameInfoStrings->SetDimensionsL(info);
	// Set image depth, for colour or b/w as appropriate
	CDesCArrayFlat* resourceArray = resourceReader.ReadDesCArrayL();
	CleanupStack::PushL(resourceArray);
	TUint formatIndex = (frameInfo.iFlags & TFrameInfo::EColor) ? 1 : 0;
	templte = (*resourceArray)[formatIndex];
	CleanupStack::PopAndDestroy(resourceArray);
	info.Format(templte, depth);
	frameInfoStrings->SetDepthL(info);
	// Set image details strings
	info = resourceReader.ReadTPtrC(); // read details, then see if we use it
	if (frameInfo.iFlags & TFrameInfo::EAlphaChannel && frameInfo.iFlags & TFrameInfo::EColor)
		{
		frameInfoStrings->SetDetailsL(info);
		}
	// Cleanup and return
	CleanupStack::Pop(frameInfoStrings); 
	CleanupStack::PopAndDestroy(2); // resourceInfo + resourceFile
	return frameInfoStrings;
	}

//
// PNG encoder class
//

// Simple factory function
CPngEncoder* CPngEncoder::NewL()
	{
	return new(ELeave) CPngEncoder;
	}

CPngEncoder::CPngEncoder()
	{
	}

// Destructor calls base class cleanup
CPngEncoder::~CPngEncoder()
	{
	CImageEncoderPlugin::Cleanup();
	}

// Sets up the codec to encode the frame
void CPngEncoder::PrepareEncoderL(const CFrameImageData* aFrameImageData)
	{
	// Default encode parameters
	TInt bpp = 24;
	TBool color = ETrue;
	TBool paletted = EFalse;
	TInt compressionLevel = TPngEncodeData::EDefaultCompression;

	// Use encode params in aFrameImageData, if present
	const TInt count = (aFrameImageData) ? aFrameImageData->FrameDataCount() : 0;
	for (TInt i=0; i < count; i++)
		{
		const TFrameDataBlock* encoderData = aFrameImageData->GetFrameData(i);
		if (encoderData->DataType() == KPNGEncodeDataUid)
			{
			const TPngEncodeData* pngEncodeData = static_cast<const TPngEncodeData*>(encoderData);
			bpp = pngEncodeData->iBitsPerPixel;
			color = pngEncodeData->iColor;
			paletted = pngEncodeData->iPaletted;
			compressionLevel = pngEncodeData->iLevel;
			}
		}

	// Create the codec to write a PNG image
	CPngWriteCodec* codec = new(ELeave) CPngWriteCodec(bpp, color, paletted, compressionLevel);
	// Let the framework takes ownership of the codec
	SetImageWriteCodec(codec);		
	codec->ConstructL();
	}
