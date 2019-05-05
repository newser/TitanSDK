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
@file tt_gzip_deflate.h
@brief gzip deflate APIs

this file specifies deflate interfaces of gzip
*/

#ifndef __TT_GZIP_DEFLATE__
#define __TT_GZIP_DEFLATE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <zip/tt_deflate.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef tt_deflate_t tt_gzipdef_t;

typedef tt_deflate_attr_t tt_gzipdef_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_gzipdef_create(IN tt_gzipdef_t *gzd,
                                        IN OPT tt_gzipdef_attr_t *attr);

tt_inline void tt_gzipdef_destroy(IN tt_gzipdef_t *gzd)
{
    tt_deflate_destroy(gzd);
}

tt_inline void tt_gzipdef_attr_default(IN tt_gzipdef_attr_t *attr)
{
    tt_deflate_attr_default(attr);
}

tt_inline tt_result_t tt_gzipdef_run(IN tt_gzipdef_t *gzd, IN tt_u8_t *ibuf,
                                     IN tt_u32_t ilen,
                                     OUT tt_u32_t *consumed_len,
                                     IN tt_u8_t *obuf, IN tt_u32_t olen,
                                     OUT tt_u32_t *produced_len,
                                     IN tt_bool_t all_in)
{
    return tt_deflate_run(gzd, ibuf, ilen, consumed_len, obuf, olen,
                          produced_len, all_in);
}

tt_inline void tt_gzipdef_reset(IN tt_gzipdef_t *gzd)
{
    tt_deflate_reset(gzd);
}

#endif /* __TT_GZIP_DEFLATE__ */
