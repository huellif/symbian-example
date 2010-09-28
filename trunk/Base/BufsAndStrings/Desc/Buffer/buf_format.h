/*
 * buf_format.h
 *
 *  Created on: 2010-9-28
 *      Author: thomas
 */

#ifndef BUF_FORMAT_H_
#define BUF_FORMAT_H_

#include <e32base.h>

//
// Common literal text
//
_LIT(KTxtHelloWorld, "Hello World!");
_LIT(KTxtRepText, "Replacement text");
_LIT(KTxtTBufC, "TBufC: ");
_LIT(KTxtTPtr, "TPtr:  ");

//
// Common Format strings
//
_LIT(KContentFormat, "\"%S\"; ");
_LIT(KDesAndContentAddressFormat, "Descriptor at 0x%08x; Ptr()=0x%08x; ");
_LIT(KContentAddressLenSizeFormat, "\"%S\"; Ptr()=0x%08x; Length()=%d; Size()=%d\n");
_LIT(KContentAddressLenSizeFormat2, "\"%S\"; Ptr()=0x%08x; Length()=%d; Size()=%d; ");
_LIT(KMaxLenFormat, "\nMaxLength()=%d\n");
_LIT(KLenSizeFormat, "Length()=%d; Size()=%d;\n");
_LIT(KMaxLenFormat2, "MaxLength()=%d\n");

void prompt_case(TDesC& aDesC);

#endif /* BUF_FORMAT_H_ */
