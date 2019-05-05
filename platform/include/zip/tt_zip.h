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
@file tt_zip.h
@brief zip APIs

this file specifies zip interfaces
*/

#ifndef __TT_ZIP__
#define __TT_ZIP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_algorithm_def.h>
#include <log/tt_log.h>
#include <misc/tt_util.h>
#include <time/tt_date.h>
#include <tt_basic_type.h>
#include <zip/tt_libzip.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u32_t reserved;
} tt_zip_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_zip_t *tt_zip_create(IN tt_zipsrc_t *z, IN tt_u32_t flag,
                                  IN OPT tt_zip_attr_t *attr);
#define TT_ZA_CREAT ZIP_CREATE
#define TT_ZA_EXCL ZIP_EXCL
#define TT_ZA_CHECKCONS ZIP_CHECKCONS
#define TT_ZA_TRUNCATE ZIP_TRUNCATE
#define TT_ZA_RDONLY ZIP_RDONLY

tt_export tt_zip_t *tt_zip_create_blob(IN void *p, IN tt_u32_t len,
                                       IN tt_bool_t free, IN tt_u32_t flag,
                                       IN OPT tt_zip_attr_t *attr);

tt_export tt_zip_t *tt_zip_create_file(IN const tt_char_t *path,
                                       IN tt_u64_t from, IN tt_u64_t len,
                                       IN tt_u32_t flag,
                                       IN OPT tt_zip_attr_t *attr);

tt_export void tt_zip_destroy(IN tt_zip_t *z, IN tt_bool_t flush);

tt_export void tt_zip_attr_default(IN tt_zip_attr_t *attr);

tt_inline const tt_char_t *tt_zip_strerror(IN tt_zip_t *z)
{
    return zip_error_strerror(zip_get_error(z));
}

tt_inline tt_u32_t tt_zip_add_file(IN tt_zip_t *z, IN const tt_char_t *name,
                                   IN tt_zipsrc_t *zs, IN tt_u32_t flag)
{
    zip_int64_t i = zip_file_add(z, name, zs, flag);
    if (i >= 0) {
        return (tt_u32_t)i;
    } else {
        TT_ERROR("fail to add zip file[%s]: %s", name, tt_zip_strerror(z));
        return TT_POS_NULL;
    }
}

tt_inline tt_u32_t tt_zip_add_dir(IN tt_zip_t *z, IN const tt_char_t *name,
                                  IN tt_u32_t flag)
{
    zip_int64_t i = zip_dir_add(z, name, flag);
    if (i >= 0) {
        return (tt_u32_t)i;
    } else {
        TT_ERROR("fail to add zip dir[%s]: %s", name, tt_zip_strerror(z));
        return TT_POS_NULL;
    }
}

tt_export tt_u32_t tt_zip_find(IN tt_zip_t *z, IN const tt_char_t *name,
                               IN tt_u32_t flag);

tt_inline tt_result_t tt_zip_replace(IN tt_zip_t *z, IN tt_u32_t index,
                                     IN tt_zipsrc_t *zs, IN tt_u32_t flag)
{
    if (zip_file_replace(z, index, zs, flag) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to replace zip file: %s", tt_zip_strerror(z));
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_zip_rename(IN tt_zip_t *z, IN tt_u32_t index,
                                    IN const tt_char_t *name, IN tt_u32_t flag)
{
    if (zip_file_rename(z, index, name, flag) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to rename zip file: %s", tt_zip_strerror(z));
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_zip_remove(IN tt_zip_t *z, IN tt_u32_t index)
{
    if (zip_delete(z, index) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to delete zip file: %s", tt_zip_strerror(z));
        return TT_FAIL;
    }
}

tt_inline tt_u32_t tt_zip_count(IN tt_zip_t *z, IN tt_u32_t flag)
{
    zip_int64_t n = zip_get_num_entries(z, flag);
    if (n >= 0) {
        return (tt_u32_t)n;
    } else {
        return ~0;
    }
}

tt_inline tt_result_t tt_zip_reset(IN tt_zip_t *z)
{
    if (zip_unchange_all(z) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to reset zip: %s", tt_zip_strerror(z));
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_zip_reset_file(IN tt_zip_t *z, IN tt_u32_t index)
{
    if (zip_unchange(z, index) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to reset zip file: %s", tt_zip_strerror(z));
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_zip_reset_meta(IN tt_zip_t *z)
{
    if (zip_unchange_archive(z) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to reset zip meta: %s", tt_zip_strerror(z));
        return TT_FAIL;
    }
}

tt_inline const tt_char_t *tt_zip_get_comment(IN tt_zip_t *z, IN tt_u32_t flag)
{
    return zip_get_archive_comment(z, NULL, flag);
}

tt_inline tt_result_t tt_zip_set_comment(IN tt_zip_t *z,
                                         IN const tt_char_t *comment,
                                         IN tt_u32_t len)
{
    if (zip_set_archive_comment(z, comment,
                                TT_COND(len != 0, len,
                                        (zip_uint16_t)tt_strlen(comment))) ==
        0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to reset zip comment: %s", tt_zip_strerror(z));
        return TT_FAIL;
    }
}

tt_inline const tt_char_t *tt_zip_get_fname(IN tt_zip_t *z, IN tt_u32_t index,
                                            IN tt_u32_t flag)
{
    return zip_get_name(z, index, flag);
}

tt_inline const tt_char_t *tt_zip_get_fcomment(IN tt_zip_t *z,
                                               IN tt_u32_t index,
                                               IN tt_u32_t flag)
{
    return zip_file_get_comment(z, index, NULL, flag);
}

tt_inline tt_result_t tt_zip_set_fcomment(IN tt_zip_t *z, IN tt_u32_t index,
                                          IN const tt_char_t *comment,
                                          IN tt_u32_t len, IN tt_u32_t flag)
{
    if (zip_file_set_comment(z, index, comment,
                             TT_COND(len != 0, len,
                                     (zip_uint16_t)tt_strlen(comment)),
                             flag) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to set zip file comment: %s", tt_zip_strerror(z));
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_zip_set_fcipher(IN tt_zip_t *z, IN tt_u32_t index,
                                         IN tt_zip_cipher_t cipher,
                                         IN const tt_char_t *password)
{
    if (zip_file_set_encryption(z, index, cipher, password) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to set zip file encryption: %s", tt_zip_strerror(z));
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_zip_set_fmtime(IN tt_zip_t *z, IN tt_u32_t index,
                                        IN tt_date_t *date)
{
    if (zip_file_set_mtime(z, index, (time_t)tt_date_diff_epoch_second(date),
                           0) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to set zip file mtime: %s", tt_zip_strerror(z));
        return TT_FAIL;
    }
}

#endif /* __TT_ZIP__ */
