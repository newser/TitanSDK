/* Licensed to the Apache Software Foundation (ASF) under one or more
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
@file tt_file_system_aio_cb.h
@brief async file system io callback

this file defines async file sysetm io callback
*/

#ifndef __TT_FILE_SYSTEM_AIO_CB__
#define __TT_FILE_SYSTEM_AIO_CB__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_result_t result;
    void *cb_param;
} tt_faioctx_t;

typedef void (*tt_on_fcreate_t)(IN const tt_char_t *path,
                                IN tt_u32_t flag,
                                IN tt_file_attr_t *attr,
                                IN tt_faioctx_t *aioctx);

typedef void (*tt_on_fremove_t)(IN const tt_char_t *path,
                                IN tt_faioctx_t *aioctx);

typedef void (*tt_on_fopen_t)(IN tt_file_t *file,
                              IN const tt_char_t *path,
                              IN tt_u32_t flag,
                              IN tt_file_attr_t *attr,
                              IN tt_faioctx_t *aioctx);

typedef void (*tt_on_fclose_t)(IN tt_file_t *file, IN tt_faioctx_t *aioctx);

typedef void (*tt_on_fseek_t)(IN tt_file_t *file,
                              IN tt_u32_t whence,
                              IN tt_s64_t distance,
                              IN tt_faioctx_t *aioctx,
                              IN tt_u64_t position);

/**
@return(by aioctx->result)
- TT_SUCCESS if any datain buffer has been written
- TT_FAIL if no data has been written due to some error
*/
typedef void (*tt_on_fwrite_t)(IN tt_file_t *file,
                               IN tt_blob_t *blob_array,
                               IN tt_u32_t blob_num,
                               IN tt_u64_t position,
                               IN tt_faioctx_t *aioctx,
                               IN tt_u32_t write_len);

/**
@return(by aioctx->result)
- TT_SUCCESS if any data has been read and filled to buffer
- TT_FAIL if no data has been read due to some error
- TT_END if no data has been read due to reaching end of file
*/
typedef void (*tt_on_fread_t)(IN tt_file_t *file,
                              IN tt_blob_t *blob_array,
                              IN tt_u32_t blob_num,
                              IN tt_u64_t position,
                              IN tt_faioctx_t *aioctx,
                              IN tt_u32_t read_len);

typedef void (*tt_on_dcreate_t)(IN const tt_char_t *path,
                                IN tt_dir_attr_t *attr,
                                IN tt_faioctx_t *aioctx);

typedef void (*tt_on_dremove_t)(IN const tt_char_t *path,
                                IN tt_faioctx_t *aioctx);

typedef void (*tt_on_dopen_t)(IN tt_dir_t *dir,
                              IN const tt_char_t *path,
                              IN tt_dir_attr_t *attr,
                              IN tt_faioctx_t *aioctx);

typedef void (*tt_on_dclose_t)(IN tt_dir_t *dir, IN tt_faioctx_t *aioctx);

typedef void (*tt_on_dread_t)(IN tt_dir_t *dir,
                              IN tt_u32_t flag,
                              IN tt_dir_entry_t *dentry_array,
                              IN tt_u32_t dentry_num,
                              IN tt_faioctx_t *aioctx);

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_FILE_SYSTEM_AIO_CB__ */
