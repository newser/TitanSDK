/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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
@file tt_string.h
@brief string

this file defines string APIs
*/

#ifndef __TT_STRING__
#define __TT_STRING__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_ASSERT_STR TT_ASSERT

#define TT_STR_CHECK(str)                                                      \
    do {                                                                       \
        TT_BUF_CHECK(&(str)->buf);                                             \
        TT_ASSERT(TT_BUF_RLEN(&(str)->buf) > 0);                               \
        TT_ASSERT((str)->buf.p[TT_BUF_RLEN(&(str)->buf) - 1] == 0);            \
    } while (0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_buf_attr_t buf_attr;
} tt_string_attr_t;

typedef struct tt_string_s
{
    tt_buf_t buf;
} tt_string_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// size could be 0, then the str won't alloc mem initially
tt_export tt_result_t tt_string_create(IN tt_string_t *str,
                                       IN const tt_char_t *cstr,
                                       IN OPT tt_string_attr_t *attr);

// create string from intersection of [from, to) and cstr
tt_export tt_result_t tt_string_create_sub(IN tt_string_t *str,
                                           IN const tt_char_t *cstr,
                                           IN tt_u32_t from,
                                           IN tt_u32_t len,
                                           IN OPT tt_string_attr_t *attr);

tt_export tt_result_t tt_string_create_nocopy(IN tt_string_t *str,
                                              IN const tt_char_t *cstr,
                                              IN OPT tt_string_attr_t *attr);

tt_export void tt_string_init(IN tt_string_t *str,
                              IN OPT tt_string_attr_t *attr);

tt_export void tt_string_destroy(IN tt_string_t *str);

tt_export void tt_string_attr_default(IN tt_string_attr_t *attr);

tt_export tt_result_t tt_string_set(IN tt_string_t *str,
                                    IN const tt_char_t *cstr);

tt_export tt_result_t tt_string_set_sub(IN tt_string_t *str,
                                        IN const tt_char_t *cstr,
                                        IN tt_u32_t from,
                                        IN tt_u32_t len);

tt_export tt_result_t tt_string_setfrom(IN tt_string_t *str,
                                        IN tt_u32_t from,
                                        IN const tt_char_t *cstr);

tt_export tt_result_t tt_string_setfrom_c(IN tt_string_t *str,
                                          IN tt_u32_t from,
                                          IN tt_char_t c);

tt_export tt_result_t tt_string_set_range(IN tt_string_t *str,
                                          IN tt_u32_t from,
                                          IN tt_u32_t len,
                                          IN const tt_char_t *cstr);

tt_export void tt_string_clear(IN tt_string_t *str);

tt_export void tt_string_print(IN tt_string_t *str, IN tt_u32_t flag);

tt_inline tt_u32_t tt_string_len(IN tt_string_t *str)
{
    return TT_BUF_RLEN(&str->buf) - 1;
}

tt_inline tt_bool_t tt_string_empty(IN tt_string_t *str)
{
    return TT_BUF_RLEN(&str->buf) == 1 ? TT_TRUE : TT_FALSE;
}

// never return NULL
tt_inline const tt_char_t *tt_string_cstr(IN tt_string_t *str)
{
    return (tt_char_t *)TT_BUF_RPOS(&str->buf);
}

// return NULL only when from is invalid
tt_export const tt_char_t *tt_string_subcstr(IN tt_string_t *str,
                                             IN tt_u32_t from,
                                             OUT OPT tt_u32_t *subcstr_len);

tt_inline void tt_string_backup_rwp(IN tt_string_t *str,
                                    IN tt_u32_t *rd_pos,
                                    IN tt_u32_t *wr_pos)
{
    tt_buf_backup_rwp(&str->buf, rd_pos, wr_pos);
}

tt_inline void tt_string_restore_rwp(IN tt_string_t *str,
                                     IN tt_u32_t *rd_pos,
                                     IN tt_u32_t *wr_pos)
{
    tt_buf_restore_rwp(&str->buf, rd_pos, wr_pos);
}

#endif /* __TT_STRING__ */
