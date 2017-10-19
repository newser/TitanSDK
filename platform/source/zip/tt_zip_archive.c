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

#include <zip/tt_zip_archive.h>

#include <misc/tt_assert.h>
#include <zip/tt_zip_source.h>

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

tt_result_t tt_ziparc_create(IN tt_ziparc_t *za,
                             IN tt_zipsrc_t *zsrc,
                             IN tt_u32_t flag,
                             IN OPT tt_ziparc_attr_t *attr)
{
    tt_ziparc_attr_t __attr;
    zip_error_t zerr;

    TT_ASSERT(za != NULL);
    TT_ASSERT(zsrc != NULL);

    if (attr == NULL) {
        tt_ziparc_attr_default(&__attr);
        attr = &__attr;
    }

    za->z = zip_open_from_source(zsrc, flag, &zerr);
    if (za->z == NULL) {
        TT_ERROR("fail to create zip arc: %s", zip_error_strerror(&zerr));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_ziparc_destroy(IN tt_ziparc_t *za, IN tt_bool_t flush)
{
    if (flush) {
        if (zip_close(za->z) != 0) {
            TT_ERROR("fail to flush zip arc: %s", tt_ziparc_strerror(za));
        }
    } else {
        zip_discard(za->z);
    }
}

void tt_ziparc_attr_default(IN tt_ziparc_attr_t *attr)
{
    attr->reserved = 0;
}
