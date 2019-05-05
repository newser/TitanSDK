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
@file tt_charset_def.h
@brief charset definitions

charset definitions
*/

#ifndef __TT_CHARSET_DEF__
#define __TT_CHARSET_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_ASSERT_CS TT_ASSERT

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum
{
    // unicode encoding
    TT_CHARSET_UTF8,
    TT_CHARSET_UTF16LE,
    TT_CHARSET_UTF16BE,
    TT_CHARSET_UTF32LE,
    TT_CHARSET_UTF32BE,

    // chinese encoding
    TT_CHARSET_GB2312,
    TT_CHARSET_GBK,
    TT_CHARSET_GB18030,

    TT_CHARSET_NUM
} tt_charset_t;
#define TT_CHARSET_VALID(c) ((c) < TT_CHARSET_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_CHARSET_DEF__ */
