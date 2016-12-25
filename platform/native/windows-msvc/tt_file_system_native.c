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

#include <event/tt_event_center.h>
#include <io/tt_file_system.h>
#include <memory/tt_memory_alloc.h>

#include <tt_cstd_api.h>
#include <tt_wchar.h>

#include <io.h>
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
                              IN wchar_t *path,
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
    tt_file_ntv_t created;
    tt_result_t result;

    flag &= ~(TT_FO_CREAT_NTV | TT_FO_EXCL_NTV | TT_FO_TRUNC_NTV);
    flag |= (TT_FO_CREAT_NTV | TT_FO_EXCL_NTV);

    result = tt_fopen_ntv(&created, path, flag, attr);
    if (TT_OK(result)) {
        tt_fclose_ntv(&created);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_fremove_ntv(IN const tt_char_t *path)
{
    wchar_t *w_path;
    BOOL ret;

    w_path = tt_wchar_create(path, NULL);
    if (w_path == NULL) {
        return TT_FAIL;
    }

    ret = DeleteFileW(w_path);
    if (!ret) {
        TT_ERROR_NTV("fail to delete file: %ls", w_path);
    }
    tt_wchar_destroy(w_path);

    return TT_COND(ret, TT_SUCCESS, TT_FAIL);
}

tt_result_t tt_fopen_ntv(OUT tt_file_ntv_t *file,
                         IN const tt_char_t *path,
                         IN tt_u32_t flag,
                         IN struct tt_file_attr_s *attr)
{
    DWORD dwDesiredAccess = 0;
    DWORD dwShareMode = 0;
    // LPSECURITY_ATTRIBUTES lpSecurityAttributes;
    DWORD dwCreationDisposition = 0;
    DWORD dwFlagsAndAttributes = 0;
    HANDLE hTemplateFile = NULL;

    int mode;
    HANDLE fh = INVALID_HANDLE_VALUE;

    wchar_t *w_path;

    // ========================================
    // map parameters
    // ========================================

    // map:
    //  - TT_FO_RDONLY_NTV
    //  - TT_FO_WRONLY_NTV
    //  - TT_FO_RDWR_NTV
    switch (flag & (TT_FO_RDONLY_NTV | TT_FO_WRONLY_NTV | TT_FO_RDWR_NTV)) {
        case TT_FO_RDONLY_NTV: {
            dwDesiredAccess |= FILE_GENERIC_READ;
        } break;
        case TT_FO_WRONLY_NTV: {
            dwDesiredAccess |= FILE_GENERIC_WRITE;
        } break;
        case TT_FO_RDWR_NTV: {
            dwDesiredAccess |= FILE_GENERIC_READ | FILE_GENERIC_WRITE;
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
        dwDesiredAccess &= ~FILE_WRITE_DATA;
        dwDesiredAccess |= FILE_APPEND_DATA;
    }

    // map:
    //  - TT_FO_CREAT_NTV
    //  - TT_FO_EXCL_NTV
    //  - TT_FO_TRUNC_NTV
    switch (flag & (TT_FO_CREAT_NTV | TT_FO_EXCL_NTV | TT_FO_TRUNC_NTV)) {
        case 0: {
            // if exist, open it
            // if not exist, fail
            dwCreationDisposition |= OPEN_EXISTING;
        } break;
        case TT_FO_CREAT_NTV: {
            // if exist, open it
            // if not exist, create it
            dwCreationDisposition |= OPEN_ALWAYS;
        } break;
        case TT_FO_CREAT_NTV | TT_FO_EXCL_NTV:
        case TT_FO_CREAT_NTV | TT_FO_EXCL_NTV | TT_FO_TRUNC_NTV: {
            // if exist, fail
            // if not exist, create it
            dwCreationDisposition |= CREATE_NEW;
        } break;
        case TT_FO_TRUNC_NTV:
        case TT_FO_CREAT_NTV | TT_FO_TRUNC_NTV: {
            // if exist, truncate it
            // if not exist, create it
            dwCreationDisposition |= CREATE_ALWAYS;
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

    dwFlagsAndAttributes |= FILE_ATTRIBUTE_NORMAL;

    // retrieve process permission
    if (_umask_s(0, &mode) != 0) {
        TT_ERROR_NTV("_umask_s failed");
        return TT_FAIL;
    }
    if (!(~mode & _S_IWRITE)) {
        // no write permission
        dwFlagsAndAttributes |= FILE_ATTRIBUTE_READONLY;
    }

    // map TT_FO_HINT_TEMPORARY
    if (flag & TT_FO_HINT_TEMPORARY) {
        dwFlagsAndAttributes |= FILE_ATTRIBUTE_TEMPORARY;
    }

    // map:
    //  - TT_FO_SEQUENTIAL_NTV
    //  - TT_FO_RANDOM_NTV
    switch (flag & (TT_FO_SEQUENTIAL_NTV | TT_FO_RANDOM_NTV)) {
        case 0: {
        } break;
        case TT_FO_SEQUENTIAL_NTV: {
            dwFlagsAndAttributes |= FILE_FLAG_SEQUENTIAL_SCAN;
        } break;
        case TT_FO_RANDOM_NTV: {
            dwFlagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;
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

    if (flag & TT_FO_INTERNAL_AIO_ENABLE) {
        dwFlagsAndAttributes |= FILE_FLAG_OVERLAPPED;
    }

    // file would be read and written even other thread has opened it
    dwShareMode |= FILE_SHARE_READ;
    dwShareMode |= FILE_SHARE_WRITE;
    dwShareMode |= FILE_SHARE_DELETE;

    // ========================================
    // create file
    // ========================================

    w_path = tt_wchar_create(path, NULL);
    if (w_path == NULL) {
        return TT_FAIL;
    }

    fh = CreateFileW(w_path,
                     dwDesiredAccess,
                     dwShareMode,
                     NULL,
                     dwCreationDisposition,
                     dwFlagsAndAttributes,
                     NULL);
    if (fh == INVALID_HANDLE_VALUE) {
        TT_ERROR_NTV("fail to create file: %ls", w_path);
        tt_wchar_destroy(w_path);
        return TT_FAIL;
    }
    tt_wchar_destroy(w_path);

    file->hf = fh;

    file->ev_mark = TT_EV_RANGE_INTERNAL_FILE;
    file->evc = NULL;
    tt_list_init(&file->aio_q);
    file->pos = 0;
    file->closing = TT_FALSE;

    return TT_SUCCESS;
}

tt_result_t tt_fclose_ntv(IN tt_file_ntv_t *file)
{
    TT_ASSERT(file->hf != INVALID_HANDLE_VALUE);

    if (CloseHandle(file->hf)) {
        file->hf = INVALID_HANDLE_VALUE;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to close system file");
        return TT_FAIL;
    }
}

tt_result_t tt_fseek_ntv(IN tt_file_ntv_t *file,
                         IN tt_u32_t whence,
                         IN tt_s64_t distance,
                         OUT tt_u64_t *position)
{
    LONG lDistanceToMove = (LONG)distance;
    LONG lpDistanceToMoveHigh = (LONG)(distance >> 32);
    DWORD dwMoveMethod;

    DWORD new_position;

    TT_ASSERT(file->hf != INVALID_HANDLE_VALUE);

    if (whence == TT_FPOS_BEGIN_NTV) {
        dwMoveMethod = FILE_BEGIN;
    } else if (whence == TT_FPOS_CUR_NTV) {
        dwMoveMethod = FILE_CURRENT;
    } else if (whence == TT_FPOS_END_NTV) {
        dwMoveMethod = FILE_END;
    } else {
        TT_ERROR("invalid whence: %d", whence);
        return TT_FAIL;
    }

    new_position = SetFilePointer(file->hf,
                                  lDistanceToMove,
                                  &lpDistanceToMoveHigh,
                                  dwMoveMethod);
    if (new_position == INVALID_SET_FILE_POINTER) {
        TT_ERROR_NTV("fail to set file pointer");
        return TT_FAIL;
    }

    if (position != NULL) {
        *position =
            ((tt_s64_t)new_position) | (((tt_s64_t)lpDistanceToMoveHigh) << 32);
    }
    return TT_SUCCESS;
}

tt_result_t tt_dcreate_ntv(IN const tt_char_t *path,
                           IN struct tt_fs_dir_attr_s *attr)
{
    wchar_t *w_path;
    BOOL ret;

    w_path = tt_wchar_create(path, NULL);
    if (w_path == NULL) {
        return TT_FAIL;
    }

    ret = CreateDirectoryW(w_path, NULL);
    if (!ret) {
        TT_ERROR_NTV("fail to create directory: %ls", path);
    }
    tt_wchar_destroy(w_path);

    return TT_COND(ret, TT_SUCCESS, TT_FAIL);
}

tt_result_t tt_dremove_ntv(IN const tt_char_t *path, IN tt_u32_t flag)
{
    wchar_t *w_path;
    int ret;

    w_path = tt_wchar_create_ex(path, NULL, TT_WCHAR_CREATE_LONGER, (void *)1);
    if (w_path == NULL) {
        return TT_FAIL;
    }

    if (flag & TT_DRM_RECURSIVE_NTV) {
        SHFILEOPSTRUCT FileOp;

#if 0
        wchar_t *__path;

        // as specified in MSDN: must be double-null terminated
#if 0        
        do
        {
            tt_u32_t path_len = (tt_u32_t)wcslen(path);

            TT_ASSERT(((tt_u8_t*)path)[path_len * sizeof(tt_char_t)] == 0);
            TT_ASSERT(((tt_u8_t*)path)[path_len * sizeof(tt_char_t) + 1] == 0);
        }while(0);
#else
        // recursively removing is not intensively used, so could use malloc
        ret = (int)wcslen(path);
        __path = (wchar_t*)malloc((ret + 2) * sizeof(wchar_t));
        if (__path == NULL)
        {
            TT_ERROR("no memory for new path");
            return TT_FAIL;
        }

        // copy and append double 0
        wcsncpy(__path, path, ret);
        __path[ret] = 0;
        __path[ret + 1] = 0;
#endif
#endif

        memset(&FileOp, 0, sizeof(SHFILEOPSTRUCT));
        FileOp.hwnd = NULL;
        FileOp.wFunc = FO_DELETE;
        FileOp.pFrom = (LPCTSTR)w_path;
        FileOp.pTo = NULL;
        FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
        FileOp.fAnyOperationsAborted = FALSE;
        FileOp.hNameMappings = NULL;
        FileOp.lpszProgressTitle = NULL;

        ret = SHFileOperationW(&FileOp);
        if (ret) {
            TT_ERROR("fail to remove directory: %s recursively", path);
        }
        // free(__path);
        tt_wchar_destroy(w_path);

        return TT_COND(ret, TT_FAIL, TT_SUCCESS);
    }

    ret = RemoveDirectoryW(w_path);
    if (!ret) {
        TT_ERROR_NTV("fail to remove directory[%s]", path);
    }
    tt_wchar_destroy(w_path);

    return TT_COND(ret, TT_SUCCESS, TT_FAIL);
}

tt_result_t tt_dopen_ntv(OUT tt_dir_ntv_t *dir,
                         IN const tt_char_t *path,
                         IN struct tt_fs_dir_attr_s *attr)
{
    wchar_t *w_path = NULL;
    tt_result_t result = TT_FAIL;

    w_path = tt_wchar_create(path, NULL);
    if (w_path == NULL) {
        return TT_FAIL;
    }

    if (!(GetFileAttributesW(w_path) & FILE_ATTRIBUTE_DIRECTORY)) {
        TT_ERROR("path[%ls] is not directory", path);
        goto __dop_out;
    }

    result = __fs_dopen(dir, w_path, attr);

__dop_out:

    if (w_path != NULL) {
        tt_wchar_destroy(w_path);
    }

    return result;
}

tt_result_t __fs_dopen(OUT tt_dir_ntv_t *dir,
                       IN wchar_t *w_path,
                       IN struct tt_fs_dir_attr_s *attr)
{
    tt_u32_t path_len = (tt_u32_t)wcslen(w_path);
    wchar_t *path_buf = NULL;
    tt_u32_t path_buf_size = (path_len + 4) * sizeof(wchar_t);

    if (path_len == 0) {
        TT_ERROR("empty path");
        return TT_FAIL;
    }

    path_buf = (wchar_t *)tt_malloc(path_buf_size);
    if (path_buf == NULL) {
        TT_ERROR("fail to alloc buf for path");
        return TT_FAIL;
    }

    // format path
    memcpy(path_buf, w_path, path_len * sizeof(wchar_t));
    if (path_buf[path_len - 1] == L'/') {
        path_buf[path_len++] = L'*';
    } else if (path_buf[path_len - 1] == L'\\') {
        path_buf[path_len++] = L'*';
    } else {
        path_buf[path_len++] = L'\\';
        path_buf[path_len++] = L'*';
    }
    path_buf[path_len++] = 0;

    // open dir
    dir->hd = FindFirstFileW(path_buf, &dir->find_data);
    if (dir->hd == INVALID_HANDLE_VALUE) {
        TT_ERROR_NTV("fail to find file in [%ls]", path_buf);
        goto dop_fail;
    }
    // it has at least '.' subdir, so next read would always succeed
    dir->next_result = TT_SUCCESS;

    dir->ev_mark = TT_EV_RANGE_INTERNAL_DIR;
    dir->evc = NULL;
    dir->aio_ev = NULL;
    tt_list_init(&dir->aio_q);
    dir->closing = TT_FALSE;

    // return path buf
    tt_free(path_buf);

    return TT_SUCCESS;

dop_fail:

    if (dir->hd != INVALID_HANDLE_VALUE) {
        FindClose(dir->hd);
    }

    if (path_buf != NULL) {
        tt_free(path_buf);
    }

    return TT_FAIL;
}

tt_result_t tt_dclose_ntv(OUT tt_dir_ntv_t *dir)
{
    if (!FindClose(dir->hd)) {
        TT_ERROR_NTV("fail to close diretory");
        return TT_FAIL;
    }
    dir->hd = INVALID_HANDLE_VALUE;

    if (dir->aio_ev != NULL) {
        tt_ev_destroy(dir->aio_ev);
        dir->aio_ev = NULL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_dread_ntv(IN tt_dir_ntv_t *dir,
                         IN tt_u32_t flag,
                         OUT tt_dir_entry_t *dentry)
{
    tt_char_t *utf8_name;

    TT_ASSERT(dir->hd != INVALID_HANDLE_VALUE);

    if (!TT_OK(dir->next_result)) {
        // failed last time
        return dir->next_result;
    }

    // note the find_data should be first returned through dentry
    // and then enumerate next file

    utf8_name = tt_utf8_create(dir->find_data.cFileName, NULL);
    if (utf8_name == NULL) {
        return TT_FAIL;
    }

    if (strncpy_s(dentry->name,
                  TT_MAX_FILE_NAME_LEN,
                  utf8_name,
                  TT_MAX_FILE_NAME_LEN) != 0) {
        TT_WARN("fail to return found file name: %s", dir->find_data.cFileName);
    }
    tt_utf8_destroy(utf8_name);

    if (dir->find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        dentry->type = TT_FSTYPE_DIR;
    } else {
        dentry->type = TT_FSTYPE_FILE;
    }

    // check next
    if (!FindNextFile(dir->hd, &dir->find_data)) {
        if (GetLastError() == ERROR_NO_MORE_FILES) {
            dir->next_result = TT_END;
        } else {
            dir->next_result = TT_FAIL;
            TT_ERROR_NTV("fail to get next file");
        }

        // note you can not return TT_FAIL here, but return it next time
        // when this function is called
        return TT_SUCCESS;
    }

    return TT_SUCCESS;
}
