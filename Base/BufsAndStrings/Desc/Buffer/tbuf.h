/*
 * tbuf.h
 *
 *  Created on: 2010-9-28
 *      Author: thomas
 */

#ifndef TBUF_H
#define TBUF_H

#include <e32base.h>

void dump_tbuf_content_len_size_maxlen_2(const TDes &aBuf);
void dump_tbuf_content_len_size_maxlen(const TDes &aBuf);

void example_tbuf_prompt();
void example_tbuf_append(TDes &aBuf);
void example_tbuf_setlength(TDes &aBuf);
void example_tbuf_zero(TDes &aBuf);
void example_tbuf_assign_with_lit_normal(TDes &aBuf);
void example_tbuf_assign_with_lit_panic(TDes &aBuf);

#endif /* TBUF_H */
