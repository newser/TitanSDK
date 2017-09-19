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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_file_system_native.h>

#include <io/tt_file_system.h>
#include <io/tt_io_event.h>
#include <io/tt_io_worker_group.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_task.h>

#include <tt_util_native.h>
#include <tt_wchar.h>

#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

//#define __SIMU_FAIL_CreateFileW
//#define __SIMU_FAIL_DeleteFileW
//#define __SIMU_FAIL_ReadFile
//#define __SIMU_FAIL_WriteFile
//#define __SIMU_FAIL_SetFilePointer
//#define __SIMU_FAIL_CloseHandle

//#define __SIMU_FAIL_CreateDirectoryW
//#define __SIMU_FAIL_RemoveDirectoryW
//#define __SIMU_FAIL_SHFileOperationW
//#define __SIMU_FAIL_FindFirstFileW
//#define __SIMU_FAIL_FindClose
//#define __SIMU_FAIL_FindNextFile

#ifdef __SIMU_FAIL_CreateFileW
#define CreateFileW __sf_CreateFileW
HANDLE WINAPI __sf_CreateFileW(LPCWSTR lpFileName,
                               DWORD dwDesiredAccess,
                               DWORD dwShareMode,
                               LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                               DWORD dwCreationDisposition,
                               DWORD dwFlagsAndAttributes,
                               HANDLE hTemplateFile);
#endif

#ifdef __SIMU_FAIL_DeleteFileW
#define DeleteFileW __sf_DeleteFileW
BOOL WINAPI __sf_DeleteFileW(LPCWSTR lpFileName);
#endif

#ifdef __SIMU_FAIL_ReadFile
#define ReadFile __sf_ReadFile
BOOL WINAPI __sf_ReadFile(HANDLE hFile,
                          __out LPVOID lpBuffer,
                          DWORD nNumberOfBytesToRead,
                          __out_opt LPDWORD lpNumberOfBytesRead,
                          __inout_opt LPOVERLAPPED lpOverlapped);
#endif

#ifdef __SIMU_FAIL_WriteFile
#define WriteFile __sf_WriteFile
BOOL WINAPI __sf_WriteFile(HANDLE hFile,
                           LPCVOID lpBuffer,
                           DWORD nNumberOfBytesToWrite,
                           __out_opt LPDWORD lpNumberOfBytesWritten,
                           __inout_opt LPOVERLAPPED lpOverlapped);
#endif

#ifdef __SIMU_FAIL_SetFilePointer
#define SetFilePointer __sf_SetFilePointer
DWORD WINAPI __sf_SetFilePointer(HANDLE hFile,
                                 LONG lDistanceToMove,
                                 __inout_opt PLONG lpDistanceToMoveHigh,
                                 DWORD dwMoveMethod);
#endif

#ifdef __SIMU_FAIL_CloseHandle
#define CloseHandle __sf_CloseHandle
BOOL WINAPI __sf_CloseHandle(HANDLE hObject);
#endif

#ifdef __SIMU_FAIL_CreateDirectoryW
#define CreateDirectoryW __sf_CreateDirectoryW
BOOL WINAPI __sf_CreateDirectoryW(LPCWSTR lpPathName,
                                  LPSECURITY_ATTRIBUTES lpSecurityAttributes);
#endif

#ifdef __SIMU_FAIL_RemoveDirectoryW
#define RemoveDirectoryW __sf_RemoveDirectoryW
BOOL WINAPI __sf_RemoveDirectoryW(LPCWSTR lpPathName);
#endif

#ifdef __SIMU_FAIL_SHFileOperationW
#define SHFileOperationW __sf_SHFileOperationW
int __sf_SHFileOperationW(LPSHFILEOPSTRUCT lpFileOp);
#endif

#ifdef __SIMU_FAIL_FindFirstFileW
#define FindFirstFileW __sf_FindFirstFileW
HANDLE WINAPI __sf_FindFirstFileW(LPCTSTR lpFileName,
                                  __out LPWIN32_FIND_DATA lpFindFileData);
#endif

#ifdef __SIMU_FAIL_FindClose
#define FindClose __sf_FindClose
BOOL WINAPI __sf_FindClose(__inout HANDLE hFindFile);
#endif

#ifdef __SIMU_FAIL_FindNextFile
#define FindNextFileW __sf_FindNextFileW
BOOL WINAPI __sf_FindNextFileW(HANDLE hFindFile,
                               __out LPWIN32_FIND_DATA lpFindFileData);
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __FCREATE,
    __FREMOVE,
    __FOPEN,
    __FCLOSE,
    __FREAD,
    __FWRITE,
    __FSEEK,

    __DCREATE,
    __DREMOVE,
    __DOPEN,
    __DCLOSE,
    __DREAD,

    __FS_EV_NUM,
};

typedef struct
{
    tt_io_ev_t io_ev;

    const tt_char_t *path;
    struct tt_file_attr_s *attr;

    tt_result_t result;
} __fcreate_t;

typedef struct
{
    tt_io_ev_t io_ev;

    const tt_char_t *path;

    tt_result_t result;
} __fremove_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_file_ntv_t *file;
    const tt_char_t *path;
    tt_u32_t flag;
    struct tt_file_attr_s *attr;
    HANDLE iocp;

    tt_result_t result;
} __fopen_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_file_ntv_t *file;
} __fclose_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_file_ntv_t *file;
    tt_u8_t *buf;
    tt_u32_t buf_len;
    tt_u32_t *read_len;

    tt_result_t result;
    tt_u32_t pos;
} __fread_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_file_ntv_t *file;
    tt_u8_t *buf;
    tt_u32_t buf_len;
    tt_u32_t *write_len;

    tt_result_t result;
    tt_u32_t pos;
} __fwrite_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_file_ntv_t *file;
    tt_u32_t whence;
    tt_s64_t offset;
    tt_u64_t *location;

    tt_result_t result;
} __fseek_t;

typedef struct
{
    tt_io_ev_t io_ev;

    const tt_char_t *path;
    struct tt_dir_attr_s *attr;

    tt_result_t result;
} __dcreate_t;

typedef struct
{
    tt_io_ev_t io_ev;

    const tt_char_t *path;

    tt_result_t result;
} __dremove_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_dir_ntv_t *dir;
    const tt_char_t *path;
    struct tt_dir_attr_s *attr;

    tt_result_t result;
} __dopen_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_dir_ntv_t *dir;
} __dclose_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_dir_ntv_t *dir;
    tt_dirent_t *entry;

    tt_result_t result;
} __dread_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __do_fcreate(IN tt_io_ev_t *io_ev);

static void __do_fremove(IN tt_io_ev_t *io_ev);

static void __do_fopen(IN tt_io_ev_t *io_ev);

static void __do_fclose(IN tt_io_ev_t *io_ev);

static void __do_fseek(IN tt_io_ev_t *io_ev);

static void __do_dcreate(IN tt_io_ev_t *io_ev);

static void __do_dremove(IN tt_io_ev_t *io_ev);

static void __do_dopen(IN tt_io_ev_t *io_ev);

static void __do_dclose(IN tt_io_ev_t *io_ev);

static void __do_dread(IN tt_io_ev_t *io_ev);

static tt_worker_io_t __fs_worker_io[__FS_EV_NUM] = {
    __do_fcreate,
    __do_fremove,
    __do_fopen,
    __do_fclose,
    NULL,
    NULL,
    __do_fseek,

    __do_dcreate,
    __do_dremove,
    __do_dopen,
    __do_dclose,
    __do_dread,
};

static tt_bool_t __do_fread(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_fwrite(IN tt_io_ev_t *io_ev);

static tt_poller_io_t __fs_poller_io[__FS_EV_NUM] = {
    NULL,
    NULL,
    NULL,
    NULL,
    __do_fread,
    __do_fwrite,
    NULL,

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __fs_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_fs_component_init_ntv()
{
    return TT_SUCCESS;
}

tt_result_t tt_fcreate_ntv(IN const tt_char_t *path,
                           IN struct tt_file_attr_s *attr)
{
    __fcreate_t fcreate;

    __fs_ev_init(&fcreate.io_ev, __FCREATE);

    fcreate.path = path;
    fcreate.attr = attr;

    fcreate.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &fcreate.io_ev);
    tt_fiber_suspend();
    return fcreate.result;
}

tt_result_t tt_fremove_ntv(IN const tt_char_t *path)
{
    __fremove_t fremove;

    __fs_ev_init(&fremove.io_ev, __FREMOVE);

    fremove.path = path;

    fremove.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &fremove.io_ev);
    tt_fiber_suspend();
    return fremove.result;
}

// this function can not open directory
tt_result_t tt_fopen_ntv(IN tt_file_ntv_t *file,
                         IN const tt_char_t *path,
                         IN tt_u32_t flag,
                         IN struct tt_file_attr_s *attr)
{
    __fopen_t fopen;

    __fs_ev_init(&fopen.io_ev, __FOPEN);

    fopen.file = file;
    fopen.path = path;
    fopen.flag = flag;
    fopen.attr = attr;
    fopen.iocp = tt_current_fiber_sched()->thread->task->iop.sys_iop.iocp;

    fopen.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &fopen.io_ev);
    tt_fiber_suspend();
    return fopen.result;
}

void tt_fclose_ntv(IN tt_file_ntv_t *file)
{
    __fclose_t fclose;

    __fs_ev_init(&fclose.io_ev, __FCLOSE);

    fclose.file = file;

    tt_iowg_push_ev(&tt_g_fs_iowg, &fclose.io_ev);
    tt_fiber_suspend();
}

tt_result_t tt_fseek_ntv(IN tt_file_ntv_t *file,
                         IN tt_u32_t whence,
                         IN tt_s64_t offset,
                         OUT tt_u64_t *location)
{
    __fseek_t fseek;

    __fs_ev_init(&fseek.io_ev, __FSEEK);

    fseek.file = file;
    fseek.whence = whence;
    fseek.offset = offset;
    fseek.location = location;

    tt_iowg_push_ev(&tt_g_fs_iowg, &fseek.io_ev);
    tt_fiber_suspend();
    return fseek.result;
}

// avoid calling fread and fwrite by multi thread at same time
tt_result_t tt_fread_ntv(IN tt_file_ntv_t *file,
                         OUT tt_u8_t *buf,
                         IN tt_u32_t buf_len,
                         OUT tt_u32_t *read_len)
{
    __fread_t fread;
    DWORD dwError;

    __fs_ev_init(&fread.io_ev, __FREAD);

    fread.file = file;
    fread.buf = buf;
    fread.buf_len = buf_len;
    fread.read_len = read_len;

    fread.result = TT_FAIL;
    fread.pos = 0;

    fread.io_ev.ov.Offset = (tt_u32_t)file->offset;
    fread.io_ev.ov.OffsetHigh = (tt_u32_t)(file->offset >> 32);

    if (!ReadFile(file->hf, buf, buf_len, NULL, &fread.io_ev.ov) &&
        ((dwError = GetLastError()) != ERROR_IO_PENDING)) {
        if (dwError == ERROR_HANDLE_EOF) {
            return TT_END;
        } else {
            TT_ERROR_NTV("fail to read file");
            return TT_FAIL;
        }
    }

    tt_fiber_suspend();
    return fread.result;
}

tt_result_t tt_fwrite_ntv(IN tt_file_ntv_t *file,
                          IN tt_u8_t *buf,
                          IN tt_u32_t buf_len,
                          OUT tt_u32_t *write_len)
{
    __fwrite_t fwrite;

    __fs_ev_init(&fwrite.io_ev, __FWRITE);

    fwrite.file = file;
    fwrite.buf = buf;
    fwrite.buf_len = buf_len;
    fwrite.write_len = write_len;

    fwrite.result = TT_FAIL;
    fwrite.pos = 0;

    // move to file end before writing
    if (file->append) {
        LONG high = 0;
        DWORD loc = SetFilePointer(file->hf, 0, &high, FILE_END);
        if (loc == INVALID_SET_FILE_POINTER) {
            TT_ERROR_NTV("fail to move to file end");
            return TT_FAIL;
        }
        file->offset = ((tt_s64_t)loc) | (((tt_s64_t)high) << 32);
    }
    fwrite.io_ev.ov.Offset = (tt_u32_t)file->offset;
    fwrite.io_ev.ov.OffsetHigh = (tt_u32_t)(file->offset >> 32);

    if (!WriteFile(file->hf, buf, buf_len, NULL, &fwrite.io_ev.ov) &&
        (GetLastError() != ERROR_IO_PENDING)) {
        TT_ERROR_NTV("fail to write file");
        return TT_FAIL;
    }

    tt_fiber_suspend();
    return fwrite.result;
}

tt_result_t tt_dcreate_ntv(IN const tt_char_t *path, IN tt_dir_attr_t *attr)
{
    __dcreate_t dcreate;

    __fs_ev_init(&dcreate.io_ev, __DCREATE);

    dcreate.path = path;
    dcreate.attr = attr;

    dcreate.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &dcreate.io_ev);
    tt_fiber_suspend();
    return dcreate.result;
}

tt_result_t tt_dremove_ntv(IN const tt_char_t *path)
{
    __dremove_t dremove;

    __fs_ev_init(&dremove.io_ev, __DREMOVE);

    dremove.path = path;

    dremove.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &dremove.io_ev);
    tt_fiber_suspend();
    return dremove.result;
}

tt_result_t tt_dopen_ntv(OUT tt_dir_ntv_t *dir,
                         IN const tt_char_t *path,
                         IN tt_dir_attr_t *attr)
{
    __dopen_t dopen;

    __fs_ev_init(&dopen.io_ev, __DOPEN);

    dopen.dir = dir;
    dopen.path = path;
    dopen.attr = attr;

    dopen.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &dopen.io_ev);
    tt_fiber_suspend();
    return dopen.result;
}

void tt_dclose_ntv(OUT tt_dir_ntv_t *dir)
{
    __dclose_t dclose;

    __fs_ev_init(&dclose.io_ev, __DCLOSE);

    dclose.dir = dir;

    tt_iowg_push_ev(&tt_g_fs_iowg, &dclose.io_ev);
    tt_fiber_suspend();
}

tt_result_t tt_dread_ntv(IN tt_dir_ntv_t *dir, OUT tt_dirent_t *entry)
{
    __dread_t dread;

    __fs_ev_init(&dread.io_ev, __DREAD);

    dread.dir = dir;
    dread.entry = entry;

    dread.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &dread.io_ev);
    tt_fiber_suspend();
    return dread.result;
}

void tt_fs_worker_io(IN tt_io_ev_t *io_ev)
{
    __fs_worker_io[io_ev->ev](io_ev);

    if (io_ev->src != NULL) {
        tt_task_finish(io_ev->src->fs->thread->task, io_ev);
    } else {
        tt_free(io_ev);
    }
}

tt_bool_t tt_fs_poller_io(IN tt_io_ev_t *io_ev)
{
    return __fs_poller_io[io_ev->ev](io_ev);
}

void __fs_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev)
{
    tt_memset(&io_ev->ov, 0, sizeof(OVERLAPPED));
    io_ev->src = tt_current_fiber();
    io_ev->dst = NULL;
    tt_dnode_init(&io_ev->node);
    io_ev->io = TT_IO_FS;
    io_ev->ev = ev;
}

void __do_fcreate(IN tt_io_ev_t *io_ev)
{
    __fcreate_t *fcreate = (__fcreate_t *)io_ev;

    HANDLE hf;
    wchar_t *w_path;

    w_path = tt_wchar_create(fcreate->path, NULL);
    if (w_path == NULL) {
        fcreate->result = TT_FAIL;
        return;
    }

    hf = CreateFileW(w_path,
                     FILE_GENERIC_READ | FILE_GENERIC_WRITE,
                     FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                     NULL,
                     CREATE_NEW,
                     FILE_ATTRIBUTE_NORMAL,
                     NULL);
    if (hf != INVALID_HANDLE_VALUE) {
        CloseHandle(hf);
        fcreate->result = TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to create file: %ls", w_path);
        fcreate->result = TT_FAIL;
    }

    tt_wchar_destroy(w_path);
}

void __do_fremove(IN tt_io_ev_t *io_ev)
{
    __fremove_t *fremove = (__fremove_t *)io_ev;

    wchar_t *w_path;

    w_path = tt_wchar_create(fremove->path, NULL);
    if (w_path == NULL) {
        fremove->result = TT_FAIL;
        return;
    }

    if (DeleteFileW(w_path)) {
        fremove->result = TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to delete file: %ls", w_path);
        fremove->result = TT_FAIL;
    }

    tt_wchar_destroy(w_path);
}

void __do_fopen(IN tt_io_ev_t *io_ev)
{
    __fopen_t *fopen = (__fopen_t *)io_ev;

    tt_file_ntv_t *file = fopen->file;
    DWORD dwDesiredAccess;
    // LPSECURITY_ATTRIBUTES lpSecurityAttributes;
    DWORD dwCreationDisposition;
    DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED;
    int mode;
    wchar_t *w_path;

    switch (fopen->flag & (TT_FO_READ | TT_FO_WRITE)) {
        case TT_FO_WRITE: {
            dwDesiredAccess = FILE_GENERIC_WRITE;
        } break;
        case (TT_FO_READ | TT_FO_WRITE): {
            dwDesiredAccess = (FILE_GENERIC_WRITE | FILE_GENERIC_READ);
        } break;
        default: {
            // readonly by default
            dwDesiredAccess = FILE_GENERIC_READ;
        } break;
    }

    if (fopen->flag & TT_FO_APPEND) {
        dwDesiredAccess &= ~FILE_WRITE_DATA;
        dwDesiredAccess |= FILE_APPEND_DATA;
    }

    switch (fopen->flag & (TT_FO_CREAT | TT_FO_EXCL | TT_FO_TRUNC)) {
        case TT_FO_CREAT: {
            // if exist, open it
            // if not exist, create it
            dwCreationDisposition = OPEN_ALWAYS;
        } break;
        case TT_FO_CREAT | TT_FO_EXCL:
        case TT_FO_CREAT | TT_FO_EXCL | TT_FO_TRUNC: {
            // if exist, fail
            // if not exist, create it
            dwCreationDisposition = CREATE_NEW;
        } break;
        case TT_FO_TRUNC:
        case TT_FO_CREAT | TT_FO_TRUNC: {
            // if exist, truncate it
            // if not exist, create it
            dwCreationDisposition = CREATE_ALWAYS;
        } break;
        default: {
            dwCreationDisposition = OPEN_EXISTING;
        } break;
    }

    if (_umask_s(0, &mode) && !(~mode & _S_IWRITE)) {
        dwFlagsAndAttributes |= FILE_ATTRIBUTE_READONLY;
    }

    w_path = tt_wchar_create(fopen->path, NULL);
    if (w_path == NULL) {
        fopen->result = TT_FAIL;
        return;
    }

    file->hf =
        CreateFileW(w_path,
                    dwDesiredAccess,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL,
                    dwCreationDisposition,
                    dwFlagsAndAttributes,
                    NULL);
    tt_wchar_destroy(w_path);
    if (file->hf == INVALID_HANDLE_VALUE) {
        TT_ERROR_NTV("fail to create file");
        fopen->result = TT_FAIL;
        return;
    }

    if (CreateIoCompletionPort(file->hf,
                               fopen->iocp,
                               (ULONG_PTR)fopen->file,
                               0) == NULL) {
        TT_ERROR_NTV("fail to associate file with iocp");
        CloseHandle(file->hf);
        fopen->result = TT_FAIL;
        return;
    }

    file->offset = 0;
    file->append = TT_BOOL(dwDesiredAccess & FILE_APPEND_DATA);

    fopen->result = TT_SUCCESS;
}

void __do_fclose(IN tt_io_ev_t *io_ev)
{
    __fclose_t *fclose = (__fclose_t *)io_ev;

    if (!CloseHandle(fclose->file->hf)) {
        TT_ERROR_NTV("fail to close file");
    }
}

tt_bool_t __do_fread(IN tt_io_ev_t *io_ev)
{
    __fread_t *fread = (__fread_t *)io_ev;

    tt_file_ntv_t *file = fread->file;

    // io_ev->io_bytes is always valid whatever io_result is
    file->offset += io_ev->io_bytes;

    fread->pos += io_ev->io_bytes;
    if (fread->pos == fread->buf_len) {
        TT_SAFE_ASSIGN(fread->read_len, fread->pos);
        fread->result = TT_SUCCESS;
        return TT_TRUE;
    }
    TT_ASSERT(fread->pos < fread->buf_len);

    // return success whenever some data is read out
    if (!TT_OK(io_ev->io_result)) {
        if (fread->pos > 0) {
            TT_SAFE_ASSIGN(fread->read_len, fread->pos);
            fread->result = TT_SUCCESS;
        } else {
            fread->result = io_ev->io_result;
        }
        return TT_TRUE;
    }

    // buf is not full
    fread->io_ev.ov.Offset = (tt_u32_t)file->offset;
    fread->io_ev.ov.OffsetHigh = (tt_u32_t)(file->offset >> 32);

    if (!ReadFile(fread->file->hf,
                  TT_PTR_INC(tt_u8_t, fread->buf, fread->pos),
                  fread->buf_len - fread->pos,
                  NULL,
                  &fread->io_ev.ov) &&
        (GetLastError() != ERROR_IO_PENDING)) {
        if (fread->pos > 0) {
            TT_SAFE_ASSIGN(fread->read_len, fread->pos);
            fread->result = TT_SUCCESS;
        } else {
            TT_ERROR_NTV("fail to read file");
            fread->result = TT_FAIL;
        }
        return TT_TRUE;
    }

    return TT_FALSE;
}

tt_bool_t __do_fwrite(IN tt_io_ev_t *io_ev)
{
    __fwrite_t *fwrite = (__fwrite_t *)io_ev;

    tt_file_ntv_t *file = fwrite->file;

    // io_ev->io_bytes is always valid whatever io_result is
    file->offset += io_ev->io_bytes;

    fwrite->pos += io_ev->io_bytes;
    if (fwrite->pos == fwrite->buf_len) {
        TT_SAFE_ASSIGN(fwrite->write_len, fwrite->pos);
        fwrite->result = TT_SUCCESS;
        return TT_TRUE;
    }
    TT_ASSERT(fwrite->pos < fwrite->buf_len);

    // return success whenever some data is written
    if (!TT_OK(io_ev->io_result)) {
        if (fwrite->pos > 0) {
            TT_SAFE_ASSIGN(fwrite->write_len, fwrite->pos);
            fwrite->result = TT_SUCCESS;
        } else {
            fwrite->result = io_ev->io_result;
        }
        return TT_TRUE;
    }

    // move to file end before writing
    if (file->append) {
        LONG high = 0;
        DWORD loc = SetFilePointer(file->hf, 0, &high, FILE_END);
        if (loc == INVALID_SET_FILE_POINTER) {
            if (fwrite->pos > 0) {
                TT_SAFE_ASSIGN(fwrite->write_len, fwrite->pos);
                fwrite->result = TT_SUCCESS;
            } else {
                TT_ERROR_NTV("fail to move to file end");
                fwrite->result = TT_FAIL;
            }
            return TT_TRUE;
        }
        file->offset = ((tt_s64_t)loc) | (((tt_s64_t)high) << 32);
    }
    fwrite->io_ev.ov.Offset = (tt_u32_t)file->offset;
    fwrite->io_ev.ov.OffsetHigh = (tt_u32_t)(file->offset >> 32);

    // buf is not full
    if (!WriteFile(fwrite->file->hf,
                   TT_PTR_INC(tt_u8_t, fwrite->buf, fwrite->pos),
                   fwrite->buf_len - fwrite->pos,
                   NULL,
                   &fwrite->io_ev.ov) &&
        (GetLastError() != ERROR_IO_PENDING)) {
        if (fwrite->pos > 0) {
            TT_SAFE_ASSIGN(fwrite->write_len, fwrite->pos);
            fwrite->result = TT_SUCCESS;
        } else {
            TT_ERROR("fail to write file");
            fwrite->result = TT_FAIL;
        }
        return TT_TRUE;
    }

    return TT_FALSE;
}

void __do_fseek(IN tt_io_ev_t *io_ev)
{
    __fseek_t *fseek = (__fseek_t *)io_ev;

    tt_file_ntv_t *file = fseek->file;

    if (fseek->whence == TT_FSEEK_BEGIN) {
        if (fseek->offset < 0) {
            TT_ERROR("negative location: %d", fseek->offset);
            fseek->result = TT_FAIL;
            return;
        }
        file->offset = fseek->offset;
    } else if (fseek->whence == TT_FSEEK_CUR) {
        if ((file->offset + fseek->offset) < 0) {
            TT_ERROR("negative location: %d", file->offset + fseek->offset);
            fseek->result = TT_FAIL;
            return;
        }
        file->offset += fseek->offset;
    } else {
        LONG high = 0;
        DWORD loc = SetFilePointer(file->hf, 0, &high, FILE_END);
        if (loc == INVALID_SET_FILE_POINTER) {
            TT_ERROR_NTV("fail to set file pointer");
            fseek->result = TT_FAIL;
            return;
        }
        file->offset = ((tt_s64_t)loc) | (((tt_s64_t)high) << 32);
    }

    TT_SAFE_ASSIGN(fseek->location, file->offset);
    fseek->result = TT_SUCCESS;
}

void __do_dcreate(IN tt_io_ev_t *io_ev)
{
    __dcreate_t *dcreate = (__dcreate_t *)io_ev;

    wchar_t *w_path;

    w_path = tt_wchar_create(dcreate->path, NULL);
    if (w_path == NULL) {
        dcreate->result = TT_FAIL;
        return;
    }

    if (CreateDirectoryW(w_path, NULL)) {
        dcreate->result = TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to create directory: %ls", w_path);
        dcreate->result = TT_FAIL;
    }

    tt_wchar_destroy(w_path);
}

void __do_dremove(IN tt_io_ev_t *io_ev)
{
    __dremove_t *dremove = (__dremove_t *)io_ev;

    wchar_t *w_path;
    SHFILEOPSTRUCTW FileOp;

    w_path = tt_wchar_create_ex(dremove->path,
                                NULL,
                                TT_WCHAR_CREATE_LONGER,
                                (void *)1);
    if (w_path == NULL) {
        dremove->result = TT_FAIL;
        return;
    }

    memset(&FileOp, 0, sizeof(SHFILEOPSTRUCT));
    FileOp.hwnd = NULL;
    FileOp.wFunc = FO_DELETE;
    FileOp.pFrom = (LPCWSTR)w_path;
    FileOp.pTo = NULL;
    FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    FileOp.fAnyOperationsAborted = FALSE;
    FileOp.hNameMappings = NULL;
    FileOp.lpszProgressTitle = NULL;

    if (RemoveDirectoryW(w_path) || !SHFileOperationW(&FileOp)) {
        dremove->result = TT_SUCCESS;
    } else {
        TT_ERROR("fail to remove directory: %s recursively", w_path);
        dremove->result = TT_FAIL;
    }

    tt_wchar_destroy(w_path);
}

void __do_dopen(IN tt_io_ev_t *io_ev)
{
    __dopen_t *dopen = (__dopen_t *)io_ev;
    tt_dir_ntv_t *dir = dopen->dir;

    wchar_t *w_path = NULL;
    tt_result_t result = TT_FAIL;
    tt_u32_t len;
    wchar_t *path = NULL;
    tt_u32_t path_len;

    w_path = tt_wchar_create(dopen->path, NULL);
    if (w_path == NULL) {
        goto out;
    }

    if (!(GetFileAttributesW(w_path) & FILE_ATTRIBUTE_DIRECTORY)) {
        TT_ERROR("path[%ls] is not directory", path);
        goto out;
    }

    len = (tt_u32_t)wcslen(w_path);
    if (len == 0) {
        TT_ERROR("empty path");
        goto out;
    }

    path_len = (len + 4) * sizeof(wchar_t);
    path = (wchar_t *)tt_malloc(path_len);
    if (path == NULL) {
        TT_ERROR("fail to alloc buf for path");
        goto out;
    }

    memcpy(path, w_path, len * sizeof(wchar_t));
    if (path[len - 1] == L'/') {
        path[len++] = L'*';
    } else if (path[len - 1] == L'\\') {
        path[len++] = L'*';
    } else {
        path[len++] = L'\\';
        path[len++] = L'*';
    }
    path[len++] = 0;

    dir->hd = FindFirstFileW(path, &dopen->dir->find_data);
    if (dir->hd == INVALID_HANDLE_VALUE) {
        TT_ERROR_NTV("fail to find file in [%ls]", path);
        goto out;
    }
    // it has at least '.' subdir, so next read would always succeed
    dir->next_result = TT_SUCCESS;

    result = TT_SUCCESS;

out:

    dopen->result = result;

    if (path != NULL) {
        tt_free(path);
    }

    if (w_path != NULL) {
        tt_wchar_destroy(w_path);
    }
}

void __do_dclose(IN tt_io_ev_t *io_ev)
{
    __dclose_t *dclose = (__dclose_t *)io_ev;

    if (!FindClose(dclose->dir->hd)) {
        TT_ERROR_NTV("fail to close diretory");
    }
}

void __do_dread(IN tt_io_ev_t *io_ev)
{
    __dread_t *dread = (__dread_t *)io_ev;
    tt_dir_ntv_t *dir = dread->dir;
    tt_dirent_t *entry = dread->entry;

    tt_char_t *utf8_name;

    if (!TT_OK(dir->next_result)) {
        dread->result = dir->next_result;
        return;
    }

    utf8_name = tt_utf8_create(dir->find_data.cFileName, NULL);
    if (utf8_name == NULL) {
        dread->result = TT_FAIL;
        return;
    }

    if (strncpy_s(entry->name,
                  TT_MAX_FILE_NAME_LEN,
                  utf8_name,
                  TT_MAX_FILE_NAME_LEN) != 0) {
        TT_WARN("fail to return found file name: %s", dir->find_data.cFileName);
    }
    tt_utf8_destroy(utf8_name);

    if (dir->find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        entry->type = TT_FS_TYPE_DIR;
    } else {
        entry->type = TT_FS_TYPE_FILE;
    }

    // check next
    if (!FindNextFileW(dir->hd, &dir->find_data)) {
        if (GetLastError() == ERROR_NO_MORE_FILES) {
            dir->next_result = TT_END;
        } else {
            dir->next_result = TT_FAIL;
            TT_ERROR_NTV("fail to get next file");
        }
    }

    dread->result = TT_SUCCESS;
}

#ifdef CreateFileW
#undef CreateFileW
HANDLE WINAPI __sf_CreateFileW(LPCWSTR lpFileName,
                               DWORD dwDesiredAccess,
                               DWORD dwShareMode,
                               LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                               DWORD dwCreationDisposition,
                               DWORD dwFlagsAndAttributes,
                               HANDLE hTemplateFile)
{
    return INVALID_HANDLE_VALUE;
}
#endif

#ifdef DeleteFileW
#undef DeleteFileW
BOOL WINAPI __sf_DeleteFileW(LPCWSTR lpFileName)
{
    if (tt_rand_u32() % 2) {
        return 0;
    } else {
        DeleteFileW(lpFileName);
    }
}
#endif

#ifdef ReadFile
#undef ReadFile
BOOL WINAPI __sf_ReadFile(HANDLE hFile,
                          __out LPVOID lpBuffer,
                          DWORD nNumberOfBytesToRead,
                          __out_opt LPDWORD lpNumberOfBytesRead,
                          __inout_opt LPOVERLAPPED lpOverlapped)
{
    if ((tt_rand_u32() % 4) == 0) {
        return FALSE;
    }

    if (nNumberOfBytesToRead > 1) {
        nNumberOfBytesToRead = tt_rand_u32() % nNumberOfBytesToRead;
    }
    return ReadFile(hFile,
                    lpBuffer,
                    nNumberOfBytesToRead,
                    lpNumberOfBytesRead,
                    lpOverlapped);
}
#endif

#ifdef WriteFile
#undef WriteFile
BOOL WINAPI __sf_WriteFile(HANDLE hFile,
                           LPCVOID lpBuffer,
                           DWORD nNumberOfBytesToWrite,
                           __out_opt LPDWORD lpNumberOfBytesWritten,
                           __inout_opt LPOVERLAPPED lpOverlapped)
{
    if ((tt_rand_u32() % 4) == 0) {
        return FALSE;
    }

    if (nNumberOfBytesToWrite > 1) {
        nNumberOfBytesToWrite = tt_rand_u32() % nNumberOfBytesToWrite;
    }
    return WriteFile(hFile,
                     lpBuffer,
                     nNumberOfBytesToWrite,
                     lpNumberOfBytesWritten,
                     lpOverlapped);
}
#endif

#ifdef SetFilePointer
#undef SetFilePointer
DWORD WINAPI __sf_SetFilePointer(HANDLE hFile,
                                 LONG lDistanceToMove,
                                 __inout_opt PLONG lpDistanceToMoveHigh,
                                 DWORD dwMoveMethod)
{
    return INVALID_SET_FILE_POINTER;
}
#endif

#ifdef CloseHandle
#undef CloseHandle
BOOL WINAPI __sf_CloseHandle(HANDLE hObject)
{
    if (tt_rand_u32() % 2) {
        return FALSE;
    } else {
        return CloseHandle(hObject);
    }
}
#endif

#ifdef CreateDirectory
#undef CreateDirectory
BOOL WINAPI __sf_CreateDirectoryW(LPCWSTR lpPathName,
                                  LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    if (tt_rand_u32() % 2) {
        return FALSE;
    } else {
        return CreateDirectoryW(lpPathName, lpSecurityAttributes);
    }
}
#endif

#ifdef RemoveDirectoryW
#undef RemoveDirectoryW
BOOL WINAPI __sf_RemoveDirectoryW(LPCWSTR lpPathName)
{
    if (tt_rand_u32() % 2) {
        return FALSE;
    } else {
        return RemoveDirectoryW(lpPathName);
    }
}
#endif

#ifdef SHFileOperationW
#undef SHFileOperationW
int __sf_SHFileOperationW(LPSHFILEOPSTRUCT lpFileOp)
{
    if (tt_rand_u32() % 2) {
        return -1;
    } else {
        return SHFileOperationW(lpFileOp);
    }
}
#endif

#ifdef FindFirstFileW
#undef FindFirstFileW
HANDLE WINAPI __sf_FindFirstFileW(LPCTSTR lpFileName,
                                  __out LPWIN32_FIND_DATA lpFindFileData)
{
    if (tt_rand_u32() % 2) {
        return INVALID_HANDLE_VALUE;
    } else {
        return FindFirstFileW(lpFileName, lpFindFileData);
    }
}
#endif

#ifdef FindClose
#undef FindClose
BOOL WINAPI __sf_FindClose(__inout HANDLE hFindFile)
{
    if (tt_rand_u32() % 2) {
        return FALSE;
    } else {
        return FindClose(hFindFile);
    }
}
#endif

#ifdef FindNextFileW
#undef FindNextFileW
BOOL WINAPI __sf_FindNextFileW(HANDLE hFindFile,
                               __out LPWIN32_FIND_DATA lpFindFileData)
{
    if (tt_rand_u32() % 2) {
        return FALSE;
    } else {
        return FindNextFileW(hFindFile, lpFindFileData);
    }
}
#endif
