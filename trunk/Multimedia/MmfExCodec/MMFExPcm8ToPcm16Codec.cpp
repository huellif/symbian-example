// MMFExPcm8Pcm16Codec.cpp
// Copyright 1997-2005 Symbian Software Ltd.  All rights reserved.
//

#include "MMFExPcm8ToPcm16Codec.h"


// Factory function
CMMFCodec* CMMFExPcm8Pcm16Codec::NewL(TAny* )
	{
	CMMFExPcm8Pcm16Codec* self=new(ELeave) CMMFExPcm8Pcm16Codec();
	return self;
	}

// Process source buffer and writes converted data to destination buffer
// The codec expands 1 byte to 2 bytes
TCodecProcessResult CMMFExPcm8Pcm16Codec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	TCodecProcessResult result;
	result.iStatus = TCodecProcessResult::EProcessIncomplete;

	// cast from generic CMMFBuffer to CMMFDataBuffer
	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	iDst = STATIC_CAST(CMMFDataBuffer*, &aDst);

	const TUint dstMaxLen = iDst->Data().MaxLength();
	if (!dstMaxLen)
		User::Leave(KErrArgument);

	//don't scribble Destination (pDst) by only consuming enough source to fill pDst
	TUint srcUse = (dstMaxLen - iDst->Position()) / 2;
	const TUint srcLen = iSrc->Data().Length();
	const TUint sourceRemain = srcLen - iSrc->Position();

	//make sure we don't blow source by checking against remaining
	//and clipping to minimum remaining if necessary
	srcUse = (srcUse<sourceRemain ? srcUse : sourceRemain);
	
	//we need to cast away CONST even on the source, as the TClass needs a TUint8*
	TUint8* pSrc = CONST_CAST(TUint8*,iSrc->Data().Ptr());
	pSrc += iSrc->Position();
	TUint8* pDst = CONST_CAST(TUint8*,iDst->Data().Ptr());
	pDst += iDst->Position();

	Convert(pSrc, pDst, srcUse );

	if ((srcUse * 2) + iDst->Position() < dstMaxLen)
		result.iStatus = TCodecProcessResult::EDstNotFilled;

	else if (srcUse + iSrc->Position() >= srcLen)
		result.iStatus = TCodecProcessResult::EProcessComplete;

	result.iSrcBytesProcessed = srcUse;
	result.iDstBytesAdded = srcUse * 2;

	iDst->Data().SetLength(iDst->Position() + (srcUse * 2));

	return result;
	}


// Helper to convert signed 8-bit to signed 16-bit
void CMMFExPcm8Pcm16Codec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
	const TInt KAndMask8bit = 0xff;
	TInt s16;
	while (aSamples--)
		{ 
		s16 = (*aSrc++)<<8;
		*aDst++ = STATIC_CAST( TInt8, s16&KAndMask8bit);
		*aDst++ = STATIC_CAST( TInt8, (s16>>8)&KAndMask8bit);
		}
	}
