// PNGProxy.cpp
//
// Copyright (c) 2001-2005 Symbian Software Ltd.  All rights reserved.
//

// Supplies the factory function called by ECom, and factories for
// the PNG decoder and encoder

// Ecom headers
#include <ECom.h>
#include <ImplementationProxy.h>

// ICL base class
#include <ICL\ImageConstruct.h>

#include "UIDS.h"
#include "PNGCodec.h"


//
// Factory for PNG decoder
//
class CPngDecodeConstruct : public CImageDecodeConstruct
	{
public:
	// Constructs the factory
	static CPngDecodeConstruct* NewL();

	// Supplies a new decoder
	// from CImageDecodeConstruct
	CImageDecoderPlugin* NewPluginL() const;
	};

CPngDecodeConstruct* CPngDecodeConstruct::NewL()
	{
	CPngDecodeConstruct* self = new (ELeave) CPngDecodeConstruct;
	return self;
	}

CImageDecoderPlugin* CPngDecodeConstruct::NewPluginL() const
	{
	return CPngDecoder::NewL();
	}


//
// Factory for PNG encoder
//
class CPngEncodeConstruct : public CImageEncodeConstruct
	{
public:
	// Constructs the factory
	static CPngEncodeConstruct* NewL();

	// Supplies a new encoder
	// from CImageEncodeConstruct
	CImageEncoderPlugin* NewPluginL() const;
	};

CPngEncodeConstruct* CPngEncodeConstruct::NewL()
	{
	CPngEncodeConstruct* self = new (ELeave) CPngEncodeConstruct;
	return self;
	}

CImageEncoderPlugin* CPngEncodeConstruct::NewPluginL() const
	{
	return CPngEncoder::NewL();
	}



//
// Exported proxy for ECom instantiation method resolution
//

// Define the factories for the PNG converters
const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KExPNGDecoderImplementationUidValue, CPngDecodeConstruct::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KExPNGEncoderImplementationUidValue, CPngEncodeConstruct::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

//
// Global panic function
//
GLDEF_C void Panic(TInt aError)
	{
	_LIT(KPNGPanicCategory, "PNGExConvertPlugin");
	User::Panic(KPNGPanicCategory, aError);
	}

