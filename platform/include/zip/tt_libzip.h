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
@file tt_libzip.h
@brief libzip APIs

this file specifies libzip interfaces
*/

#ifndef __TT_LIBZIP__
#define __TT_LIBZIP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <zip.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_ZF_NOCASE ZIP_FL_NOCASE
#define TT_ZF_NODIR ZIP_FL_NODIR
#define TT_ZF_COMPRESSED ZIP_FL_COMPRESSED
#define TT_ZF_UNCHANGED ZIP_FL_UNCHANGED
#define TT_ZF_RECOMPRESS ZIP_FL_RECOMPRESS
#define TT_ZF_ENCRYPTED ZIP_FL_ENCRYPTED
#define TT_ZF_ENC_GUESS ZIP_FL_ENC_GUESS
#define TT_ZF_ENC_RAW ZIP_FL_ENC_RAW
#define TT_ZF_ENC_STRICT ZIP_FL_ENC_STRICT
#define TT_ZF_LOCAL ZIP_FL_LOCAL
#define TT_ZF_CENTRAL ZIP_FL_CENTRAL
#define TT_ZF_ENC_UTF_8 ZIP_FL_ENC_UTF_8
#define TT_ZF_ENC_CP437 ZIP_FL_ENC_CP437
#define TT_ZF_OVERWRITE ZIP_FL_OVERWRITE

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef zip_source_t tt_zipsrc_t;

typedef zip_t tt_zip_t;

typedef enum {
    TT_ZIP_CIPHER_NONE = ZIP_EM_NONE,
    TT_ZIP_CIPHER_AES128 = ZIP_EM_AES_128,
    TT_ZIP_CIPHER_AES192 = ZIP_EM_AES_192,
    TT_ZIP_CIPHER_AES256 = ZIP_EM_AES_256,
} tt_zip_cipher_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_LIBZIP__ */
