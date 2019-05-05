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
@file tt_inflate.h
@brief inflate APIs

this file specifies inflate interfaces
*/

#ifndef __TT_INFLATE__
#define __TT_INFLATE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <zlib.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    z_stream zs;
} tt_inflate_t;

typedef struct
{
    tt_u32_t window_bits; // 8 - 15
} tt_inflate_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_inflate_create(IN tt_inflate_t *ifl,
                                        IN OPT tt_inflate_attr_t *attr);

tt_export void tt_inflate_destroy(IN tt_inflate_t *ifl);

tt_export void tt_inflate_attr_default(IN tt_inflate_attr_t *attr);

tt_export tt_result_t tt_inflate_run(IN tt_inflate_t *ifl, IN tt_u8_t *ibuf,
                                     IN tt_u32_t ilen,
                                     OUT tt_u32_t *consumed_len,
                                     IN tt_u8_t *obuf, IN tt_u32_t olen,
                                     OUT tt_u32_t *produced_len,
                                     IN tt_bool_t finish);

tt_export void tt_inflate_reset(IN tt_inflate_t *ifl);

#endif /* __TT_INFLATE__ */
