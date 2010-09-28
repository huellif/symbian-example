/*
 * buf.c
 *
 *  Created on: 2010-9-28
 *      Author: thomas
 */

#include <e32cons.h>

#include "buf_format.h"
#include "tbuf.h"

GLREF_C CConsoleBase* console;

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
