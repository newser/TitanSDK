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
@brief zip file APIs

this file specifies zip file interfaces
*/

#ifndef __TT_ZIP_FILE__
#define __TT_ZIP_FILE__

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

typedef zip_file_t tt_zipfile_t;

typedef struct
{
    const tt_char_t *password;
} tt_zipfile_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_zipfile_t *tt_zipfile_open(IN tt_zip_t *z,
                                        IN const tt_char_t *name,
                                        IN tt_u32_t flag,
                                        IN OPT tt_zipfile_attr_t *attr);

tt_export tt_zipfile_t *tt_zipfile_open_index(IN tt_zip_t *z,
                                              IN tt_u32_t index,
                                              IN tt_u32_t flag,
                                              IN OPT tt_zipfile_attr_t *attr);

tt_export void tt_zipfile_close(IN tt_zipfile_t *zf);

tt_export void tt_zipfile_attr_default(IN tt_zipfile_attr_t *attr);

tt_export tt_result_t tt_zipfile_read(IN tt_zipfile_t *zf,
                                      OUT tt_u8_t *buf,
                                      IN tt_u32_t len,
                                      OUT tt_u32_t *read_len);

#endif /* __TT_ZIP_FILE__ */
