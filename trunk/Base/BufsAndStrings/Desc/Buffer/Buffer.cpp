// Buffer.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.
// Examples to demonstrate the basic ideas of
// buffer descriptors.
#include "CommonFramework.h"
#include "buf_format.h"
#include "tbufc.h"
#include "tbuf.h"

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

