/*
 * tbuf.cpp
 *
 *  Created on: 2010-9-28
 *      Author: thomas
 */
#include "buf_format.h"
#include "tbufc.h"

#include <e32cons.h>

GLREF_C CConsoleBase* console;

void show_cstr_content(TText *cstr)
    {
    // Look at the address of the C string
    _LIT(KFormat1, "C string at 0x%08x; \n");
    console->Printf(KFormat1, &cstr[0]);
    }

void show_tbufc_content_desandcontent_address_len_size(TBufC<16> &aBufC)
    {
    // Look at: 
    //   1. Descriptor content
    //   2. Address of descriptor
    //   3. Address of descriptor data area
    //   4. Length of descriptor
    //   5. Size of descriptor
    //         
    // Address of descriptor data area is 
    // different from the address of cstr but
    // is offset 4 from the start of the 
    // descriptor itself.
    //
    // Descriptor length is 12.
    //
    // The template parameter value defines 
    // the length of the descriptor data area 
    // and, therefore, governs its size 
    // (depending on the build variant).
    // Size of data is 24
    console->Printf(KContentFormat, &aBufC);
    console->Printf(KDesAndContentAddressFormat, &aBufC, aBufC.Ptr());
    _LIT(KFormat4, "Length()=%d; Size()=%d\n");
    console->Printf(KFormat4, aBufC.Length(), aBufC.Size());
    }

void show_tbufc_content_len_size(TBufC<16> &aBufC)
    {
    _LIT(KFormat5,"\"%S\"; Length()=%d; Size()=%d\n");
    console->Printf(KFormat5, &aBufC, aBufC.Length(), aBufC.Size());
    }

void show_tbufc_contentaddress_len_size(TBufC<16> &aBufC)
    {
    console->Printf(KTxtTBufC);
    console->Printf(KContentAddressLenSizeFormat, &aBufC, aBufC.Ptr(),
            aBufC.Length(), aBufC.Size());
    }

void show_tptr_content_contentaddress_len_size(TPtr &aPtr)
    {
    console->Printf(KTxtTPtr);
    console->Printf(KContentAddressLenSizeFormat2, &aPtr, aPtr.Ptr(),
            aPtr.Length(), aPtr.Size());
    console->Printf(KMaxLenFormat, aPtr.MaxLength());
    }

void show_tbuf_content_desandcontent_address_len_size_maxlen(TBuf<16> &aBuf)
    {
    console->Printf(KContentFormat, &aBuf);
    console->Printf(KDesAndContentAddressFormat, &aBuf, aBuf.Ptr());
    console->Printf(KLenSizeFormat, aBuf.Length(), aBuf.Size());
    console->Printf(KMaxLenFormat2, aBuf.MaxLength());
    }

void example_tbufc_init_with_cstr()
    {
    // Set up an area and initialize to a 
    // C style string (including the NULL).
    TText cstr[13] =
        {
        'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\0'
        };

    // Construct a TBufC using the NULL 
    // terminated string in cstr to initialize
    // it.
    TBufC<16> bufc1(&cstr[0]);

    show_cstr_content(cstr);
    show_tbufc_content_desandcontent_address_len_size(bufc1);
    }

void example_tbufc_replace_with_lit_normal(TBufC<16> &aBufC)
    {
    // Cannot modify existing data but can replace
    // it entirely using assignment operator. 
    // The replacement text must not have a length 
    // greater than 16
    aBufC = KTxtRepText;
    show_tbufc_content_len_size(aBufC);
    }

void example_tbufc_replace_with_lit_panic(TBufC<16> &/*aBufC*/)
    {
    // Replacing text which has a length > 16
    // causes panic !!
    // 
    // Remove the "//" marks on the next two lines
    // to see this happen
    //_LIT(KTxtRepTextPanic,"Text replacement causes panic");
    //bufc2 = KTxtRepTextPanic;


    // The Des() function returns a TPtr to the
    // TBufC.
    // The TBufC data can be changed through 
    // the TPtr.
    // The maximum length of the TPtr is the 
    // value of the TBufC template parameter,
    // i.e. 16 
    }

void example_tbufc_modify_with_tptr(TBufC<16> &aBufC)
    {
    aBufC = KTxtHelloWorld;
    show_tbufc_contentaddress_len_size(aBufC);

    TPtr ptr = aBufC.Des();
    show_tptr_content_contentaddress_len_size(ptr);
    // Now change the TBufC data through
    // the TPtr. This is OK provided the length
    // of the changed data does not exceed 16.
    //
    // The following change deletes the last 
    // character (the "!") and appends 
    // the characters " & Hi".
    //
    // Note that the length of both the TBufC 
    // and the TPtr reflect the changed data.
    show_tbufc_contentaddress_len_size(aBufC);
    _LIT(KTxtAndHi, " & Hi");
    ptr.Delete((ptr.Length() - 1), 1);
    ptr.Append(KTxtAndHi);

    show_tptr_content_contentaddress_len_size(ptr);
    }
