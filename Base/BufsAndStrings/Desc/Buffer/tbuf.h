/*
 * tbuf.h
 *
 *  Created on: 2010-9-28
 *      Author: thomas
 */

#ifndef TBUF_H
#define TBUF_H

#include <e32base.h>

void show_tbuf_content_len_size_maxlen_2(TBuf<16> &aBuf);
void show_tbuf_content_len_size_maxlen(TBuf<16> &aBuf);

void example_tbuf_prompt();
void example_tbuf_append_tbuf(TBuf<16> &aBuf);
void example_tbuf_setlength(TBuf<16> &aBuf);
void example_tbuf_zero(TBuf<16> &aBuf);
void example_tbuf_replace_with_lit_normal(TBuf<16> &aBuf);
void example_tbuf_replace_with_lit_panic(TBuf<16> &aBuf);

#endif /* TBUF_H */
