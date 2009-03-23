// EXAMPLECONV.h
//
// Copyright (c) 2006 Symbian  Software Ltd.  All rights reserved.
//
//

#if (!defined __EXAMPLECONV_H__)
#define __EXAMPLECONV_H____

#include <conarc.h>
#include <ecom.h>
#include <ImplementationProxy.h>

// CExampleConverter inherits from the abstract base class CConverterBase2.
// CConverterBase2 is the base class for ECom style converters.
class CExampleConverter : public CConverterBase2 
    {
public: static CConverterBase2* NewL();
    CExampleConverter();
    ~CExampleConverter();
    // from CConverterBase
    void ConvertObjectAL(RReadStream& aReadStream, RWriteStream& aWriteStream, MConverterUiObserver* aObserver=NULL);
    TBool DoConvertL();
    TUid Uid();
    TInt Capabilities();
    void CancelConvert();
private:
    RReadStream* iReadStream;
    RWriteStream* iWriteStream;
    TBuf8<3> iBuffer;
    TInt iBufferPos;
    };

#endif /* __EXAMPLECONV_H___ */
