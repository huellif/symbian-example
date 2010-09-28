// Buffer.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.
// Examples to demonstrate the basic ideas of
// buffer descriptors.
#include "CommonFramework.h"
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

void example_tbuf_prompt()
    {
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    _LIT(KTxtBasicConcepts, "\n-->TBuf basic concepts");
    console->Printf(KTxtBasicConcepts);
    _LIT(KTxtPressToContinue, " (press any key to continue)\n");
    console->Printf(KTxtPressToContinue);
    console->Getch();
    }

void show_tbuf_content_len_size_maxlen_2(TBuf<16> &aBuf)
    {
    console->Printf(KContentFormat, &aBuf);
    console->Printf(KLenSizeFormat, aBuf.Length(), aBuf.Size());
    console->Printf(KMaxLenFormat2, aBuf.MaxLength());
    }

void example_tbuf_append_tbuf(TBuf<16> &aBuf)
    {
    // The data can be modified
    aBuf.Append('@');
    show_tbuf_content_len_size_maxlen_2(aBuf);
    }

void show_tbuf_content_len_size_maxlen(TBuf<16> &aBuf)
    {
    console->Printf(KContentFormat, &aBuf);
    console->Printf(KLenSizeFormat, aBuf.Length(), aBuf.Size());
    console->Printf(KMaxLenFormat2, aBuf.MaxLength());
    }

void example_tbuf_setlength(TBuf<16> &aBuf)
    {
    // Length can be changed; data represented
    // by the descriptor is now "Hel"
    aBuf.SetLength(3);
    show_tbuf_content_len_size_maxlen(aBuf);
    }

void example_tbuf_zero(TBuf<16> &aBuf)
    {
    // Length can be zeroised; no data is now 
    // represented by the descriptor but 
    // the maximum length is still 16
    aBuf.Zero();
    show_tbuf_content_len_size_maxlen(aBuf);
    }

void example_tbuf_replace_with_lit_normal(TBuf<16> &aBuf)
    {
    // The data can be replaced entirely 
    // using the assignment operator.
    // The replacement text must not have a
    // length greater than 16.
    aBuf = KTxtRepText;
    show_tbuf_content_len_size_maxlen(aBuf);
    }

void example_tbuf_replace_with_lit_panic(TBuf<16> &/*aBuf*/)
    {
    // Replacing text which has a length > 16
    // causes panic !!
    // 
    // Remove the "//" marks on the next two lines
    // to see this happen
    //_LIT(KTxtRepTextPanic,"Text replacement causes panic");
    //buf = _L("Text replacement causes panic!");
    }

LOCAL_C void doExampleL()
    {
    example_tbufc_init_with_cstr();

    // If the TBufC is to hold string data on
    // construction, use a _LIT macro.
    TBufC<16> bufc2(KTxtHelloWorld);
    show_tbufc_content_len_size(bufc2);

    example_tbufc_replace_with_lit_normal(bufc2);
    example_tbufc_replace_with_lit_panic(bufc2);
    example_tbufc_modify_with_tptr(bufc2);

    example_tbuf_prompt();

    // Construct a TBuf using a Literal
    TBuf<16> buf(KTxtHelloWorld);

    // Look at: 
    //   1. Descriptor content
    //   2. Address of descriptor
    //   3. Address of descriptor data area
    //   4. Length of descriptor
    //   5. Size of descriptor
    //   6. Maximum length of descriptor      
    //
    // Like a TBufC, the address of the descriptor
    // data area is offset 4 from the start of the
    // descriptor itself.
    //
    // Descriptor length is 12.
    //
    // The template parameter value defines 
    // the maximum length of the descriptor. 
    // and, therefore, governs its size 
    // (depending on the build variant).
    // Size of data is 24

    show_tbuf_content_desandcontent_address_len_size_maxlen(buf);

    example_tbuf_append_tbuf(buf);
    example_tbuf_setlength(buf);
    example_tbuf_zero(buf);
    example_tbuf_replace_with_lit_normal(buf);
    example_tbuf_replace_with_lit_panic(buf);
    }

