// EXAMPLERECOGNIZER.H
//
// Copyright (c) Symbian Software Ltd 2005.  All rights reserved.
//

#if !defined(__EXAMPLERECOGNIZER_H__)
#define __EXAMPLERECOGNIZER_H__

#if !defined(__APMREC_H__)
#include <apmrec.h>
#endif

/*
CExampleNewRecognizer is a concrete data recognizer.
It implements CApaDataRecognizerType, the abstract base 
class for recognizers.
*/
class CExampleNewRecognizer: public CApaDataRecognizerType
	{
public:
    CExampleNewRecognizer();
    TUint PreferredBufSize();
	TDataType SupportedDataTypeL(TInt) const;   
    static CApaDataRecognizerType* CreateRecognizerL();
private:
    void DoRecognizeL(const TDesC&, const TDesC8&);
	};

#endif


