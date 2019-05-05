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

#include <zip/tt_zip_source.h>

#include <misc/tt_assert.h>

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

tt_result_t tt_zipsrc_seek(IN tt_zipsrc_t *zs, IN tt_u32_t whence,
                           IN tt_s64_t offset)
{
    int w;

    TT_ASSERT(whence <= TT_ZSSEEK_END);
    switch (whence) {
    case TT_ZSSEEK_BEGIN: {
        w = SEEK_SET;
    } break;
    case TT_ZSSEEK_CUR: {
        w = SEEK_CUR;
    } break;
    default:
    case TT_ZSSEEK_END: {
        w = SEEK_END;
    } break;
    }

    if (zip_source_seek(zs, offset, w) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to seek zip source: %s",
                 zip_error_strerror(zip_source_error(zs)));
        return TT_FAIL;
    }
}

tt_result_t tt_zipsrc_seek_write(IN tt_zipsrc_t *zs, IN tt_u32_t whence,
                                 IN tt_s64_t offset)
{
    int w;

    TT_ASSERT(whence <= TT_ZSSEEK_END);
    switch (whence) {
    case TT_ZSSEEK_BEGIN: {
        w = SEEK_SET;
    } break;
    case TT_ZSSEEK_CUR: {
        w = SEEK_CUR;
    } break;
    default:
    case TT_ZSSEEK_END: {
        w = SEEK_END;
    } break;
    }

    if (zip_source_seek_write(zs, offset, w) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to seek write zip source: %s",
                 zip_error_strerror(zip_source_error(zs)));
        return TT_FAIL;
    }
}
