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

#include <zip/tt_zip.h>

#include <misc/tt_assert.h>
#include <zip/tt_zip_source.h>
#include <zip/tt_zip_source_blob.h>
#include <zip/tt_zip_source_file.h>

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

tt_zip_t *tt_zip_create(IN tt_zipsrc_t *zsrc,
                        IN tt_u32_t flag,
                        IN OPT tt_zip_attr_t *attr)
{
    tt_zip_attr_t __attr;
    zip_t *z;
    zip_error_t zerr;

    TT_ASSERT(zsrc != NULL);

    if (attr == NULL) {
        tt_zip_attr_default(&__attr);
        attr = &__attr;
    }

    z = zip_open_from_source(zsrc, flag, &zerr);
    if (z == NULL) {
        TT_ERROR("fail to create zip arc: %s", zip_error_strerror(&zerr));
        return NULL;
    }

    return z;
}

tt_zip_t *tt_zip_create_blob(IN void *p,
                             IN tt_u32_t len,
                             IN tt_bool_t free,
                             IN tt_u32_t flag,
                             IN OPT tt_zip_attr_t *attr)
{
    tt_zipsrc_t *zs;
    tt_zip_t *z;

    zs = tt_zipsrc_blob_create(p, len, free);
    if (zs == NULL) {
        return NULL;
    }

    z = tt_zip_create(zs, flag, attr);
    if (z == NULL) {
        tt_zipsrc_release(zs);
        return NULL;
    }

    return z;
}

tt_zip_t *tt_zip_create_file(IN const tt_char_t *path,
                             IN tt_u64_t from,
                             IN tt_u64_t len,
                             IN tt_u32_t flag,
                             IN OPT tt_zip_attr_t *attr)
{
    tt_zipsrc_t *zs;
    tt_zip_t *z;

    zs = tt_zipsrc_file_create(path, from, len);
    if (zs == NULL) {
        return NULL;
    }

    z = tt_zip_create(zs, flag, attr);
    if (z == NULL) {
        tt_zipsrc_release(zs);
        return NULL;
    }

    return z;
}

void tt_zip_destroy(IN tt_zip_t *z, IN tt_bool_t flush)
{
    if (flush) {
        if (zip_close(z) != 0) {
            TT_ERROR("fail to flush zip arc: %s", tt_zip_strerror(z));
        }
    } else {
        zip_discard(z);
    }
}

void tt_zip_attr_default(IN tt_zip_attr_t *attr)
{
    attr->reserved = 0;
}

tt_u32_t tt_zip_find(IN tt_zip_t *z, IN const tt_char_t *name, IN tt_u32_t flag)
{
    zip_int64_t i;

    TT_ASSERT(z != NULL);
    TT_ASSERT(name != NULL);

    i = zip_name_locate(z, name, flag);
    if ((i >= 0) && (i <= 0x7FFFFFFF)) {
        return (tt_u32_t)i;
    } else {
        return TT_POS_NULL;
    }
}
