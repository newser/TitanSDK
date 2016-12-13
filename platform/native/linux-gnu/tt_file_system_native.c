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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_file_system_native.h>

#include <io/tt_file_system.h>
#include <misc/tt_assert.h>
#include <os/tt_mutex.h>

#include <tt_cstd_api.h>
#include <tt_util_native.h>

#include <fts.h>
#include <sys/stat.h>
#include <sys/types.h>

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

static tt_result_t __fs_dopen(OUT tt_dir_ntv_t *dir,
                              IN const tt_char_t *path,
                              IN struct tt_fs_dir_attr_s *attr);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_fs_component_init_ntv()
{
    return TT_SUCCESS;
}

tt_result_t tt_fcreate_ntv(IN const tt_char_t *path,
                           IN tt_u32_t flag,
                           IN struct tt_file_attr_s *attr)
{
    tt_result_t result = TT_FAIL;
    tt_file_ntv_t __created;

    // should create exlusively
    flag &= ~(TT_FO_CREAT_NTV | TT_FO_EXCL_NTV | TT_FO_TRUNC_NTV);
    flag |= (TT_FO_CREAT_NTV | TT_FO_EXCL_NTV);

    result = tt_fopen_ntv(&__created, path, flag, attr);
    if (TT_OK(result)) {
        tt_fclose_ntv(&__created);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_fopen_ntv(OUT tt_file_ntv_t *file,
                         IN const tt_char_t *path,
                         IN tt_u32_t flag,
                         IN struct tt_file_attr_s *attr)
{
    int fd = __INVALID_FD;
    int __flag = 0;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    // actually mode should be generated from attr

    // ========================================
    // map parameters
    // ========================================

    // map:
    //  - TT_FO_RDONLY_NTV
    //  - TT_FO_WRONLY_NTV
    //  - TT_FO_RDWR_NTV
    switch (flag & (TT_FO_RDONLY_NTV | TT_FO_WRONLY_NTV | TT_FO_RDWR_NTV)) {
        case TT_FO_RDONLY_NTV: {
            __flag |= O_RDONLY;
        } break;
        case TT_FO_WRONLY_NTV: {
            __flag |= O_WRONLY;
        } break;
        case TT_FO_RDWR_NTV: {
            __flag |= O_RDWR;
        } break;
        default: {
#if 0
            // other compositions are invalid
            TT_ERROR("invalid composition: \n" \
                     "  [TT_FO_RDONLY_NTV: %d]\n" \
                     "  [TT_FO_WRONLY_NTV: %d]\n" \
                     "  [TT_FO_RDWR_NTV: %d]",
                     (flag & TT_FO_RDONLY_NTV ? 1 : 0),
                     (flag & TT_FO_WRONLY_NTV ? 1 : 0),
                     (flag & TT_FO_RDWR_NTV ? 1 : 0));
            return TT_FAIL;
#endif
        } break;
    }

    // map TT_FO_APPEND_NTV
    if (flag & TT_FO_APPEND_NTV) {
        __flag |= O_APPEND;
    }

    // map:
    //  - TT_FO_CREAT_NTV
    //  - TT_FO_EXCL_NTV
    //  - TT_FO_TRUNC_NTV
    switch (flag & (TT_FO_CREAT_NTV | TT_FO_EXCL_NTV | TT_FO_TRUNC_NTV)) {
        case 0: {
            // if exist, open it
            // if not exist, fail
        } break;
        case TT_FO_CREAT_NTV: {
            // if exist, open it
            // if not exist, create it
            __flag |= O_CREAT;
        } break;
        case TT_FO_CREAT_NTV | TT_FO_EXCL_NTV:
        case TT_FO_CREAT_NTV | TT_FO_EXCL_NTV | TT_FO_TRUNC_NTV: {
            // if exist, fail
            // if not exist, create it
            __flag |= O_CREAT | O_EXCL;
        } break;
        case TT_FO_TRUNC_NTV:
        case TT_FO_CREAT_NTV | TT_FO_TRUNC_NTV: {
            // if exist, truncate it
            // if not exist, create it
            __flag |= O_CREAT | O_TRUNC;
        } break;
        default: {
            // other compositions are invalid
            TT_ERROR(
                "invalid composition: \n"
                "  [TT_FO_CREAT_NTV: %d]\n"
                "  [TT_FO_EXCL_NTV: %d]\n"
                "  [TT_FO_TRUNC_NTV: %d]",
                (flag & TT_FO_CREAT_NTV ? 1 : 0),
                (flag & TT_FO_EXCL_NTV ? 1 : 0),
                (flag & TT_FO_TRUNC_NTV ? 1 : 0));
            return TT_FAIL;
        } break;
    }

    // map TT_FO_TEMPORARY_NTV
    if (flag & TT_FO_TEMPORARY_NTV) {
    }

    // map:
    //  - TT_FO_SEQUENTIAL_NTV
    //  - TT_FO_RANDOM_NTV
    switch (flag & (TT_FO_SEQUENTIAL_NTV | TT_FO_RANDOM_NTV)) {
        case 0: {
        } break;
        case TT_FO_SEQUENTIAL_NTV: {
        } break;
        case TT_FO_RANDOM_NTV: {
        } break;
        default: {
            // other compositions are invalid
            TT_ERROR(
                "invalid composition: \n"
                "  [TT_FO_SEQUENTIAL_NTV: %d]\n"
                "  [TT_FO_RANDOM_NTV: %d]\n",
                (flag & TT_FO_SEQUENTIAL_NTV ? 1 : 0),
                (flag & TT_FO_RANDOM_NTV ? 1 : 0));
            return TT_FAIL;
        } break;
    }

// ========================================
// create file
// ========================================

__retry:
    fd = open(path, __flag, mode);
    if (fd < 0) {
        if (errno == EINTR) {
            goto __retry;
        } else {
            TT_ERROR_NTV("fail to create file: %s", path);
            return TT_FAIL;
        }
    }

    // set close-on-exec
    if (((__flag = fcntl(fd, F_GETFD, 0)) == -1) ||
        (fcntl(fd, F_SETFD, __flag | FD_CLOEXEC) == -1)) {
        TT_ERROR_NTV("fail to set file close-on-exec");

        __RETRY_IF_EINTR(close(fd) != 0);
        return TT_FAIL;
    }

    file->fd = fd;

    file->evc = NULL;
    file->aio_ev = NULL;
    tt_list_init(&file->aio_q);
    file->closing = TT_FALSE;

    return TT_SUCCESS;
}

tt_result_t tt_fclose_ntv(IN tt_file_ntv_t *file)
{
    TT_ASSERT(file->fd != __INVALID_FD);

__retry:
    if (close(file->fd) == 0) {
        file->fd = -1;
        return TT_SUCCESS;
    } else {
        if (errno == EINTR) {
            goto __retry;
        } else {
            TT_ERROR_NTV("fail to close system file");
            return TT_FAIL;
        }
    }
}

tt_result_t tt_fseek_ntv(IN tt_file_ntv_t *file,
                         IN tt_u32_t whence,
                         IN tt_s64_t distance,
                         OUT tt_u64_t *position)
{
    off_t __offset = distance;
    int __whence;

    off_t new_position;

    TT_ASSERT(file->fd != __INVALID_FD);

    if (whence == TT_FPOS_BEGIN_NTV) {
        __whence = SEEK_SET;
    } else if (whence == TT_FPOS_CUR_NTV) {
        __whence = SEEK_CUR;
    } else if (whence == TT_FPOS_END_NTV) {
        __whence = SEEK_END;
    } else {
        TT_ERROR("invalid whence: %d", whence);
        return TT_FAIL;
    }

    new_position = lseek(file->fd, __offset, __whence);
    if (new_position < 0) {
        TT_ERROR_NTV("fail to set file pointer");
        return TT_FAIL;
    }

    if (position != NULL) {
        *position = new_position;
    }
    return TT_SUCCESS;
}

tt_result_t tt_dcreate_ntv(IN const tt_char_t *path,
                           IN struct tt_fs_dir_attr_s *attr)
{
    mode_t mode =
        S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    // without 'x', directory is not explorable

    if (mkdir(path, mode) != 0) {
        TT_ERROR_NTV("fail to create directory: %s", path);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_dremove_ntv(IN const tt_char_t *path, IN tt_u32_t flag)
{
    if (flag & TT_DRM_RECURSIVE_NTV) {
        tt_char_t *__paths[2] = {(tt_char_t *)path, NULL};
        FTS *__fts = NULL;
        FTSENT *__ftse = NULL;
        tt_result_t result = TT_SUCCESS;

    // open hierarchy
    __retry1:
        __fts =
            fts_open(__paths, FTS_NOCHDIR | FTS_NOSTAT | FTS_PHYSICAL, NULL);
        if (__fts == NULL) {
            if (errno == EINTR) {
                goto __retry1;
            } else {
                TT_ERROR_NTV("fail to open fts: %s", path);
                return TT_FAIL;
            }
        }

        // explore hierarchy
        while (TT_OK(result)) {
        __retry2:
            __ftse = fts_read(__fts);
            if (__ftse == NULL) {
                if (errno != 0) {
                    if (errno == EINTR) {
                        goto __retry2;
                    } else {
                        TT_ERROR_NTV("fail to read fts");
                        result = TT_FAIL;
                    }
                }
                break;
            }

            switch (__ftse->fts_info) {
                case FTS_DEFAULT:
                case FTS_F:
                case FTS_NSOK:
                case FTS_SL:
                case FTS_SLNONE: {
                    // remove file
                    if (unlink(__ftse->fts_accpath) != 0) {
                        TT_ERROR_NTV("fail to remove file[%s]",
                                     __ftse->fts_accpath);
                        result = TT_FAIL;
                        continue;
                        // continue would break loop
                    }
                } break;
                case FTS_DP: {
                    // can remove directory now
                    if (rmdir(__ftse->fts_accpath) != 0) {
                        TT_ERROR_NTV("fail to remove directory[%s]",
                                     __ftse->fts_accpath);
                        return TT_FAIL;
                    }
                } break;
                case FTS_D: {
                    // something to ignore
                } break;
                default: {
                    // something unexpected
                    TT_ERROR("expected fts info: %d", __ftse->fts_info);
                    result = TT_FAIL;
                    continue;
                    // continue would break loop
                } break;
            }
        }

    // close hierarchy
    __retry3:
        if (fts_close(__fts) != 0) {
            if (errno == EINTR) {
                goto __retry3;
            } else {
                TT_ERROR_NTV("fail to close fts");
                return TT_FAIL;
            }
        }

        return result;
    } else {
        if (rmdir(path) != 0) {
            TT_ERROR_NTV("fail to remove directory[%s]", path);
            return TT_FAIL;
        }

        return TT_SUCCESS;
    }
}

tt_result_t tt_dopen_ntv(OUT tt_dir_ntv_t *dir,
                         IN const tt_char_t *path,
                         IN struct tt_fs_dir_attr_s *attr)
{
    return __fs_dopen(dir, path, attr);
}

tt_result_t __fs_dopen(OUT tt_dir_ntv_t *dir,
                       IN const tt_char_t *path,
                       IN struct tt_fs_dir_attr_s *attr)
{
    DIR *p;

    p = opendir(path);
    if (p == NULL) {
        TT_ERROR_NTV("opendir fail");
        return TT_FAIL;
    }

    dir->dirp = p;

    dir->evc = NULL;
    dir->aio_ev = NULL;
    tt_list_init(&dir->aio_q);
    dir->closing = TT_FALSE;

    return TT_SUCCESS;
}

tt_result_t tt_dclose_ntv(OUT tt_dir_ntv_t *dir)
{
    TT_ASSERT(dir->dirp != NULL);

    if (closedir(dir->dirp) != 0) {
        TT_ERROR_NTV("fail to close diretory");
        return TT_FAIL;
    }

    dir->dirp = NULL;
    return TT_SUCCESS;
}

tt_result_t tt_dread_ntv(IN tt_dir_ntv_t *dir,
                         IN tt_u32_t flag,
                         OUT tt_dir_entry_t *dentry)
{
    struct dirent entry;
    struct dirent *result = NULL;

    TT_ASSERT(dir->dirp != NULL);

    if (readdir_r(dir->dirp, &entry, &result) != 0) {
        TT_ERROR_NTV("fail to read dir");
        return TT_FAIL;
    }

    if (result == NULL) {
        return TT_END;
    }

    strncpy(dentry->name, entry.d_name, TT_MAX_FILE_NAME_LEN);

    switch (entry.d_type) {
        case DT_DIR: {
            dentry->type = TT_FSTYPE_DIR;
        } break;
        case DT_UNKNOWN: {
            dentry->type = TT_FSTYPE_UNKNOWN;
        } break;
        default: {
            dentry->type = TT_FSTYPE_FILE;
        } break;
    }

    return TT_SUCCESS;
}
