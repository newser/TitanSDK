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
@file tt_file_system_aio_native.h
@brief file system socket io

this file defines async file system io APIs
*/

#ifndef __TT_FILE_SYSTEM_AIO_NATIVE__
#define __TT_FILE_SYSTEM_AIO_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_file_system.h>
#include <io/tt_file_system_aio_cb.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// define this macro to simulate failure of sys call randomly
//#define __SIMULATE_FS_AIO_FAIL

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_fcreate_async_ntv(IN const tt_char_t *path,
                                        IN tt_u32_t flag,
                                        IN tt_file_attr_t *attr,
                                        IN tt_on_fcreate_t on_fcreate,
                                        IN void *cb_param);

extern tt_result_t tt_fremove_async_ntv(IN const tt_char_t *path,
                                        IN tt_on_fremove_t on_fremove,
                                        IN void *cb_param);

extern tt_result_t tt_fopen_async_ntv(IN tt_file_t *file,
                                      IN const tt_char_t *path,
                                      IN tt_u32_t flag,
                                      IN tt_file_attr_t *attr,
                                      IN tt_on_fopen_t on_fopen,
                                      IN void *cb_param);

extern tt_result_t tt_fclose_async_ntv(IN tt_file_t *file,
                                       IN tt_on_fclose_t on_fclose,
                                       IN void *cb_param);

extern tt_result_t tt_fseek_async_ntv(IN tt_file_t *file,
                                      IN tt_u32_t whence,
                                      IN tt_s64_t distance,
                                      IN tt_on_fseek_t on_fseek,
                                      IN void *cb_param);

extern tt_result_t tt_fwrite_async_ntv(IN tt_file_t *file,
                                       IN tt_blob_t *blob_array,
                                       IN tt_u32_t blob_num,
                                       IN tt_u64_t position,
                                       IN tt_on_fwrite_t on_fwrite,
                                       IN void *cb_param);

extern tt_result_t tt_fread_async_ntv(IN tt_file_t *file,
                                      IN tt_blob_t *blob_array,
                                      IN tt_u32_t blob_num,
                                      IN tt_u64_t position,
                                      IN tt_on_fread_t on_fread,
                                      IN void *cb_param);

extern tt_result_t tt_file_tev_handler(IN struct tt_evpoller_s *evp,
                                       IN tt_ev_t *ev);

extern tt_result_t tt_dcreate_async_ntv(IN const tt_char_t *path,
                                        IN tt_dir_attr_t *attr,
                                        IN tt_on_dcreate_t on_dcreate,
                                        IN void *cb_param);

extern tt_result_t tt_dremove_async_ntv(IN const tt_char_t *path,
                                        IN tt_u32_t flag,
                                        IN tt_on_dremove_t on_dremove,
                                        IN void *cb_param);

extern tt_result_t tt_dopen_async_ntv(IN tt_dir_t *dir,
                                      IN const tt_char_t *path,
                                      IN tt_dir_attr_t *attr,
                                      IN tt_on_dopen_t on_dopen,
                                      IN void *cb_param);

extern tt_result_t tt_dclose_async_ntv(IN tt_dir_t *dir,
                                       IN tt_on_dclose_t on_dclose,
                                       IN void *cb_param);

extern tt_result_t tt_dread_async_ntv(IN tt_dir_t *dir,
                                      IN tt_u32_t flag,
                                      IN tt_dir_entry_t *dentry,
                                      IN tt_u32_t dentry_num,
                                      IN tt_on_dread_t on_dread,
                                      IN void *cb_param);

extern tt_result_t tt_dir_tev_handler(IN struct tt_evpoller_s *evp,
                                      IN tt_ev_t *ev);

#endif /* __TT_FILE_SYSTEM_AIO_NATIVE__ */
