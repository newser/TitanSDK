/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file tt_buffer_common.h
@brief byte buffer common operations

this file defines buffer common operations APIs
*/

#ifndef __TT_BUFFER_COMMON__
#define __TT_BUFFER_COMMON__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_buf_swap(IN tt_buf_t *a, IN tt_buf_t *b);

// return 0 if equal, otherwise non 0 value is returned
extern tt_s32_t tt_buf_cmp(IN tt_buf_t *a, IN tt_buf_t *b);

extern tt_s32_t tt_buf_cmp_cstr(IN tt_buf_t *a, IN const tt_char_t *cstr);

tt_inline tt_result_t tt_buf_copy(IN tt_buf_t *dst, IN tt_buf_t *src)
{
    tt_buf_clear(dst);
    return tt_buf_put(dst, TT_BUF_RPOS(src), TT_BUF_RLEN(src));
}

extern void tt_buf_remove(IN tt_buf_t *buf, IN tt_u32_t pos);

extern void tt_buf_remove_range(IN tt_buf_t *buf,
                                IN tt_u32_t from,
                                IN tt_u32_t to);

extern tt_result_t tt_buf_insert(IN tt_buf_t *buf,
                                 IN tt_u32_t idx,
                                 IN tt_u8_t *data,
                                 IN tt_u32_t data_len);

tt_inline tt_result_t tt_buf_insert_cstr(IN tt_buf_t *buf,
                                         IN tt_u32_t idx,
                                         IN const tt_char_t *cstr)
{
    return tt_buf_insert(buf, idx, (tt_u8_t *)cstr, (tt_u32_t)tt_strlen(cstr));
}

extern tt_result_t tt_buf_tok(IN tt_buf_t *buf,
                              IN tt_u8_t *sep,
                              IN tt_u32_t sep_num,
                              IN tt_u32_t flag,
                              OUT tt_u8_t **last,
                              OUT tt_u32_t *last_len);
// flag
#define TT_BUFTOK_NOEMPTY (1 << 0)

extern void tt_buf_trim_sp(IN tt_buf_t *buf);

#endif /* __TT_BUFFER_COMMON__ */
