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
@file tt_zlib_inflate.h
@brief zlib inflate APIs

this file specifies inflate interfaces of zlib
*/

#ifndef __TT_ZLIB_INFLATE__
#define __TT_ZLIB_INFLATE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <zip/tt_inflate.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef tt_inflate_t tt_zlibinf_t;

typedef tt_inflate_attr_t tt_zlibinf_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_zlibinf_create(IN tt_zlibinf_t *zli,
                                        IN OPT tt_zlibinf_attr_t *attr);

tt_inline void tt_zlibinf_destroy(IN tt_zlibinf_t *zli)
{
    tt_inflate_destroy(zli);
}

tt_inline void tt_zlibinf_attr_default(IN tt_zlibinf_attr_t *attr)
{
    tt_inflate_attr_default(attr);
}

tt_inline tt_result_t tt_zlibinf_run(IN tt_zlibinf_t *zli, IN tt_u8_t *ibuf,
                                     IN tt_u32_t ilen,
                                     OUT tt_u32_t *consumed_len,
                                     IN tt_u8_t *obuf, IN tt_u32_t olen,
                                     OUT tt_u32_t *produced_len,
                                     IN tt_bool_t finish)
{
    return tt_inflate_run(zli, ibuf, ilen, consumed_len, obuf, olen,
                          produced_len, finish);
}

tt_inline void tt_zlibinf_reset(IN tt_zlibinf_t *zli)
{
    tt_inflate_reset(zli);
}

#endif /* __TT_ZLIB_INFLATE__ */
