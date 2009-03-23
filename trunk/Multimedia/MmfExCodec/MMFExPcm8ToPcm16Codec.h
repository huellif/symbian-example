// MMFExPcm8Pcm16Codec.h
// Copyright 1997-2005 Symbian Software Ltd.  All rights reserved.
//


#ifndef __EXPCM8_PCM16_CODEC_H__
#define __EXPCM8_PCM16_CODEC_H__

#include <mmf\server\mmfcodec.h>

// Declares a MMF plug-in codec that converts
// from PCM8 format to PCM16
class CMMFExPcm8Pcm16Codec : public CMMFCodec
	{
public:
	// Construction
	static CMMFCodec* NewL(TAny* aInitParams);
	// Implement CMMFCodec
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);

private:
	// Helper function
	void Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples);

private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;
	};

#endif
