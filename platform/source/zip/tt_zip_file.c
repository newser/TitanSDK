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

#include <zip/tt_zip_file.h>

#include <algorithm/tt_algorithm_def.h>
#include <misc/tt_assert.h>
#include <zip/tt_zip_archive.h>

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

tt_u32_t tt_zipfile_find(IN tt_ziparc_t *za,
                         IN const tt_char_t *name,
                         IN tt_u32_t flag)
{
    zip_int64_t i;

    TT_ASSERT(za != NULL);
    TT_ASSERT(name != NULL);

    i = zip_name_locate(za->z, name, flag);
    if ((i >= 0) && (i <= 0x7FFFFFFF)) {
        return (tt_u32_t)i;
    } else {
        return TT_POS_NULL;
    }
}

tt_result_t tt_zipfile_open(IN tt_zipfile_t *zf,
                            IN tt_ziparc_t *za,
                            IN const tt_char_t *name,
                            IN tt_u32_t flag,
                            IN OPT tt_zipfile_attr_t *attr)
{
    tt_zipfile_attr_t __attr;

    TT_ASSERT(zf != NULL);
    TT_ASSERT(za != NULL);
    TT_ASSERT(name != NULL);

    if (attr == NULL) {
        tt_zipfile_attr_default(&__attr);
        attr = &__attr;
    }

    if (attr->password != NULL) {
        zf->f = zip_fopen(za->z, name, flag);
    } else {
        zf->f = zip_fopen_encrypted(za->z, name, flag, attr->password);
    }
    if (zf->f == NULL) {
        TT_ERROR("fail to open zip file[%s]: ", name, tt_ziparc_strerror(za));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_zipfile_open_index(IN tt_zipfile_t *zf,
                                  IN tt_ziparc_t *za,
                                  IN tt_u32_t index,
                                  IN tt_u32_t flag,
                                  IN OPT tt_zipfile_attr_t *attr)
{
    tt_zipfile_attr_t __attr;

    TT_ASSERT(zf != NULL);
    TT_ASSERT(za != NULL);
    TT_ASSERT(index != TT_POS_NULL);

    if (attr == NULL) {
        tt_zipfile_attr_default(&__attr);
        attr = &__attr;
    }

    if (attr->password != NULL) {
        zf->f = zip_fopen_index(za->z, index, flag);
    } else {
        zf->f = zip_fopen_index_encrypted(za->z, index, flag, attr->password);
    }
    if (zf->f == NULL) {
        TT_ERROR("fail to open zip file[%d]: ", index, tt_ziparc_strerror(za));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_zipfile_close(IN tt_zipfile_t *zf, IN tt_bool_t flush)
{
    TT_ASSERT(zf != NULL);

    zip_fclose(zf->f);
}

void tt_zipfile_attr_default(IN tt_zipfile_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->password = NULL;
}
