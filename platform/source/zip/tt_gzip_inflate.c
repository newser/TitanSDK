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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <zip/tt_gzip_inflate.h>

#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <zip/tt_zlib.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_gzipinf_create(IN tt_gzipinf_t *gzi,
                              IN OPT tt_gzipinf_attr_t *attr)
{
    tt_inflate_attr_t __attr;
    int z_err;

    TT_ASSERT(gzi != NULL);

    if (attr != NULL) {
        tt_memcpy(&__attr, attr, sizeof(tt_inflate_attr_t));
    } else {
        tt_inflate_attr_default(&__attr);
    }
    attr = &__attr;
    TT_LIMIT_RANGE(attr->window_bits, 9, 15);

    TT_ZSTREAM_INIT(&gzi->zs);

    z_err = inflateInit2(&gzi->zs, attr->window_bits + 16); // gzip
    if (z_err != Z_OK) {
        TT_ERROR("fail to init gzip inflate");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
