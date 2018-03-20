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
@file tt_wchar.h
@brief wide char on windows

this file defines wide char APIs

windows APIs need wchar string as params of APIs ending with 'W', using
APIs ending with 'A' is not portable as the string is generally local
code page encoded which may vary in different area

to make program able to be distributed to different area, it should:
- guarantee file is utf8 encoded
- on windows, convert utf8 string to wide chars and pass to xxxW()
*/

#ifndef __TT_WCHAR__
#define __TT_WCHAR__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

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

extern wchar_t *tt_wchar_create(IN const tt_char_t *utf8_str,
                                IN tt_u32_t len,
                                OUT OPT tt_u32_t *wchar_bytes);

extern wchar_t *tt_wchar_create_ex(IN const tt_char_t *utf8_str,
                                   IN tt_u32_t len,
                                   OUT OPT tt_u32_t *wchar_bytes,
                                   IN tt_u32_t flag,
                                   IN void *val);
// returned wchar string has more space as specified by val, and the extra
// size are filled with 0
#define TT_WCHAR_CREATE_LONGER (1 << 0)

extern void tt_wchar_destroy(IN wchar_t *wchar_str);

extern tt_char_t *tt_utf8_create(IN wchar_t *wchar_str,
                                 IN tt_u32_t len,
                                 OUT OPT tt_u32_t *utf8_bytes);

extern void tt_utf8_destroy(IN tt_char_t *utf8_str);

#endif /* __TT_WCHAR__ */
