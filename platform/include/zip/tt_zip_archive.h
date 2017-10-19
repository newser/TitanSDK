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
@file tt_zip_archive.h
@brief zip archive APIs

this file specifies zip archive interfaces
*/

#ifndef __TT_ZIP_ARCHIVE__
#define __TT_ZIP_ARCHIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>
#include <zip/tt_libzip.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_ziparc_s
{
    zip_t *z;
} tt_ziparc_t;

typedef struct
{
    tt_u32_t reserved;
} tt_ziparc_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_ziparc_create(IN tt_ziparc_t *za,
                                       IN tt_zipsrc_t *zsrc,
                                       IN tt_u32_t flag,
                                       IN OPT tt_ziparc_attr_t *attr);
#define TT_ZA_CREAT ZIP_CREATE
#define TT_ZA_EXCL ZIP_EXCL
#define TT_ZA_CHECKCONS ZIP_CHECKCONS
#define TT_ZA_TRUNCATE ZIP_TRUNCATE
#define TT_ZA_RDONLY ZIP_RDONLY

tt_export void tt_ziparc_destroy(IN tt_ziparc_t *za, IN tt_bool_t flush);

tt_export void tt_ziparc_attr_default(IN tt_ziparc_attr_t *attr);

tt_inline const tt_char_t *tt_ziparc_strerror(IN tt_ziparc_t *za)
{
    return zip_error_strerror(zip_get_error(za->z));
}

#endif /* __TT_ZIP_ARCHIVE__ */
