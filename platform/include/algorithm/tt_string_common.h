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
@file tt_string_common.h
@brief string common operations

this file defines string common APIs
*/

#ifndef __TT_STRING_COMMON__
#define __TT_STRING_COMMON__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_string.h>

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

// return 0 if out of bounds
tt_export tt_char_t tt_string_getchar(IN tt_string_t *s, IN tt_u32_t pos);

tt_inline tt_s32_t tt_string_cmp(IN tt_string_t *a, IN const tt_char_t *b)
{
    return tt_strcmp(tt_string_cstr(a), b);
}

// return false if [from, from+len) exceed either a or b
tt_export tt_bool_t tt_string_equal_range(IN tt_string_t *a,
                                          IN const tt_char_t *b,
                                          IN tt_u32_t from,
                                          IN tt_u32_t len);

tt_export tt_s32_t tt_string_ncasecmp(IN tt_string_t *a, IN const tt_char_t *b);

tt_export tt_result_t tt_string_copy(OUT tt_string_t *dst, IN tt_string_t *src);

tt_export void tt_string_swap(IN tt_string_t *a, IN tt_string_t *b);

// if substr is empty, return 0
tt_export tt_u32_t tt_string_find(IN tt_string_t *s,
                                  IN const tt_char_t *substr);

tt_export tt_u32_t tt_string_findfrom(IN tt_string_t *s,
                                      IN tt_u32_t from,
                                      IN const tt_char_t *substr);

tt_export tt_u32_t tt_string_find_c(IN tt_string_t *s, IN tt_char_t c);

tt_export tt_u32_t tt_string_findfrom_c(IN tt_string_t *s,
                                        IN tt_u32_t from,
                                        IN tt_char_t c);

tt_export tt_u32_t tt_string_rfind_c(IN tt_string_t *s, IN tt_char_t c);

tt_export void tt_string_remove_range(IN tt_string_t *s,
                                      IN tt_u32_t from,
                                      IN tt_u32_t len);

tt_export tt_result_t tt_string_append(IN OUT tt_string_t *s,
                                       IN const tt_char_t *substr);

tt_export tt_result_t tt_string_append_c(IN OUT tt_string_t *s, IN tt_char_t c);

tt_export tt_result_t tt_string_append_rep(IN OUT tt_string_t *s,
                                           IN tt_char_t c,
                                           IN tt_u32_t c_num);

tt_export tt_result_t tt_string_append_sub(IN OUT tt_string_t *s,
                                           IN const tt_char_t *substr,
                                           IN tt_u32_t from,
                                           IN tt_u32_t len);

tt_export tt_bool_t tt_string_startwith(IN tt_string_t *s,
                                        IN const tt_char_t *substr);

tt_export tt_bool_t tt_string_endwith(IN tt_string_t *s,
                                      IN const tt_char_t *substr);

tt_export tt_result_t tt_string_substr(IN tt_string_t *s,
                                       IN tt_u32_t from,
                                       IN tt_u32_t len,
                                       OUT tt_string_t *substr);

tt_export tt_s32_t tt_string_replace(IN OUT tt_string_t *s,
                                     IN tt_string_t *oldstr,
                                     IN tt_string_t *newstr);

tt_export tt_s32_t tt_string_replace_c(IN OUT tt_string_t *s,
                                       IN tt_char_t oldchar,
                                       IN tt_char_t newchar);

tt_export void tt_string_tolower(IN OUT tt_string_t *s);

tt_export void tt_string_toupper(IN OUT tt_string_t *s);

tt_export void tt_string_trim(IN OUT tt_string_t *s);

tt_export tt_result_t tt_string_insert(IN OUT tt_string_t *s,
                                       IN tt_u32_t from,
                                       IN const tt_char_t *substr);

tt_export tt_result_t tt_string_insert_c(IN OUT tt_string_t *s,
                                         IN tt_u32_t from,
                                         IN tt_char_t c);

#endif /* __TT_STRING_COMMON__ */
