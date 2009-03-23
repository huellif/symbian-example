// ExampleConv.cpp
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.

///////////////////////////////////////////////////////////////////////
// This is an example converter that converts a file containing
// text encoded in quoted-printable to plain text.
// _________________________________________________________________

#include <conarc.h>
#include <ecom.h>
#include <ImplementationProxy.h>
#include "ExampleConv.hrh" 
#include "ExampleConv.h"

#include <E32STD.H>
#include <e32uid.h>
#include <S32STRM.H>

#define KLineFeed 10
#define KCarriageReturn 13
#define KEquals '='

CConverterBase2* CExampleConverter::NewL()
    {
    CConverterBase2* egConv=new (ELeave) CExampleConverter();
    return egConv;
    }

// Standard ECOM factory code
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KExampleConvImplementationUid,CExampleConverter::NewL)
    };

// Standard ECOM factory code
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// Prepares the converter for conversion. This is called by the converter's ConvertObjectL() 
// once before calling DoConvertL() in a loop.
void CExampleConverter::ConvertObjectAL(RReadStream& aReadStream, RWriteStream& aWriteStream, MConverterUiObserver* /*aObserver*/)
    {
    iReadStream = &aReadStream;
    iWriteStream = &aWriteStream;
    iBufferPos = 0;
    iBuffer.SetMax();
    }

// Performs a step in converting the data. Called by the converter's ConvertObjectL()
// repeatedly until it returns false.
TBool CExampleConverter::DoConvertL()
    {
    // Check if we need to move out the oldest character
    if (iBufferPos == 3)
        {
        iBuffer[0] = iBuffer[1];
        iBuffer[1] = iBuffer[2];
        iBufferPos = 2;
        }

    TBuf8<1> character;
    TRAPD(err, iReadStream->ReadL(character));
    if (err == KErrEof)
        {
        // Write what we have in the buffer, and exit
        iWriteStream->WriteL(iBuffer, iBufferPos);
        return EFalse;
        }
    else if (err == KErrNone)
        {
        iBuffer[iBufferPos] = character[0];
        iBufferPos++;
        if (iBufferPos == 3)
            {
            // Check that we have three characters to compare:
            if (iBuffer[0] == KEquals)
                {
                // Check for special QP codes
                if (!(iBuffer[1] == KCarriageReturn && iBuffer[2] == KLineFeed))
                    {
                    // act on the code
                    TLex8 lex(iBuffer);
                    TUint code;
                    (void)lex.Get(); // skip the equals
                    lex.Val(code,EHex);
                    iWriteStream->WriteUint8L(STATIC_CAST(TUint8,code));
                    }
                iBufferPos=0;
                }
            else if (iBuffer[1] == KCarriageReturn && iBuffer[2] == KLineFeed)
                {
                // throw away these characters
                iBufferPos=0;
                }
            else
                {
                // Write out the last character in the stream
                iWriteStream->WriteUint8L(iBuffer[0]);
                }
            }
        }

    else
        {
        // Some other error
        return EFalse;
        }
    return ETrue;    
    }

TUid CExampleConverter::Uid()
    {
    return TUid::Uid(KExampleConvImplementationUid);
    }

TInt CExampleConverter::Capabilities()
    {
    return EConvertsObjects;
    }

// Cleans up after the conversion.
void CExampleConverter::CancelConvert()
    {
    iReadStream->Close();
    iWriteStream->Close();
    }    

CExampleConverter::CExampleConverter()
    {
    }
    
CExampleConverter::~CExampleConverter()
    {
    }
