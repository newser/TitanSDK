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

#include <zip/tt_deflate.h>

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

tt_result_t tt_deflate_create(IN tt_deflate_t *dfl,
                              IN OPT tt_deflate_attr_t *attr)
{
    tt_deflate_attr_t __attr;
    int z_err;

    TT_ASSERT(dfl != NULL);

    if (attr != NULL) {
        tt_memcpy(&__attr, attr, sizeof(tt_deflate_attr_t));
    } else {
        tt_deflate_attr_default(&__attr);
    }
    attr = &__attr;
    TT_LIMIT_MAX(attr->level, 9);
    TT_LIMIT_RANGE(attr->window_bits, 9, 15);
    TT_LIMIT_RANGE(attr->mem_level, 1, 9);

    TT_ZSTREAM_INIT(&dfl->zs);

    z_err = deflateInit2(&dfl->zs,
                         attr->level,
                         Z_DEFLATED,
                         -(int)attr->window_bits,
                         attr->mem_level,
                         Z_DEFAULT_STRATEGY);
    if (z_err != Z_OK) {
        TT_ERROR("fail to init deflate");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_deflate_destroy(IN tt_deflate_t *dfl)
{
    TT_ASSERT(dfl != NULL);

    deflateEnd(&dfl->zs);
}

void tt_deflate_attr_default(IN tt_deflate_attr_t *attr)
{
    attr->level = Z_DEFAULT_COMPRESSION;
    attr->window_bits = 15;
    attr->mem_level = 8;
}

tt_result_t tt_deflate_run(IN tt_deflate_t *dfl,
                           IN tt_u8_t *ibuf,
                           IN tt_u32_t ilen,
                           OUT tt_u32_t *consumed_len,
                           IN tt_u8_t *obuf,
                           IN tt_u32_t olen,
                           OUT tt_u32_t *produced_len,
                           IN tt_bool_t finish)
{
    z_stream *zs = &dfl->zs;
    tt_u32_t il, ol;
    int z_err;

    if (ibuf != NULL) {
        zs->next_in = ibuf;
        zs->avail_in = ilen;
    }
    il = zs->avail_in;

    if (obuf != NULL) {
        zs->next_out = obuf;
        zs->avail_out = olen;
    }
    ol = zs->avail_out;

    z_err = deflate(zs, TT_COND(finish, Z_FINISH, Z_SYNC_FLUSH));
    if ((z_err == Z_OK) || (z_err == Z_BUF_ERROR)) {
        TT_SAFE_ASSIGN(consumed_len, il - zs->avail_in);
        TT_SAFE_ASSIGN(produced_len, ol - zs->avail_out);
        return TT_SUCCESS;
    } else if (z_err == Z_STREAM_END) {
        TT_SAFE_ASSIGN(consumed_len, il - zs->avail_in);
        TT_SAFE_ASSIGN(produced_len, ol - zs->avail_out);
        return TT_E_END;
    } else {
        TT_ERROR("deflate fail");
        return TT_FAIL;
    }
}

void tt_deflate_reset(IN tt_deflate_t *dfl)
{
    TT_ASSERT(dfl != NULL);

    deflateReset(&dfl->zs);
}
