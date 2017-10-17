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
@file tt_zip_source.h
@brief zip source APIs

this file specifies zip source interfaces
*/

#ifndef __TT_ZIP_SOURCE__
#define __TT_ZIP_SOURCE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <log/tt_log.h>
#include <misc/tt_util.h>
#include <tt_basic_type.h>
#include <zip/tt_libzip.h>

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

tt_inline void tt_zipsrc_ref(IN tt_zipsrc_t *zs)
{
    zip_source_keep(zs);
}

tt_inline void tt_zipsrc_release(IN tt_zipsrc_t *zs)
{
    zip_source_free(zs);
}

tt_inline tt_result_t tt_zipsrc_open(IN tt_zipsrc_t *zs)
{
    if (zip_source_open(zs) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to open zip source: %s",
                 zip_error_strerror(zip_source_error(zs)));
        return TT_FAIL;
    }
}

tt_inline void tt_zipsrc_close(IN tt_zipsrc_t *zs)
{
    zip_source_close(zs);
}

tt_inline tt_result_t tt_zipsrc_read(IN tt_zipsrc_t *zs,
                                     OUT tt_u8_t *buf,
                                     IN tt_u32_t len,
                                     OUT tt_u32_t *read_len)
{
    zip_int64_t n = zip_source_read(zs, buf, len);
    if (n > 0) {
        TT_SAFE_ASSIGN(read_len, (tt_u32_t)n);
        return TT_SUCCESS;
    } else if (n == 0) {
        TT_SAFE_ASSIGN(read_len, 0);
        return TT_E_END;
    } else {
        TT_ERROR("fail to read zip source: %s",
                 zip_error_strerror(zip_source_error(zs)));
        return TT_FAIL;
    }
}

#endif /* __TT_ZIP_SOURCE__ */
