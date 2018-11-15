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
@file tt_percent_encode.h
@brief percent encode

this file defines percent encode api
*/

#ifndef __TT_PERCENT_ENCODE__
#define __TT_PERCENT_ENCODE__

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

// safe for 0 len
tt_export tt_u32_t tt_percent_encode_len(IN const tt_char_t *str,
                                         IN tt_u32_t len,
                                         IN tt_char_t *enc_tbl);

// safe for 0 len
tt_export tt_u32_t tt_percent_encode(IN const tt_char_t *str,
                                     IN tt_u32_t len,
                                     IN tt_char_t *enc_tbl,
                                     OUT tt_char_t *dst);

#endif /* __TT_PERCENT_ENCODE__ */
