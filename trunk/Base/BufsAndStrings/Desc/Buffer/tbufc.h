/*
 * tbufc.h
 *
 *  Created on: 2010-9-28
 *      Author: thomas
 */

#ifndef TBUFC_H_
#define TBUFC_H_

void dump_cstr_content(TText *cstr);
void dump_tbufc_content_len_size(TBufC<16> &aBufC);
void dump_tbufc_contentaddress_len_size(TBufC<16> &aBufC);
void dump_tptr_content_contentaddress_len_size(TPtr &aPtr);
void dump_tbuf_content_desandcontent_address_len_size_maxlen(TBuf<16> &aBuf);

void example_tbufc_init_with_cstr();
void example_tbufc_replace_with_lit_normal(TBufC<16> &aBufC);
void example_tbufc_replace_with_lit_panic(TBufC<16> &/*aBufC*/);
void example_tbufc_modify_with_tptr(TBufC<16> &aBufC);

#endif /* TBUFC_H_ */
