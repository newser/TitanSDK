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
@file tt_file_system_native.h
@brief file system native APIs

this file implements file system APIs at system level.
*/

#ifndef __TT_FILE_SYSTEM_NATIVE__
#define __TT_FILE_SYSTEM_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <event/tt_event_base.h>
#include <event/tt_thread_event.h>

#include <tt_sys_error.h>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define __INVALID_FD -1

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_file_attr_s;
struct tt_fs_dir_attr_s;
struct tt_dir_entry_s;

typedef struct
{
    int fd;

    // ========================================
    // below are for aio usage
    // ========================================

    // tt_u32_t ev_mark;
    struct tt_evcenter_s *evc;
    tt_ev_t *aio_ev;
    tt_list_t aio_q;
    tt_bool_t closing : 1;
} tt_file_ntv_t;

typedef struct
{
    DIR *dirp;

    // ========================================
    // below are for aio usage
    // ========================================

    // tt_u32_t ev_mark;
    struct tt_evcenter_s *evc;
    tt_ev_t *aio_ev;
    tt_list_t aio_q;
    tt_bool_t closing : 1;
} tt_dir_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_fs_component_init_ntv();

// - should call tt_fclose_portlayer(file) later
// - flag is ignored if file is null
extern tt_result_t tt_fcreate_ntv(IN const tt_char_t *path,
                                  IN tt_u32_t flag,
                                  IN struct tt_file_attr_s *attr);

// may fail due to permission
tt_inline tt_result_t tt_fremove_ntv(IN const tt_char_t *path)
{
    if (unlink(path) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to remove %s", path);
        return TT_FAIL;
    }
}

// this function can not open directory
extern tt_result_t tt_fopen_ntv(OUT tt_file_ntv_t *file,
                                IN const tt_char_t *path,
                                IN tt_u32_t flag,
                                IN struct tt_file_attr_s *attr);
/*
 * "flag" for tt_fopen_portlayer
 */
#define TT_FO_RDONLY_NTV (1 << 0)
#define TT_FO_WRONLY_NTV (1 << 1)
#define TT_FO_RDWR_NTV (TT_FO_RDONLY_NTV | TT_FO_WRONLY_NTV)
#define TT_FO_APPEND_NTV (1 << 3)
#define TT_FO_CREAT_NTV (1 << 4)
#define TT_FO_EXCL_NTV (1 << 5)
#define TT_FO_TRUNC_NTV (1 << 6)
#define TT_FO_TEMPORARY_NTV (1 << 7)
#define TT_FO_SEQUENTIAL_NTV (1 << 9)
#define TT_FO_RANDOM_NTV (1 << 10)
// highest 8 bits are reserved for internal usage
#define TT_FO_INTERNAL_AIO_ENABLE (1 << 24)

extern tt_result_t tt_fclose_ntv(IN tt_file_ntv_t *file);

extern tt_result_t tt_fseek_ntv(IN tt_file_ntv_t *file,
                                IN tt_u32_t whence,
                                IN tt_s64_t distance,
                                OUT tt_u64_t *position);
// whence
#define TT_FPOS_BEGIN_NTV (0)
#define TT_FPOS_CUR_NTV (1)
#define TT_FPOS_END_NTV (2)

// avoid calling fread and fwrite by multi thread at same time
tt_inline tt_result_t tt_fread_ntv(IN tt_file_ntv_t *file,
                                   OUT tt_u8_t *buf,
                                   IN tt_u32_t buf_len,
                                   OUT tt_u32_t *read_len)
{
    ssize_t __read_len;

    __read_len = read(file->fd, buf, buf_len);
    if (__read_len > 0) {
        if (read_len != NULL) {
            *read_len = (tt_u32_t)__read_len;
        }
        return TT_SUCCESS;
    } else if (__read_len == 0) {
        return TT_END;
    } else {
        TT_ERROR_NTV("read fail");
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_fwrite_ntv(IN tt_file_ntv_t *file,
                                    IN tt_u8_t *buf,
                                    IN tt_u32_t buf_len,
                                    OUT tt_u32_t *write_len)
{
    ssize_t __write_len;

    __write_len = write(file->fd, buf, buf_len);
    if (__write_len >= 0) {
        if (write_len != NULL) {
            *write_len = (tt_u32_t)__write_len;
        }
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("write fail");
        return TT_FAIL;
    }
}

extern tt_result_t tt_dcreate_ntv(IN const tt_char_t *path,
                                  IN struct tt_fs_dir_attr_s *attr);

extern tt_result_t tt_dremove_ntv(IN const tt_char_t *path, IN tt_u32_t flag);
/*
 * "flag" for tt_dremove_portlayer
 */
#define TT_DRM_RECURSIVE_NTV (1 << 0)

// need call tt_dclose_portlayer(dir) later
extern tt_result_t tt_dopen_ntv(OUT tt_dir_ntv_t *dir,
                                IN const tt_char_t *path,
                                IN struct tt_fs_dir_attr_s *attr);

extern tt_result_t tt_dclose_ntv(OUT tt_dir_ntv_t *dir);


// return TT_END if reaching end
extern tt_result_t tt_dread_ntv(IN tt_dir_ntv_t *dir,
                                IN tt_u32_t flag,
                                OUT struct tt_dir_entry_s *dentry);
#define TT_DREAD_TYPE_NTV (1 << 0)

#endif
