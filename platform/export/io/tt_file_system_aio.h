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
@file tt_file_system_aio.h
@brief file system socket io

this file defines async file system io APIs
*/

#ifndef __TT_FILE_SYSTEM_AIO__
#define __TT_FILE_SYSTEM_AIO__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_file_system_aio_native.h>

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

// all aio calls use "local evc", so they can only be used in
// event pollers

tt_inline tt_result_t tt_fcreate_async(IN const tt_char_t *path,
                                       IN tt_u32_t flag,
                                       IN tt_file_attr_t *attr,
                                       IN tt_on_fcreate_t on_fcreate,
                                       IN void *cb_param)
{
    return tt_fcreate_async_ntv(path, flag, attr, on_fcreate, cb_param);
}

tt_inline tt_result_t tt_fremove_async(IN const tt_char_t *path,
                                       IN tt_on_fremove_t on_fremove,
                                       IN void *cb_param)
{
    return tt_fremove_async_ntv(path, on_fremove, cb_param);
}

// - called more than once on same @ref file would lead to resource leakage
tt_inline tt_result_t tt_fopen_async(IN tt_file_t *file,
                                     IN const tt_char_t *path,
                                     IN tt_u32_t flag,
                                     IN tt_file_attr_t *attr,
                                     IN tt_on_fopen_t on_fopen,
                                     IN void *cb_param)
{
    return tt_fopen_async_ntv(file, path, flag, attr, on_fopen, cb_param);
}

// - setting @ref on_fclose to NULL means directly do closing, all pending
//   aios would be released(not called), note this way can not be used in
//   the evc in which the file is opened
// - called more than once on same @ref file lead to unpredictable result
// - behavior of aios submitted after this function are undefined
// - @ref on_fclose would be called when all aios submitted before this
//   function are finished
tt_inline tt_result_t tt_fclose_async(IN tt_file_t *file,
                                      IN tt_on_fclose_t on_fclose,
                                      IN void *cb_param)
{
    return tt_fclose_async_ntv(file, on_fclose, cb_param);
}

tt_inline tt_result_t tt_fseek_async(IN tt_file_t *file,
                                     IN tt_u32_t whence,
                                     IN tt_s64_t distance,
                                     IN tt_on_fseek_t on_fseek,
                                     IN void *cb_param)
{
    return tt_fseek_async_ntv(file, whence, distance, on_fseek, cb_param);
}

/**
@note
- @ref position is ignored if TT_FO_APPEND is specified
- negative @ref position indicates using internal file position
- if app submitted consecutive write aios to write 100/80/90 bytes
  but the second write aio only write 30 bytes due to some internal
  error, then the content of the file would be 100/30/90 bytes. to
  avoid this, submit one aio each time and check how many bytes are
  written and then determine the next action.
*/
tt_inline tt_result_t tt_fwrite_async(IN tt_file_t *file,
                                      IN tt_blob_t *blob_array,
                                      IN tt_u32_t blob_num,
                                      IN tt_u64_t position,
                                      IN tt_on_fwrite_t on_fwrite,
                                      IN void *cb_param)
{
    // try to write all data
    return tt_fwrite_async_ntv(file,
                               blob_array,
                               blob_num,
                               position,
                               on_fwrite,
                               cb_param);
}

/**
@note
- @ref position is ignored if TT_FO_APPEND is specified
- negative @ref position indicates using internal file position
- if passing a buffer with 1K space to read a file of 512 bytes
  size, then this function first return TT_SUCCESS and 512 bytes
  read through callback and then TT_END and 0 byte read
*/
tt_inline tt_result_t tt_fread_async(IN tt_file_t *file,
                                     IN tt_blob_t *blob_array,
                                     IN tt_u32_t blob_num,
                                     IN tt_u64_t position,
                                     IN tt_on_fread_t on_fread,
                                     IN void *cb_param)
{
    return tt_fread_async_ntv(file,
                              blob_array,
                              blob_num,
                              position,
                              on_fread,
                              cb_param);
}

tt_inline tt_result_t tt_dcreate_async(IN const tt_char_t *path,
                                       IN tt_dir_attr_t *attr,
                                       IN tt_on_dcreate_t on_dcreate,
                                       IN void *cb_param)
{
    return tt_dcreate_async_ntv(path, attr, on_dcreate, cb_param);
}

tt_inline tt_result_t tt_dremove_async(IN const tt_char_t *path,
                                       IN tt_u32_t flag,
                                       IN tt_on_dremove_t on_dremove,
                                       IN void *cb_param)
{
    return tt_dremove_async_ntv(path, flag, on_dremove, cb_param);
}

tt_inline tt_result_t tt_dopen_async(IN tt_dir_t *dir,
                                     IN const tt_char_t *path,
                                     IN tt_dir_attr_t *attr,
                                     IN tt_on_dopen_t on_dopen,
                                     IN void *cb_param)
{
    return tt_dopen_async_ntv(dir, path, attr, on_dopen, cb_param);
}

tt_inline tt_result_t tt_dclose_async(IN tt_dir_t *dir,
                                      IN tt_on_dclose_t on_dclose,
                                      IN void *cb_param)
{
    return tt_dclose_async_ntv(dir, on_dclose, cb_param);
}

tt_inline tt_result_t tt_dread_async(IN tt_dir_t *dir,
                                     IN tt_u32_t flag,
                                     IN tt_dir_entry_t *dentry_array,
                                     IN tt_u32_t dentry_num,
                                     IN tt_on_dread_t on_dread,
                                     IN void *cb_param)
{
    return tt_dread_async_ntv(dir,
                              flag,
                              dentry_array,
                              dentry_num,
                              on_dread,
                              cb_param);
}

#endif /* __TT_FILE_SYSTEM_AIO__ */
