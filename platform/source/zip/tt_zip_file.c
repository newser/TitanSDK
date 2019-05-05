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
#include <misc/tt_util.h>
#include <zip/tt_zip.h>

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

tt_zipfile_t *tt_zipfile_open(IN tt_zip_t *z, IN const tt_char_t *name,
                              IN tt_u32_t flag, IN OPT tt_zipfile_attr_t *attr)
{
    tt_zipfile_attr_t __attr;
    zip_file_t *zf;

    TT_ASSERT(z != NULL);
    TT_ASSERT(name != NULL);

    if (attr == NULL) {
        tt_zipfile_attr_default(&__attr);
        attr = &__attr;
    }

    if (attr->password != NULL) {
        zf = zip_fopen_encrypted(z, name, flag, attr->password);
    } else {
        zf = zip_fopen(z, name, flag);
    }
    if (zf == NULL) {
        TT_ERROR("fail to open zip file[%s]: ", name, tt_zip_strerror(z));
        return NULL;
    }

    return zf;
}

tt_zipfile_t *tt_zipfile_open_index(IN tt_zip_t *z, IN tt_u32_t index,
                                    IN tt_u32_t flag,
                                    IN OPT tt_zipfile_attr_t *attr)
{
    tt_zipfile_attr_t __attr;
    zip_file_t *zf;

    TT_ASSERT(z != NULL);
    TT_ASSERT(index != TT_POS_NULL);

    if (attr == NULL) {
        tt_zipfile_attr_default(&__attr);
        attr = &__attr;
    }

    if (attr->password != NULL) {
        zf = zip_fopen_index_encrypted(z, index, flag, attr->password);
    } else {
        zf = zip_fopen_index(z, index, flag);
    }
    if (zf == NULL) {
        TT_ERROR("fail to open zip file[%d]: ", index, tt_zip_strerror(z));
        return NULL;
    }

    return zf;
}

void tt_zipfile_close(IN tt_zipfile_t *zf)
{
    TT_ASSERT(zf != NULL);

    zip_fclose(zf);
}

void tt_zipfile_attr_default(IN tt_zipfile_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->password = NULL;
}

tt_result_t tt_zipfile_read(IN tt_zipfile_t *zf, OUT tt_u8_t *buf,
                            IN tt_u32_t len, OUT tt_u32_t *read_len)
{
    zip_int64_t n = zip_fread(zf, buf, len);
    if (n > 0) {
        TT_SAFE_ASSIGN(read_len, (tt_u32_t)n);
        return TT_SUCCESS;
    } else if (n == 0) {
        TT_SAFE_ASSIGN(read_len, 0);
        return TT_E_END;
    } else {
        TT_ERROR("fail to read zip file");
        return TT_FAIL;
    }
}
