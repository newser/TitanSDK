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
#include <time/tt_date.h>

#include <tt_util_native.h>

#include <copyfile.h>
#include <fcntl.h>
#include <fts.h>
#include <sys/stat.h>
#include <sys/types.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

//#define __SIMU_FAIL_OPEN
//#define __SIMU_FAIL_UNLINK
//#define __SIMU_FAIL_READ
//#define __SIMU_FAIL_WRITE
//#define __SIMU_FAIL_SEEK
//#define __SIMU_FAIL_CLOSE

//#define __SIMU_FAIL_MKDIR
//#define __SIMU_FAIL_FTSOPEN
//#define __SIMU_FAIL_FTSREAD
//#define __SIMU_FAIL_FTSCLOSE
//#define __SIMU_FAIL_READDIR
//#define __SIMU_FAIL_CLOSEDIR

#ifdef __SIMU_FAIL_OPEN
#define open __sf_open
int __sf_open(const char *path, int oflag, ...);
#endif

#ifdef __SIMU_FAIL_UNLINK
#define unlink __sf_unlink
int __sf_unlink(const char *path);
#endif

#ifdef __SIMU_FAIL_READ
#define read __sf_read
ssize_t __sf_read(int fildes, void *buf, size_t nbyte);
#endif

#ifdef __SIMU_FAIL_WRITE
#define write __sf_write
ssize_t __sf_write(int fildes, const void *buf, size_t nbyte);
#endif

#ifdef __SIMU_FAIL_SEEK
#define lseek __sf_lseek
off_t __sf_lseek(int fildes, off_t offset, int whence);
#endif

#ifdef __SIMU_FAIL_CLOSE
#define close __sf_close
int __sf_close(int fildes);
#endif

#ifdef __SIMU_FAIL_MKDIR
#define mkdir __sf_mkdir
int __sf_mkdir(const char *path, mode_t mode);
#endif

#ifdef __SIMU_FAIL_FTSOPEN
#define fts_open __sf_fts_open
FTS *__sf_fts_open(char *const *path_argv,
                   int options,
                   int (*compar)(const FTSENT **, const FTSENT **));
#endif

#ifdef __SIMU_FAIL_FTSREAD
#define fts_read __sf_fts_read
FTSENT *__sf_fts_read(FTS *ftsp);
#endif

#ifdef __SIMU_FAIL_FTSCLOSE
#define fts_close __sf_fts_close
int __sf_fts_close(FTS *ftsp);
#endif

#ifdef __SIMU_FAIL_OPENDIR
#define opendir __sf_opendir
DIR *__sf_opendir(const char *filename);
#endif

#ifdef __SIMU_FAIL_READDIR
#define readdir_r __sf_readdir
int __sf_readdir(DIR *dirp, struct dirent *entry, struct dirent **result);
#endif

#ifdef __SIMU_FAIL_CLOSEDIR
#define closedir __sf_closedir
int __sf_closedir(DIR *dirp);
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
    __FSTAT,
    __FTRUNC,
    __FCOPY,
    __FSYNC,
    __FUTIME,

    __DCREATE,
    __DREMOVE,
    __DOPEN,
    __DCLOSE,
    __DREAD,
    __DCOPY,

    __FS_EXIST,
    __FS_RENAME,

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
} __fread_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_file_ntv_t *file;
    tt_u8_t *buf;
    tt_u32_t buf_len;
    tt_u32_t *write_len;

    tt_result_t result;
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

    tt_file_ntv_t *file;
    tt_fstat_t *fst;

    tt_result_t result;
} __fstat_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_file_ntv_t *file;
    tt_u64_t len;

    tt_result_t result;
} __ftrunc_t;

typedef struct
{
    tt_io_ev_t io_ev;

    const tt_char_t *dst;
    const tt_char_t *src;
    tt_u32_t flag;

    tt_result_t result;
} __fcopy_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_file_ntv_t *file;

    tt_result_t result;
} __fsync_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_file_ntv_t *file;
    tt_date_t *accessed;
    tt_date_t *modified;

    tt_result_t result;
} __futime_t;

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

typedef struct
{
    tt_io_ev_t io_ev;

    const tt_char_t *dst;
    const tt_char_t *src;
    tt_u32_t flag;

    tt_result_t result;
} __dcopy_t;

typedef struct
{
    tt_io_ev_t io_ev;

    const tt_char_t *path;

    tt_bool_t result;
} __fs_exist_t;

typedef struct
{
    tt_io_ev_t io_ev;

    const tt_char_t *from;
    const tt_char_t *to;

    tt_result_t result;
} __fs_rename_t;

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

static void __do_fread(IN tt_io_ev_t *io_ev);

static void __do_fwrite(IN tt_io_ev_t *io_ev);

static void __do_fseek(IN tt_io_ev_t *io_ev);

static void __do_fstat(IN tt_io_ev_t *io_ev);

static void __do_ftrunc(IN tt_io_ev_t *io_ev);

static void __do_fcopy(IN tt_io_ev_t *io_ev);

static void __do_fsync(IN tt_io_ev_t *io_ev);

static void __do_futime(IN tt_io_ev_t *io_ev);

static void __do_dcreate(IN tt_io_ev_t *io_ev);

static void __do_dremove(IN tt_io_ev_t *io_ev);

static void __do_dopen(IN tt_io_ev_t *io_ev);

static void __do_dclose(IN tt_io_ev_t *io_ev);

static void __do_dread(IN tt_io_ev_t *io_ev);

static void __do_dcopy(IN tt_io_ev_t *io_ev);

static void __do_fs_exist(IN tt_io_ev_t *io_ev);

static void __do_fs_rename(IN tt_io_ev_t *io_ev);

static tt_worker_io_t __fs_io_handler[__FS_EV_NUM] = {
    __do_fcreate,  __do_fremove,   __do_fopen, __do_fclose,
    __do_fread,    __do_fwrite,    __do_fseek, __do_fstat,
    __do_ftrunc,   __do_fcopy,     __do_fsync, __do_futime,

    __do_dcreate,  __do_dremove,   __do_dopen, __do_dclose,
    __do_dread,    __do_dcopy,

    __do_fs_exist, __do_fs_rename,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __fs_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev);

static void __time2date(IN time_t *t, IN tt_date_t *d);

static void __date2timeval(IN tt_date_t *d, IN struct timeval *tv);

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

    __fs_ev_init(&fread.io_ev, __FREAD);

    fread.file = file;
    fread.buf = buf;
    fread.buf_len = buf_len;
    fread.read_len = read_len;

    fread.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &fread.io_ev);
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

    tt_iowg_push_ev(&tt_g_fs_iowg, &fwrite.io_ev);
    tt_fiber_suspend();
    return fwrite.result;
}

tt_result_t tt_fstat_ntv(IN tt_file_ntv_t *file, OUT tt_fstat_t *fst)
{
    __fstat_t fstat;

    __fs_ev_init(&fstat.io_ev, __FSTAT);

    fstat.file = file;
    fstat.fst = fst;

    fstat.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &fstat.io_ev);
    tt_fiber_suspend();
    return fstat.result;
}

tt_result_t tt_ftrunc_ntv(IN tt_file_ntv_t *file, IN tt_u64_t len)
{
    __ftrunc_t ftrunc;

    __fs_ev_init(&ftrunc.io_ev, __FTRUNC);

    ftrunc.file = file;
    ftrunc.len = len;

    ftrunc.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &ftrunc.io_ev);
    tt_fiber_suspend();
    return ftrunc.result;
}

tt_result_t tt_fcopy_ntv(IN const tt_char_t *dst,
                         IN const tt_char_t *src,
                         IN tt_u32_t flag)
{
    __fcopy_t fcopy;

    __fs_ev_init(&fcopy.io_ev, __FCOPY);

    fcopy.dst = dst;
    fcopy.src = src;
    fcopy.flag = flag;

    fcopy.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &fcopy.io_ev);
    tt_fiber_suspend();
    return fcopy.result;
}

tt_result_t tt_fsync_ntv(IN tt_file_ntv_t *file)
{
    __fsync_t fsync;

    __fs_ev_init(&fsync.io_ev, __FSYNC);

    fsync.file = file;

    fsync.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &fsync.io_ev);
    tt_fiber_suspend();
    return fsync.result;
}

tt_result_t tt_futime_ntv(IN tt_file_ntv_t *file,
                          IN tt_date_t *accessed,
                          IN tt_date_t *modified)
{
    __futime_t futime;

    __fs_ev_init(&futime.io_ev, __FUTIME);

    futime.file = file;
    futime.accessed = accessed;
    futime.modified = modified;

    futime.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &futime.io_ev);
    tt_fiber_suspend();
    return futime.result;
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

tt_result_t tt_dcopy_ntv(IN const tt_char_t *dst,
                         IN const tt_char_t *src,
                         IN tt_u32_t flag)
{
    __dcopy_t dcopy;

    __fs_ev_init(&dcopy.io_ev, __DCOPY);

    dcopy.dst = dst;
    dcopy.src = src;
    dcopy.flag = flag;

    dcopy.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &dcopy.io_ev);
    tt_fiber_suspend();
    return dcopy.result;
}

tt_bool_t tt_fs_exist_ntv(IN const tt_char_t *path)
{
    __fs_exist_t fs_exist;

    __fs_ev_init(&fs_exist.io_ev, __FS_EXIST);

    fs_exist.path = path;

    fs_exist.result = TT_FALSE;

    tt_iowg_push_ev(&tt_g_fs_iowg, &fs_exist.io_ev);
    tt_fiber_suspend();
    return fs_exist.result;
}

tt_result_t tt_fs_rename_ntv(IN const tt_char_t *from, IN const tt_char_t *to)
{
    __fs_rename_t fs_rename;

    __fs_ev_init(&fs_rename.io_ev, __FS_RENAME);

    fs_rename.from = from;
    fs_rename.to = to;

    fs_rename.result = TT_FAIL;

    tt_iowg_push_ev(&tt_g_fs_iowg, &fs_rename.io_ev);
    tt_fiber_suspend();
    return fs_rename.result;
}

void tt_fs_worker_io(IN tt_io_ev_t *io_ev)
{
    __fs_io_handler[io_ev->ev](io_ev);

    if (io_ev->src != NULL) {
        tt_task_finish(io_ev->src->fs->thread->task, io_ev);
    } else {
        tt_free(io_ev);
    }
}

void __fs_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev)
{
    io_ev->src = tt_current_fiber();
    io_ev->dst = NULL;
    tt_dnode_init(&io_ev->node);
    io_ev->io = TT_IO_FS;
    io_ev->ev = ev;
}

void __do_fcreate(IN tt_io_ev_t *io_ev)
{
    __fcreate_t *fcreate = (__fcreate_t *)io_ev;

    int fd, flag = O_CREAT | O_EXCL;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

again:
    fd = open(fcreate->path, flag, mode);
    if (fd >= 0) {
        __RETRY_IF_EINTR(close(fd));
        fcreate->result = TT_SUCCESS;
    } else if (errno == EINTR) {
        goto again;
    } else if (errno == EEXIST) {
        fcreate->result = TT_E_EXIST;
    } else {
        TT_ERROR_NTV("fail to create file: %s", fcreate->path);
        fcreate->result = TT_FAIL;
    }
}

void __do_fremove(IN tt_io_ev_t *io_ev)
{
    __fremove_t *fremove = (__fremove_t *)io_ev;

    if (unlink(fremove->path) == 0) {
        fremove->result = TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to remove %s", fremove->path);
        fremove->result = TT_FAIL;
    }
}

void __do_fopen(IN tt_io_ev_t *io_ev)
{
    __fopen_t *fopen = (__fopen_t *)io_ev;

    int fd;
    int flag = 0;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    // actually mode should be generated from attr

    switch (fopen->flag & (TT_FO_READ | TT_FO_WRITE)) {
        case TT_FO_WRITE: {
            flag |= O_WRONLY;
        } break;
        case (TT_FO_READ | TT_FO_WRITE): {
            flag |= O_RDWR;
        } break;
        case TT_FO_READ: {
            flag |= O_RDONLY;
        } break;
    }

    if (fopen->flag & TT_FO_APPEND) {
        flag |= O_APPEND;
    }

    switch (fopen->flag & (TT_FO_CREAT | TT_FO_EXCL | TT_FO_TRUNC)) {
        case TT_FO_CREAT: {
            // if exist, open it
            // if not exist, create it
            flag |= O_CREAT;
        } break;
        case TT_FO_CREAT | TT_FO_EXCL:
        case TT_FO_CREAT | TT_FO_EXCL | TT_FO_TRUNC: {
            // if exist, fail
            // if not exist, create it
            flag |= O_CREAT | O_EXCL;
        } break;
        case TT_FO_TRUNC:
        case TT_FO_CREAT | TT_FO_TRUNC: {
            // if exist, truncate it
            // if not exist, create it
            flag |= O_CREAT | O_TRUNC;
        } break;
        default: {
        } break;
    }

again:
    fd = open(fopen->path, flag, mode);
    if (fd >= 0) {
        tt_file_ntv_t *f = fopen->file;

        if (((flag = fcntl(fd, F_GETFD, 0)) == -1) ||
            (fcntl(fd, F_SETFD, flag | FD_CLOEXEC) == -1)) {
            TT_ERROR_NTV("fail to set file close-on-exec");
            __RETRY_IF_EINTR(close(fd) != 0);
            fopen->result = TT_FAIL;
        }

        f->fd = fd;

        fopen->result = TT_SUCCESS;
    } else if (errno == EINTR) {
        goto again;
    } else if (errno == ENOENT) {
        TT_ERROR_NTV("fail to open file: %s", fopen->path);
        fopen->result = TT_E_NOEXIST;
    } else {
        TT_ERROR_NTV("fail to open file: %s", fopen->path);
        fopen->result = TT_FAIL;
    }
}

void __do_fclose(IN tt_io_ev_t *io_ev)
{
    __fclose_t *fclose = (__fclose_t *)io_ev;

    __RETRY_IF_EINTR(close(fclose->file->fd));
}

void __do_fread(IN tt_io_ev_t *io_ev)
{
    __fread_t *fread = (__fread_t *)io_ev;

    ssize_t len = 0, n;

again:
    n = read(fread->file->fd, fread->buf, fread->buf_len);
    if (n > 0) {
        len += n;
        if (len < fread->buf_len) {
            goto again;
        }
        TT_SAFE_ASSIGN(fread->read_len, (tt_u32_t)len);
        fread->result = TT_SUCCESS;
    } else if (n == 0) {
        TT_SAFE_ASSIGN(fread->read_len, (tt_u32_t)len);
        fread->result =
            TT_COND(len > 0 || fread->buf_len == 0, TT_SUCCESS, TT_E_END);
    } else {
        TT_ERROR_NTV("read fail");
        TT_SAFE_ASSIGN(fread->read_len, (tt_u32_t)len);
        fread->result = TT_COND(len > 0, TT_SUCCESS, TT_FAIL);
    }
}

void __do_fwrite(IN tt_io_ev_t *io_ev)
{
    __fwrite_t *fwrite = (__fwrite_t *)io_ev;

    ssize_t len = 0, n;

again:
    n = write(fwrite->file->fd, fwrite->buf, fwrite->buf_len);
    if (n >= 0) {
        len += n;
        if (len < fwrite->buf_len) {
            goto again;
        }
        TT_SAFE_ASSIGN(fwrite->write_len, (tt_u32_t)len);
        fwrite->result = TT_SUCCESS;
    } else {
        TT_ERROR_NTV("write fail");
        TT_SAFE_ASSIGN(fwrite->write_len, (tt_u32_t)len);
        fwrite->result = TT_COND(len > 0, TT_SUCCESS, TT_FAIL);
    }
}

void __do_fseek(IN tt_io_ev_t *io_ev)
{
    __fseek_t *fseek = (__fseek_t *)io_ev;

    int whence;
    off_t offset = fseek->offset;
    off_t location;

    switch (fseek->whence) {
        case TT_FSEEK_BEGIN:
            whence = SEEK_SET;
            break;
        case TT_FSEEK_END:
            whence = SEEK_END;
            break;
        case TT_FSEEK_CUR:
        default:
            whence = SEEK_CUR;
            break;
    }

    location = lseek(fseek->file->fd, offset, whence);
    if (location >= 0) {
        TT_SAFE_ASSIGN(fseek->location, location);
        fseek->result = TT_SUCCESS;
    } else {
        TT_ERROR_NTV("lseek failed");
        fseek->result = TT_FAIL;
    }
}

void __do_fstat(IN tt_io_ev_t *io_ev)
{
    __fstat_t *fstat_ev = (__fstat_t *)io_ev;
    struct stat st;
    tt_fstat_t *fst = fstat_ev->fst;

    if (fstat(fstat_ev->file->fd, &st) != 0) {
        fstat_ev->result = TT_COND(errno == ENOENT, TT_E_NOEXIST, TT_FAIL);
        TT_ERROR_NTV("fstat failed");
        return;
    }

    tt_memset(fst, 0, sizeof(tt_fstat_t));
    tt_date_init(&fst->created, tt_g_local_tmzone);
    tt_date_init(&fst->accessed, tt_g_local_tmzone);
    tt_date_init(&fst->modified, tt_g_local_tmzone);

    fst->size = st.st_size;
    __time2date(&st.st_ctime, &fst->created);
    __time2date(&st.st_atime, &fst->accessed);
    __time2date(&st.st_mtime, &fst->modified);
    fst->link_num = (tt_u32_t)st.st_nlink;
    fst->is_file = TT_BOOL(S_ISREG(st.st_mode));
    fst->is_dir = TT_BOOL(S_ISDIR(st.st_mode));
    fst->is_usr_readable = TT_BOOL(st.st_mode & S_IRUSR);
    fst->is_grp_readable = TT_BOOL(st.st_mode & S_IRGRP);
    fst->is_oth_readable = TT_BOOL(st.st_mode & S_IROTH);
    fst->is_usr_writable = TT_BOOL(st.st_mode & S_IWUSR);
    fst->is_grp_writable = TT_BOOL(st.st_mode & S_IWGRP);
    fst->is_oth_writable = TT_BOOL(st.st_mode & S_IWOTH);
    fst->is_link = TT_BOOL(S_ISLNK(st.st_mode));
    fstat_ev->result = TT_SUCCESS;
}

void __do_ftrunc(IN tt_io_ev_t *io_ev)
{
    __ftrunc_t *ftrunc_ev = (__ftrunc_t *)io_ev;

    if (ftruncate(ftrunc_ev->file->fd, ftrunc_ev->len) == 0) {
        ftrunc_ev->result = TT_SUCCESS;
    } else {
        ftrunc_ev->result = TT_FAIL;
        TT_ERROR_NTV("ftrunc failed");
    }
}

void __do_fcopy(IN tt_io_ev_t *io_ev)
{
    __fcopy_t *fcopy_ev = (__fcopy_t *)io_ev;
    uint32_t flag = COPYFILE_ALL;

    if (fcopy_ev->flag & TT_FCOPY_EXCL) {
        flag |= COPYFILE_EXCL;
    }

    if (copyfile(fcopy_ev->src, fcopy_ev->dst, NULL, flag) == 0) {
        fcopy_ev->result = TT_SUCCESS;
    } else {
        fcopy_ev->result = TT_FAIL;
        TT_ERROR("fcopy failed");
    }
}

void __do_fsync(IN tt_io_ev_t *io_ev)
{
    __fsync_t *fsync_ev = (__fsync_t *)io_ev;

again:
    if (fsync(fsync_ev->file->fd) == 0) {
        fsync_ev->result = TT_SUCCESS;
    } else if (errno == EINTR) {
        goto again;
    } else {
        fsync_ev->result = TT_FAIL;
        TT_ERROR("fsync failed");
    }
}

void __do_futime(IN tt_io_ev_t *io_ev)
{
    __futime_t *futime_ev = (__futime_t *)io_ev;
    struct timeval times[2];

    __date2timeval(futime_ev->accessed, &times[0]);
    __date2timeval(futime_ev->modified, &times[1]);

    if (futimes(futime_ev->file->fd, times) == 0) {
        futime_ev->result = TT_SUCCESS;
    } else {
        futime_ev->result = TT_FAIL;
        TT_ERROR("futime failed");
    }
}

void __do_dcreate(IN tt_io_ev_t *io_ev)
{
    __dcreate_t *dcreate = (__dcreate_t *)io_ev;

    mode_t mode =
        S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    // without 'x', directory is not explorable

    if (mkdir(dcreate->path, mode) == 0) {
        dcreate->result = TT_SUCCESS;
    } else if (errno == EEXIST) {
        dcreate->result = TT_E_EXIST;
    } else {
        TT_ERROR_NTV("fail to create directory: %s", dcreate->path);
        dcreate->result = TT_FAIL;
    }
}

void __do_dremove(IN tt_io_ev_t *io_ev)
{
    __dremove_t *dremove = (__dremove_t *)io_ev;

    tt_char_t *path[2] = {(tt_char_t *)dremove->path, NULL};
    FTS *fts;
    FTSENT *ftse;

retry1:
    fts = fts_open(path, FTS_NOCHDIR | FTS_NOSTAT | FTS_PHYSICAL, NULL);
    if (fts == NULL) {
        if (errno == EINTR) {
            goto retry1;
        } else {
            TT_ERROR_NTV("fail to open fts: %s", path);
            dremove->result = TT_FAIL;
            return;
        }
    }

    while (1) {
    retry2:
        ftse = fts_read(fts);
        if (ftse == NULL) {
            if (errno == 0) {
                break;
            } else if (errno == EINTR) {
                goto retry2;
            } else {
                TT_ERROR_NTV("fail to read fts");
                dremove->result = TT_FAIL;
                return;
            }
        }

        switch (ftse->fts_info) {
            case FTS_DEFAULT:
            case FTS_F:
            case FTS_NSOK:
            case FTS_SL:
            case FTS_SLNONE: {
                // remove file
                if (unlink(ftse->fts_accpath) != 0) {
                    TT_ERROR_NTV("fail to remove file[%s]", ftse->fts_accpath);
                    dremove->result = TT_FAIL;
                    return;
                }
            } break;
            case FTS_DP: {
                // can remove directory now
                if (rmdir(ftse->fts_accpath) != 0) {
                    TT_ERROR_NTV("fail to remove directory[%s]",
                                 ftse->fts_accpath);
                    dremove->result = TT_FAIL;
                    return;
                }
            } break;
            case FTS_D: {
                // something to ignore
            } break;
            default: {
                // something unexpected
                TT_ERROR("expected fts info: %d", ftse->fts_info);
                dremove->result = TT_FAIL;
                return;
            } break;
        }
    }

__retry3:
    if (fts_close(fts) != 0) {
        if (errno == EINTR) {
            goto __retry3;
        } else {
            TT_ERROR_NTV("fail to close fts");
            // dremove->result = TT_FAIL;
            // return;
        }
    }

    dremove->result = TT_SUCCESS;
}

void __do_dopen(IN tt_io_ev_t *io_ev)
{
    __dopen_t *dopen = (__dopen_t *)io_ev;

    DIR *p;

    p = opendir(dopen->path);
    if (p != NULL) {
        dopen->dir->dir = p;
        dopen->result = TT_SUCCESS;
    } else {
        TT_ERROR_NTV("opendir fail");
        dopen->result = TT_FAIL;
    }
}

void __do_dclose(IN tt_io_ev_t *io_ev)
{
    __dclose_t *dclose = (__dclose_t *)io_ev;

    if (closedir(dclose->dir->dir) != 0) {
        TT_ERROR_NTV("fail to close diretory");
    }
}

void __do_dread(IN tt_io_ev_t *io_ev)
{
    __dread_t *dread = (__dread_t *)io_ev;

    struct dirent entry;
    struct dirent *result = NULL;

    if (readdir_r(dread->dir->dir, &entry, &result) == 0) {
        if (result != NULL) {
            strncpy(dread->entry->name, entry.d_name, TT_MAX_FILE_NAME_LEN);

            switch (entry.d_type) {
                case DT_DIR: {
                    dread->entry->type = TT_FS_TYPE_DIR;
                } break;
                case DT_UNKNOWN: {
                    dread->entry->type = TT_FS_TYPE_UNKNOWN;
                } break;
                default: {
                    dread->entry->type = TT_FS_TYPE_FILE;
                } break;
            }

            dread->result = TT_SUCCESS;
        } else {
            dread->result = TT_E_END;
        }
    } else {
        TT_ERROR_NTV("fail to read dir");
        dread->result = TT_FAIL;
    }
}

void __do_dcopy(IN tt_io_ev_t *io_ev)
{
    __dcopy_t *dcopy_ev = (__dcopy_t *)io_ev;
    uint32_t flag = COPYFILE_ALL | COPYFILE_RECURSIVE;

#if 0
    if (dcopy_ev->flag & TT_DCOPY_EXCL) {
        flag |= COPYFILE_EXCL;
    }
#endif

    if (copyfile(dcopy_ev->src, dcopy_ev->dst, NULL, flag) == 0) {
        dcopy_ev->result = TT_SUCCESS;
    } else {
        dcopy_ev->result = TT_FAIL;
        TT_ERROR("dcopy failed");
    }
}

void __do_fs_exist(IN tt_io_ev_t *io_ev)
{
    __fs_exist_t *fs_exist = (__fs_exist_t *)io_ev;

    if (access(fs_exist->path, F_OK) == 0) {
        fs_exist->result = TT_TRUE;
    } else {
        fs_exist->result = TT_FALSE;
    }
}

void __do_fs_rename(IN tt_io_ev_t *io_ev)
{
    __fs_rename_t *fs_rename = (__fs_rename_t *)io_ev;

    if (rename(fs_rename->from, fs_rename->to) == 0) {
        fs_rename->result = TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to rename from %s to %s",
                     fs_rename->from,
                     fs_rename->to);
        fs_rename->result = TT_FAIL;
    }
}

void __time2date(IN time_t *t, IN tt_date_t *d)
{
    struct tm tm;
    localtime_r(t, &tm);
    tt_date_set_year(d, tm.tm_year + 1900);
    tt_date_set_month(d, tm.tm_mon);
    tt_date_set_monthday(d, tm.tm_mday);
    tt_date_set_hour(d, tm.tm_hour);
    tt_date_set_minute(d, tm.tm_min);
    tt_date_set_second(d, TT_COND(tm.tm_sec < 60, tm.tm_sec, 59));
}

void __date2timeval(IN tt_date_t *d, IN struct timeval *tv)
{
    tv->tv_sec = tt_date_diff_epoch_second(d);
    tv->tv_usec = 0;
}

#ifdef open
#undef open
int __sf_open(const char *path, int oflag, ...)
{
    return -1;
}

#endif

#ifdef unlink
#undef unlink
int __sf_unlink(const char *path)
{
    if (tt_rand_u32() % 2) {
        return -1;
    } else {
        return unlink(path);
    }
}
#endif

#ifdef read
#undef read
ssize_t __sf_read(int fildes, void *buf, size_t nbyte)
{
    if ((tt_rand_u32() % 4) == 0) {
        return -1;
    }

    if (nbyte > 1) {
        nbyte = tt_rand_u32() % nbyte;
    }
    return read(fildes, buf, nbyte);
}
#endif

#ifdef write
#undef write
ssize_t __sf_write(int fildes, const void *buf, size_t nbyte)
{
    if ((tt_rand_u32() % 4) == 0) {
        return -1;
    }

    if (nbyte > 1) {
        nbyte = tt_rand_u32() % nbyte;
    }
    return write(fildes, buf, nbyte);
}
#endif

#ifdef lseek
#undef lseek
off_t __sf_lseek(int fildes, off_t offset, int whence)
{
    return -1;
}
#endif

#ifdef close
#undef close
int __sf_close(int fildes)
{
    if (tt_rand_u32() % 2) {
        return -1;
    } else {
        return close(fildes);
    }
}
#endif

#ifdef mkdir
#undef mkdir
int __sf_mkdir(const char *path, mode_t mode)
{
    if (tt_rand_u32() % 2) {
        return -1;
    } else {
        return mkdir(path, mode);
    }
}
#endif

#ifdef fts_open
#undef fts_open
FTS *__sf_fts_open(char *const *path_argv,
                   int options,
                   int (*compar)(const FTSENT **, const FTSENT **))
{
    if (tt_rand_u32() % 2) {
        return NULL;
    } else {
        return fts_open(path_argv, options, compar);
    }
}
#endif

#ifdef fts_read
#undef fts_read
FTSENT *__sf_fts_read(FTS *ftsp)
{
    if (tt_rand_u32() % 2) {
        return NULL;
    } else {
        return fts_read(ftsp);
    }
}
#endif

#ifdef fts_close
#undef fts_close
int __sf_fts_close(FTS *ftsp)
{
    if (tt_rand_u32() % 2) {
        return -1;
    } else {
        return fts_close(ftsp);
    }
}
#endif

#ifdef opendir
#undef opendir
DIR *__sf_opendir(const char *filename)
{
    if (tt_rand_u32() % 2) {
        return NULL;
    } else {
        return opendir(filename);
    }
}
#endif

#ifdef readdir_r
#undef readdir_r
int __sf_readdir(DIR *dirp, struct dirent *entry, struct dirent **result)
{
    if (tt_rand_u32() % 2) {
        return -1;
    } else {
        return readdir_r(dirp, entry, result);
    }
}
#endif

#ifdef closedir
#undef closedir
int __sf_closedir(DIR *dirp)
{
    if (tt_rand_u32() % 2) {
        return -1;
    } else {
        return closedir(dirp);
    }
}
#endif
