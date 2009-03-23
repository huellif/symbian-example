// PngCodec.H
//
// Copyright (c) 1999-2005 Symbian Software Ltd.  All rights reserved.
//

#ifndef __PNGCODEC_H__
#define __PNGCODEC_H__

#include <bitdev.h>
#include <ezcompressor.h>
#include <ezdecompressor.h>
#include <ICL/ImageProcessor.h>
#include <ICL/ImageCodec.h>

#include "PngConvert.h"

//.
// Constants.relating to PNG standard 
//
const TInt KPngFileSignatureLength = 8;
const TUint8 KPngSignature[KPngFileSignatureLength] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

const TInt KPngChunkIdSize = 4;
const TInt KPngMaxPLTESize = 256;
const TInt KPngNumInterlacedPasses = 8; // 7 passes plus a safety entry

const TInt KPngChunkLengthSize = 4;
const TInt KPngChunkCRCSize = 4;

const TInt KPngIHDRChunkSize = 13;
const TInt KPngIENDChunkSize = 0;
const TInt KPngcHRMChunkSize = 32;
const TInt KPnggAMAChunkSize = 4;
const TInt KPngpHYsChunkSize = 9;
const TInt KPngtIMEChunkSize = 7;

// KPngMaxImageSize is the maximum size for width and height, as defined in the PNG Specification v1.0 page 14
const TUint KPngMaxImageSize = ((TUint)2 << 30) - 1;
const TInt KPngCrcTableLength = 256;
const TUint KPngCrcMask = 0xffffffff;

_LIT8(KPngIHDRChunkId,"IHDR");
_LIT8(KPngPLTEChunkId,"PLTE");
_LIT8(KPngIDATChunkId,"IDAT");
_LIT8(KPngIENDChunkId,"IEND");
_LIT8(KPngbKGDChunkId,"bKGD");
_LIT8(KPngpHYsChunkId,"pHYs");
_LIT8(KPngtRNSChunkId,"tRNS");


// .
// Encapsulates information about a PNG image
// See PNG standard for details.
// 
class TPngImageInformation : public TFrameInfo
	{
public:
	TPngImageInformation();
public:
	enum TColorElements
		{
		EMonochrome = 0x0,
		EPaletteUsed = 0x1,
		EColorUsed = 0x2,
		EAlphaChannelUsed = 0x4
		};
	enum TColorType
		{
		EGrayscale = EMonochrome, // 0
		EDirectColor = EColorUsed, // 2
		EIndexedColor = EColorUsed | EPaletteUsed, // 3
		EAlphaGrayscale = EMonochrome | EAlphaChannelUsed, // 4
		EAlphaDirectColor = EColorUsed | EAlphaChannelUsed // 6
		};
	enum TCompressionMethod
		{
		EDeflateInflate32K = 0
		};
	enum TFilterMethod
		{
		EAdaptiveFiltering = 0
		};
	enum TInterlaceMethod
		{
		ENoInterlace = 0,
		EAdam7Interlace = 1
		};
	enum TPhysicalUnits
		{
		EUnknownUnits = 0,
		EMeters = 1
		};
public:
	// IHDR chunk
	TSize iSize; // iWidth/iHeight = 1 ... (2^31)-1
	TInt iBitDepth; // 1,2,4,8,16 subject to color type restrictions
	TColorType iColorType; // 0,2,3,4,6
	TCompressionMethod iCompressionMethod; // 0
	TFilterMethod iFilterMethod; // 0
	TInterlaceMethod iInterlaceMethod; // 0 or 1

	// PLTE chunk
	TBool iPalettePresent;
	TRgb iPalette[KPngMaxPLTESize];

	// bKGD chunk
	TBool iBackgroundPresent;

	// pHYs chunk
	TBool iPhysicalPresent;
	TPhysicalUnits iPhysicalUnits;
	TSize iPhysicalSize;

	// tRNS chunk
	TBool iTransparencyPresent;
	TUint16 iTransparentGray;
	TUint16 iTransparentRed;
	TUint16 iTransparentGreen;
	TUint16 iTransparentBlue;
	TUint8 iTransparencyValue[KPngMaxPLTESize];
	};

//
// Handles reading raw PNG scanlines and preparing the data
// to be decompressed. A helper to CPngReadCodec.
// It is abstract, and subclassed to implement readers for 
// scanlines of different bitmap depths
//
class CPngReadSubCodec : public CBase
	{
public:
	static CPngReadSubCodec* NewL(CImageProcessor* aImageProc,CImageProcessor* aMaskProc,const TPngImageInformation& aInfo);
	virtual ~CPngReadSubCodec();
	TDes8& FirstBuffer();
	TDes8& DecodeL();
protected:
	CPngReadSubCodec();
	void WritePixel(TRgb aPixelColor);
	void WritePixel(TRgb aPixelColor,TUint8 aAlphaValue);
private:
	void ConstructL(CImageProcessor* aImageProc,CImageProcessor* aMaskProc,const TPngImageInformation& aInfo);
	void FilterScanlineDataL(TUint8* aDataPtr,const TUint8* aDataPtrLimit);
	TInt PaethPredictor(TInt aLeft,TInt aAbove,TInt aAboveLeft);
	virtual void DoConstructL() = 0;
	virtual TInt ScanlineBufferSize(TInt aPixelLength) = 0;
	virtual void DoDecode(TUint8* aDataPtr,const TUint8* aDataPtrLimit) = 0;
	void UpdatePos();
protected:
	CImageProcessor* iImageProc;
	CImageProcessor* iMaskProc;
	TPngImageInformation iInfo;
	TInt iScanlineBufferSize;
	TInt iBytesPerPixel;
	HBufC8* iScanlineBuffer1;
	HBufC8* iScanlineBuffer2;
	TPtr8 iScanlineDes1;
	TPtr8 iScanlineDes2;
	TInt iCurrentScanlineBuffer;
	TInt iInterlacedScanlineBufferSize[KPngNumInterlacedPasses];
	TInt iPass;
	TPoint iPos;
	};

class CPngDecoder;

//
// Codec class that does the real work of reading a PNG image.
// Inherits base classes that provide image mask processing
// and interface to unzip functionality
//
class CPngReadCodec : public CImageMaskProcessorReadCodec, public MEZBufferManager
	{
public:
	// Construction
	~CPngReadCodec();
	void ConstructL () { CImageMaskProcessorReadCodec::ConstructL(); } // make public, since we don't have NewL

private:
	// From CImageReadCodec
	// Initialise frame to read
	void InitFrameL(TFrameInfo& aFrameInfo, CFrameImageData& aFrameImageData, 
		TBool aDisableErrorDiffusion, CFbsBitmap& aDestination, CFbsBitmap* aDestinationMask);
	// Initialise from header
	void InitFrameHeader(TFrameInfo& aFrameSettings, CFrameImageData& /* aFrameImageData */);
	// Process header
	TFrameState ProcessFrameHeaderL(TBufPtr8& aData);
	// Process frame
	TFrameState ProcessFrameL(TBufPtr8& aSrc);

private:
	// Helper functions to process PNG chunk types
	void DoNewFrameL(CPngDecoder* aPngDecoder);
	void DoProcessInfoL(const TUint8*& aDataPtr,const TUint8* aDataPtrLimit);
	void DoProcessIHDRL(const TUint8* aDataPtr,TInt aChunkLength);
	void DoProcessPLTEL(const TUint8* aDataPtr,TInt aChunkLength);
	void DoProcessbKGDL(const TUint8* aDataPtr,TInt aChunkLength);
	void DoProcesspHYsL(const TUint8* aDataPtr,TInt aChunkLength);
	void DoProcesstRNSL(const TUint8* aDataPtr,TInt aChunkLength);
	void DoProcessDataL(const TUint8*& aDataPtr,const TUint8* aDataPtrLimit);

	// From MEZBufferManager: feed data to unzip
	void InitializeL(CEZZStream &aZStream);
	void NeedInputL(CEZZStream &aZStream);
	void NeedOutputL(CEZZStream &aZStream);
	void FinalizeL(CEZZStream &aZStream);

private:
	TFrameInfo* iFrameInfo;
	TPngImageInformation iImageInfo;
	TBuf8<KPngChunkIdSize> iChunkId;
	TInt iChunkBytesRemaining;
	CPngReadSubCodec* iDecoder;
	CEZDecompressor* iDecompressor;
	TPtrC8 iDataDes;
	};

//
// Handles preparing image data
// for compression and writing raw scanlines 
// A helper to CPngWriteCodec
// 
class CPngWriteSubCodec : public CBase
	{
public:
	static CPngWriteSubCodec* NewL(const TPngImageInformation& aInfo, const CFbsBitmap* aSource);
	virtual ~CPngWriteSubCodec();
protected:
	CPngWriteSubCodec();
private:
	void ConstructL(const TPngImageInformation& aInfo, const CFbsBitmap* aSource);
public:
	TDes8& Buffer();
	inline TInt BufferSize() const;
	TDes8& EncodeL(const TInt aScanline);
	inline CPalette* Palette() const;
protected:
	TUint8 ReverseBits(const TUint8 aValue) const;
	void EncodePalettedScanline(TUint8* aDataPtr, const CFbsBitmap* aSource, const TInt aScanline,
								const TInt aPixelsPerByte, const TInt aShiftValue);
private:
	virtual void DoConstructL();
	virtual TInt ScanlineBufferSize(TInt aPixelLength) = 0;
	virtual void DoEncode(const CFbsBitmap* aSource, const TInt aScanline,
					TUint8* aDataPtr, const TUint8* aDataPtrLimit) = 0;
protected:
	TPngImageInformation iInfo;
	const CFbsBitmap* iSource;
	TInt iScanlineBufferSize;
	HBufC8* iScanlineBuffer;
	TPtr8 iScanlineDes;
	CPalette* iPalette;
	};

//
// Codec class that does the real work of writing a PNG image.
// Inherits base classes to interface to zip functionality
//
class CPngWriteCodec : public CImageWriteCodec, public MEZBufferManager
	{
public:
	// Construction
	CPngWriteCodec(TInt aBpp, TBool aColor, TBool aPaletted, TInt aCompressionLevel);
	void ConstructL () { CImageWriteCodec::ConstructL(); } // make public, since we don't have NewL
	~CPngWriteCodec();

private:
	// from CImageWriteCodec
	void InitFrameL(TBufPtr8& aDst, const CFbsBitmap& aSource);
	TFrameState ProcessFrameL(TBufPtr8& aDst);

private:
	// Helper functions to write PNG chunks
	void WritePngChunk(TUint8*& aDestPtr, const TDesC8& aChunkId, const TDesC8& aData, TInt& aLength);
	void DeflateEncodedDataL(TBufPtr8& aDst, TFrameState& aState);
	void FlushCompressedDataL(TBufPtr8& aDst, TFrameState& aState);
	void WritePLTEChunk(TBufPtr8& aDst);
	void WriteIDATChunk(TBufPtr8& aDst);
	void WriteEndChunk(TBufPtr8& aDst);
	TInt WriteHeaderChunk(TBufPtr8& aDst);
	void GetImageDataL(TInt& aBytesToProcess);
	void GetPngScanLine(TDes8& aBuf, const TPoint& aPixel, TInt& aLength);
	void CalcCrcTable();
	void GetCrc(TUint32& aCrc, const TUint8* aPtr, const TInt aLength);

private:
	// from MEZBufferManager
	// feeds data to zip
	void InitializeL(CEZZStream &aZStream);
	void NeedInputL(CEZZStream &aZStream);
	void NeedOutputL(CEZZStream &aZStream);
	void FinalizeL(CEZZStream &aZStream);

private:
	enum TPngEncoderState
		{
		EPngDeflate,	// Deflate encoded image data
		EPngFlush,		// Flush encoded image data
		EPngWritePLTE,	// Write PLTE chunk
		EPngWriteIDAT,	// Write IDAT chunk
		EPngEndChunk	// Write end chunk
		};
	
private:
	TUint32 iCrcTable[KPngCrcTableLength];
	TBool iCrcTableCalculated;
	CPngWriteSubCodec* iEncoder;
	TPngEncoderState iEncoderState;
	TInt iCompressionLevel;
	CEZCompressor* iCompressor;
	TInt iScanline;
	TUint8* iDestStartPtr;
	TUint8* iDestPtr;
	TUint8* iDestPtrLimit;
	TPngImageInformation iImageInfo;
	TPtr8 iCompressorPtr;	// data area for compressor to write to
	TBool iCallAgain;
	};


// Global panic function
void Panic(TInt aError);

//
// inline definitions
//
inline TInt CPngWriteSubCodec::BufferSize() const
	{ return iScanlineBufferSize; }

inline CPalette* CPngWriteSubCodec::Palette() const
	{ return iPalette; }


#endif // __PNGCODEC_H__
