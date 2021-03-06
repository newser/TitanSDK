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
@file tt_file_system_native.h
@brief file system native APIs

this file implements file system APIs at system level.
*/

#ifndef __TT_FILE_SYSTEM_NATIVE__
#define __TT_FILE_SYSTEM_NATIVE__

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

struct tt_file_attr_s;
struct tt_dir_attr_s;
struct tt_dirent_s;
struct tt_io_ev_s;

typedef struct
{
    tt_s64_t offset;
    HANDLE hf;
    tt_bool_t append : 1;
} tt_file_ntv_t;

typedef struct
{
    HANDLE hd;
    tt_result_t next_result;
    WIN32_FIND_DATAW find_data;
} tt_dir_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_fs_component_init_ntv();

extern void tt_fs_component_exit_ntv();

extern void tt_fs_status_dump_ntv(IN tt_u32_t flag);

// - should call tt_fclose_portlayer(file) later
// - flag is ignored if file is null
extern tt_result_t tt_fcreate_ntv(IN const tt_char_t *path,
                                  IN struct tt_file_attr_s *attr);

extern tt_result_t tt_fremove_ntv(IN const tt_char_t *path);

extern tt_result_t tt_fopen_ntv(IN tt_file_ntv_t *file,
                                IN const tt_char_t *path,
                                IN tt_u32_t flag,
                                IN struct tt_file_attr_s *attr);

extern void tt_fclose_ntv(IN tt_file_ntv_t *file);

extern tt_result_t tt_fseek_ntv(IN tt_file_ntv_t *file,
                                IN tt_u32_t whence,
                                IN tt_s64_t offset,
                                OUT tt_u64_t *location);

extern tt_result_t tt_fread_ntv(IN tt_file_ntv_t *file,
                                OUT tt_u8_t *buf,
                                IN tt_u32_t buf_len,
                                OUT tt_u32_t *read_len);

extern tt_result_t tt_fwrite_ntv(IN tt_file_ntv_t *file,
                                 IN tt_u8_t *buf,
                                 IN tt_u32_t buf_len,
                                 OUT tt_u32_t *write_len);

extern tt_result_t tt_fstat_ntv(IN tt_file_ntv_t *file,
                                OUT struct tt_fstat_s *fstat);

extern tt_result_t tt_ftrunc_ntv(IN tt_file_ntv_t *file, IN tt_u64_t len);

extern tt_result_t tt_fcopy_ntv(IN const tt_char_t *dst,
                                IN const tt_char_t *src,
                                IN tt_u32_t flag);

extern tt_result_t tt_fsync_ntv(IN tt_file_ntv_t *file);

tt_inline tt_result_t tt_fdatasync_ntv(IN tt_file_ntv_t *file)
{
    // macos does not have fdatasync
    return tt_fsync_ntv(file);
}

extern tt_result_t tt_futime_ntv(IN tt_file_ntv_t *file,
                                 IN struct tt_date_s *accessed,
                                 IN struct tt_date_s *modified);

extern tt_result_t tt_dcreate_ntv(IN const tt_char_t *path,
                                  IN struct tt_dir_attr_s *attr);

extern tt_result_t tt_dremove_ntv(IN const tt_char_t *path);

extern tt_result_t tt_dopen_ntv(OUT tt_dir_ntv_t *dir,
                                IN const tt_char_t *path,
                                IN struct tt_dir_attr_s *attr);

extern void tt_dclose_ntv(OUT tt_dir_ntv_t *dir);

// return TT_E_END if reaching end
extern tt_result_t tt_dread_ntv(IN tt_dir_ntv_t *dir,
                                OUT struct tt_dirent_s *entry);

extern tt_result_t tt_dcopy_ntv(IN const tt_char_t *dst,
                                IN const tt_char_t *src,
                                IN tt_u32_t flag);

extern tt_bool_t tt_fs_exist_ntv(IN const tt_char_t *path);

extern tt_result_t tt_fs_rename_ntv(IN const tt_char_t *from,
                                    IN const tt_char_t *to);

extern tt_result_t tt_fs_link_ntv(IN const tt_char_t *path,
                                  IN const tt_char_t *link);

extern tt_result_t tt_fs_symlink_ntv(IN const tt_char_t *path,
                                     IN const tt_char_t *link);

extern tt_result_t tt_fs_readlink_ntv(IN const tt_char_t *link,
                                      OUT tt_char_t *path,
                                      IN tt_u32_t len);

extern tt_result_t tt_fs_realpath_ntv(IN const tt_char_t *path,
                                      OUT tt_char_t *resolved,
                                      IN tt_u32_t len);

extern void tt_fs_worker_io(IN struct tt_io_ev_s *io_ev);

extern tt_bool_t tt_fs_poller_io(IN struct tt_io_ev_s *io_ev);

#endif
