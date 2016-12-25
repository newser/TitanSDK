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

#include <tt_file_system_aio_native.h>

#include <event/tt_event_center.h>
#include <event/tt_event_poller.h>
#include <memory/tt_memory_alloc.h>

#include <tt_cstd_api.h>

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_FSAIO TT_ASSERT

#define __FSAIO_CHECK_EVC

#define __FSAIO_DEBUG_FLAG
#ifdef __FSAIO_DEBUG_FLAG
#define __FSAIO_DEBUG_FLAG_DECLARE tt_u32_t __debug_flag;
#define __FSAIO_DEBUG_FLAG_SET(aio, flag) (aio)->__debug_flag = (flag)
#define __FSAIO_DEBUG_FLAG_OR(aio, n) (aio)->__debug_flag |= 1 << (n)
#else
#define __FSAIO_DEBUG_FLAG_DECLARE
#define __FSAIO_DEBUG_FLAG_SET(aio, flag)
#define __FSAIO_DEBUG_FLAG_OR(aio, n)
#endif

#define __INLINE_FBUF_NUM 4
#define __FBUF_SIZE(n) ((tt_u32_t)(sizeof(__fbuf_t) * (n)))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    EV_FAIO,
};

enum
{
    EV_FAIO_START = TT_EV_MKID_SUB(TT_EV_RANGE_INTERNAL_FILE, EV_FAIO, 0),

    EV_FWRITE,
    EV_FREAD,
    EV_FOPEN,
    EV_FREMOVE,
    EV_FCLOSE,
    EV_FCREATE,
    EV_FSEEK,
    EV_FAIO_Q,

    EV_FAIO_END,
};

enum
{
    EV_DAIO,
};

enum
{
    EV_DAIO_START = TT_EV_MKID_SUB(TT_EV_RANGE_INTERNAL_DIR, EV_DAIO, 0),

    EV_DCREATE,
    EV_DREMOVE,
    EV_DOPEN,
    EV_DCLOSE,
    EV_DREAD,
    EV_DAIO_Q,

    EV_DAIO_END,
};

typedef union
{
    struct iovec w;
    tt_blob_t t;
} __fbuf_t;

typedef struct
{
    struct iovec *iov;
    tt_u32_t iov_num;
    tt_u32_t cur_idx;
    tt_u32_t cur_pos;
    tt_u32_t io_bytes;
} __fbuf_state_t;

typedef struct
{
    tt_file_t *file;
} __faio_q_t;

typedef struct
{
    tt_char_t *path;
    tt_u32_t flag;
    tt_file_attr_t attr;
    tt_on_fcreate_t on_fcreate;

    tt_result_t result;
    void *cb_param;
} __fcreate_t;

typedef struct
{
    tt_char_t *path;
    tt_on_fremove_t on_fremove;

    tt_result_t result;
    void *cb_param;
} __fremove_t;

typedef struct
{
    tt_file_t *file;
    tt_char_t *path;
    tt_u32_t flag;
    tt_file_attr_t attr;
    tt_on_fopen_t on_fopen;
    tt_evcenter_t *evc;

    tt_result_t result;
    void *cb_param;
} __fopen_t;

typedef struct
{
    tt_file_t *file;
    tt_on_fclose_t on_fclose;

    tt_result_t result;
    void *cb_param;
} __fclose_t;

typedef struct
{
    tt_file_t *file;
    tt_u32_t whence;
    tt_s64_t distance;
    tt_on_fseek_t on_fseek;

    tt_result_t result;
    void *cb_param;
    tt_u64_t position;
} __fseek_t;

typedef struct
{
    tt_file_t *file;
    __fbuf_state_t fbuf_state;
    __fbuf_t fbuf[__INLINE_FBUF_NUM];
    tt_u64_t position;

    tt_result_t result;
    tt_on_fwrite_t on_fwrite;
    void *cb_param;

    __FSAIO_DEBUG_FLAG_DECLARE
} __fwrite_t;

typedef struct
{
    tt_file_t *file;
    __fbuf_state_t fbuf_state;
    tt_u64_t position;

    tt_result_t result;
    tt_on_fread_t on_fread;
    void *cb_param;
    __fbuf_t fbuf[__INLINE_FBUF_NUM];

    __FSAIO_DEBUG_FLAG_DECLARE
} __fread_t;

typedef struct
{
    tt_dir_t *dir;
} __daio_q_t;

typedef struct
{
    tt_char_t *path;
    tt_dir_attr_t attr;
    tt_on_dcreate_t on_dcreate;

    tt_result_t result;
    void *cb_param;
} __dcreate_t;

typedef struct
{
    tt_char_t *path;
    tt_u32_t flag;
    tt_on_dremove_t on_dremove;

    tt_result_t result;
    void *cb_param;
} __dremove_t;

typedef struct
{
    tt_dir_t *dir;
    tt_char_t *path;
    tt_dir_attr_t attr;
    tt_on_dopen_t on_dopen;
    tt_evcenter_t *evc;

    tt_result_t result;
    void *cb_param;
} __dopen_t;

typedef struct
{
    tt_dir_t *dir;
    tt_on_dclose_t on_dclose;

    tt_result_t result;
    void *cb_param;
} __dclose_t;

typedef struct
{
    tt_dir_t *dir;
    tt_u32_t flag;
    tt_on_dread_t on_dread;

    tt_dir_entry_t *dentry;
    tt_u32_t dentry_num;
    tt_dir_entry_t __dentry;

    tt_result_t result;
    void *cb_param;
    tt_u32_t read_num;
} __dread_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __fwrite_on_destroy(IN struct tt_ev_s *ev);

static tt_ev_itf_t __fwrite_itf = {
    NULL, __fwrite_on_destroy,
};

static void __fread_on_destroy(IN struct tt_ev_s *ev);

static tt_ev_itf_t __fread_itf = {
    NULL, __fread_on_destroy,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// fcreate
static void __do_fcreate(IN __fcreate_t *aio);

// fremove
static void __do_fremove(IN __fremove_t *aio);

// fopen
static void __do_fopen(IN __fopen_t *aio);

// fclose
static tt_bool_t __do_fclose(IN __fclose_t *aio);

// fseek
static tt_bool_t __do_fseek(IN __fseek_t *aio);

// fwrite
static tt_bool_t __do_fwrite(IN __fwrite_t *aio);
static tt_result_t __do_fwrite_io(IN __fwrite_t *aio);

// fread
static tt_bool_t __do_fread(IN __fread_t *aio);
static tt_result_t __do_fread_io(IN __fread_t *aio);

// faio queue
static void __do_faio_q(IN tt_file_ntv_t *sys_f);
static void __free_faio_q(IN tt_file_ntv_t *sys_f);

// dcreate
static void __do_dcreate(IN __dcreate_t *aio);

// dremove
static void __do_dremove(IN __dremove_t *aio);

// dopen
static void __do_dopen(IN __dopen_t *aio);

// dclose
static tt_bool_t __do_dclose(IN __dclose_t *aio);

// dread
static tt_bool_t __do_dread(IN __dread_t *aio);

// daio queue
static void __do_daio_q(IN tt_dir_ntv_t *sys_d);
static void __free_daio_q(IN tt_dir_ntv_t *sys_d);

// misc
static void __iovec2blob_inline(IN struct iovec *iov, IN tt_u32_t iov_num);

static tt_bool_t __update_fbuf_state(IN __fbuf_state_t *fbuf_state,
                                     IN tt_u32_t io_bytes);

#ifdef __SIMULATE_FS_AIO_FAIL

#define tt_evc_sendto_thread tt_evc_sendto_thread_SF
static tt_result_t tt_evc_sendto_thread_SF(IN tt_evcenter_t *evc,
                                           IN tt_thread_ev_t *tev);

#define readv readv_SF
static ssize_t readv_SF(int fd, const struct iovec *iov, int iovcnt);
#define writev writev_SF
static ssize_t writev_SF(int fd, const struct iovec *iov, int iovcnt);

#define pread pread_SF
static ssize_t pread_SF(int fd, void *buf, size_t count, off_t offset);

#define pwrite pwrite_SF
static ssize_t pwrite_SF(int fd, const void *buf, size_t count, off_t offset);

#endif

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_fcreate_async_ntv(IN const tt_char_t *path,
                                 IN tt_u32_t flag,
                                 IN tt_file_attr_t *attr,
                                 IN tt_on_fcreate_t on_fcreate,
                                 IN void *cb_param)
{
    tt_evcenter_t *evc = tt_evc_current();
    tt_u32_t path_len;
    tt_ev_t *ev;
    __fcreate_t *aio;

    TT_ASSERT(path != NULL);
    TT_ASSERT(on_fcreate != NULL);

    if (evc == NULL) {
        TT_ERROR("not in evc");
        return TT_BAD_PARAM;
    }

    path_len = (tt_u32_t)tt_strlen(path) + 1;

    // aio
    ev = tt_thread_ev_create(EV_FCREATE, sizeof(__fcreate_t) + path_len, NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __fcreate_t);

    // init aio
    aio->path = TT_PTR_INC(tt_char_t, aio, sizeof(__fcreate_t));
    tt_memcpy(aio->path, path, path_len);

    aio->flag = flag;

    if (attr != NULL) {
        tt_memcpy(&aio->attr, attr, sizeof(tt_file_attr_t));
    } else {
        tt_file_attr_default(&aio->attr);
    }

    aio->on_fcreate = on_fcreate;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // post aio
    if (!TT_OK(tt_evc_sendto_thread(evc, ev))) {
        TT_ERROR("fail to post fcreate");

        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_fremove_async_ntv(IN const tt_char_t *path,
                                 IN tt_on_fremove_t on_fremove,
                                 IN void *cb_param)
{
    tt_evcenter_t *evc = tt_evc_current();
    tt_u32_t path_len;
    tt_ev_t *ev;
    __fremove_t *aio;

    TT_ASSERT(path != NULL);
    TT_ASSERT(on_fremove != NULL);

    if (evc == NULL) {
        TT_ERROR("not in evc");
        return TT_BAD_PARAM;
    }

    path_len = (tt_u32_t)tt_strlen(path) + 1;

    // aio
    ev = tt_thread_ev_create(EV_FREMOVE, sizeof(__fremove_t) + path_len, NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __fremove_t);

    // init aio
    aio->path = TT_PTR_INC(tt_char_t, aio, sizeof(__fremove_t));
    tt_memcpy(aio->path, path, path_len);

    aio->on_fremove = on_fremove;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // post aio
    if (!TT_OK(tt_evc_sendto_thread(evc, ev))) {
        TT_ERROR("fail to post fremove");

        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_fopen_async_ntv(IN tt_file_t *file,
                               IN const tt_char_t *path,
                               IN tt_u32_t flag,
                               IN tt_file_attr_t *attr,
                               IN tt_on_fopen_t on_fopen,
                               IN void *cb_param)
{
    tt_evcenter_t *evc = tt_evc_current();
    tt_u32_t path_len;
    tt_ev_t *ev;
    __fopen_t *aio;

    TT_ASSERT(file != NULL);
    TT_ASSERT(path != NULL);
    TT_ASSERT(on_fopen != NULL);

    if (evc == NULL) {
        TT_ERROR("not in evc");
        return TT_BAD_PARAM;
    }

    path_len = (tt_u32_t)strlen(path) + 1;

    // aio
    ev = tt_thread_ev_create(EV_FOPEN, sizeof(__fopen_t) + path_len, NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __fopen_t);

    // init aio
    aio->file = file;
    aio->path = TT_PTR_INC(tt_char_t, aio, sizeof(__fopen_t));
    tt_memcpy(aio->path, path, path_len);
    aio->flag = flag | TT_FO_INTERNAL_AIO_ENABLE;

    if (attr != NULL) {
        tt_memcpy(&aio->attr, attr, sizeof(tt_file_attr_t));
    } else {
        tt_file_attr_default(&aio->attr);
    }

    aio->on_fopen = on_fopen;
    aio->evc = evc;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // post aio
    if (!TT_OK(tt_evc_sendto_thread(evc, ev))) {
        TT_ERROR("fail to post fopen");

        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_fclose_async_ntv(IN tt_file_t *file,
                                IN tt_on_fclose_t on_fclose,
                                IN void *cb_param)
{
    tt_file_ntv_t *sys_f;
    tt_evcenter_t *evc;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __fclose_t *aio;

    TT_ASSERT(file != NULL);

    sys_f = &file->sys_file;
    evc = sys_f->evc;

    if (on_fclose == NULL) {
        if (evc == tt_evc_current()) {
            TT_ERROR("can not brute close in fs evc");
            return TT_BAD_PARAM;
        }

        __free_faio_q(sys_f);
        close(sys_f->fd);
        sys_f->fd = -1;
        return TT_SUCCESS;
    }

#ifdef __FSAIO_CHECK_EVC
    if (evc != tt_evc_current()) {
        TT_ERROR("different fs evc");
        return TT_BAD_PARAM;
    }
#endif

    if (!file->aio_enable) {
        TT_ERROR("file does not support aio");
        return TT_FAIL;
    }

    if (sys_f->closing) {
        return TT_BAD_PARAM;
    }

    // aio
    ev = tt_thread_ev_create(EV_FCLOSE, sizeof(__fclose_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __fclose_t);

    // init aio
    aio->file = file;
    aio->on_fclose = on_fclose;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add to q
    tt_list_addtail(&sys_f->aio_q, &tev->node);

    // notify to process
    if ((tt_list_count(&sys_f->aio_q) == 1) &&
        !TT_OK(tt_evc_sendto_thread(evc, sys_f->aio_ev))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }

    sys_f->closing = TT_TRUE;
    return TT_SUCCESS;
}

tt_result_t tt_fseek_async_ntv(IN tt_file_t *file,
                               IN tt_u32_t whence,
                               IN tt_s64_t distance,
                               IN tt_on_fseek_t on_fseek,
                               IN void *cb_param)
{
    tt_file_ntv_t *sys_f;
    tt_evcenter_t *evc;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __fseek_t *aio;

    TT_ASSERT(file != NULL);
    TT_ASSERT(on_fseek != NULL);

    sys_f = &file->sys_file;
    evc = sys_f->evc;

#ifdef __FSAIO_CHECK_EVC
    if (evc != tt_evc_current()) {
        TT_ERROR("different fs evc");
        return TT_BAD_PARAM;
    }
#endif

    if (!file->aio_enable) {
        TT_ERROR("file does not support aio");
        return TT_FAIL;
    }

    if (sys_f->closing) {
        return TT_BAD_PARAM;
    }

    // aio
    ev = tt_thread_ev_create(EV_FSEEK, sizeof(__fseek_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __fseek_t);

    // init aio
    aio->file = file;
    aio->whence = whence;
    aio->distance = distance;
    aio->on_fseek = on_fseek;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;
    aio->position = -1;

    // add to q
    tt_list_addtail(&sys_f->aio_q, &tev->node);

    // notify to process
    if ((tt_list_count(&sys_f->aio_q) == 1) &&
        TT_OK(tt_evc_sendto_thread(evc, sys_f->aio_ev))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_fwrite_async_ntv(IN tt_file_t *file,
                                IN tt_blob_t *blob,
                                IN tt_u32_t iov_num,
                                IN tt_u64_t position,
                                IN tt_on_fwrite_t on_fwrite,
                                IN void *cb_param)
{
    tt_file_ntv_t *sys_f;
    tt_evcenter_t *evc;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __fwrite_t *aio;
    __fbuf_state_t *fbuf_state;
    tt_u32_t i;

    TT_ASSERT(file != NULL);
    TT_ASSERT(blob != NULL);
    TT_ASSERT(iov_num != 0);
    TT_ASSERT(on_fwrite != NULL);

    sys_f = &file->sys_file;
    evc = sys_f->evc;

#ifdef __FSAIO_CHECK_EVC
    if (evc != tt_evc_current()) {
        TT_ERROR("different fs evc");
        return TT_BAD_PARAM;
    }
#endif

    if (!file->aio_enable) {
        TT_ERROR("file does not support aio");
        return TT_FAIL;
    }

    if (sys_f->closing) {
        return TT_BAD_PARAM;
    }

    for (i = 0; i < iov_num; ++i) {
        if ((blob[i].addr == NULL) || (blob[i].len == 0)) {
            TT_ERROR("invalid buffer, none to write");
            return TT_FAIL;
        }
    }

    // alloc aio
    ev = tt_thread_ev_create(EV_FWRITE, sizeof(__fwrite_t), &__fwrite_itf);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __fwrite_t);
    fbuf_state = &aio->fbuf_state;

    // init aio
    aio->file = file;

    fbuf_state->iov_num = iov_num;
    if (iov_num <= __INLINE_FBUF_NUM) {
        fbuf_state->iov = (struct iovec *)aio->fbuf;
    } else {
        fbuf_state->iov = (struct iovec *)tt_malloc(__FBUF_SIZE(iov_num));
        if (fbuf_state->iov == NULL) {
            TT_ERROR("no mem for buf array");

            // can not use tt_ev_destroy, as ev data is not consistent now
            tt_free(ev);
            return TT_FAIL;
        }
    }
    for (i = 0; i < iov_num; ++i) {
        fbuf_state->iov[i].iov_base = blob[i].addr;
        fbuf_state->iov[i].iov_len = (size_t)blob[i].len;
    }
    fbuf_state->cur_idx = 0;
    fbuf_state->cur_pos = 0;
    fbuf_state->io_bytes = 0;

    aio->position = position;

    aio->result = TT_PROCEEDING;
    aio->on_fwrite = on_fwrite;
    aio->cb_param = cb_param;

    __FSAIO_DEBUG_FLAG_SET(aio, 0);

    // add to q
    tt_list_addtail(&sys_f->aio_q, &tev->node);

    // notify to process
    if ((tt_list_count(&sys_f->aio_q) == 1) &&
        !TT_OK(tt_evc_sendto_thread(evc, sys_f->aio_ev))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_fread_async_ntv(IN tt_file_t *file,
                               IN tt_blob_t *blob,
                               IN tt_u32_t iov_num,
                               IN tt_u64_t position,
                               IN tt_on_fread_t on_fread,
                               IN void *cb_param)
{
    tt_file_ntv_t *sys_f;
    tt_evcenter_t *evc;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __fread_t *aio;
    __fbuf_state_t *fbuf_state;
    tt_u32_t i;

    TT_ASSERT(file != NULL);
    TT_ASSERT(blob != NULL);
    TT_ASSERT(iov_num != 0);
    TT_ASSERT(on_fread != NULL);

    sys_f = &file->sys_file;
    evc = sys_f->evc;

#ifdef __FSAIO_CHECK_EVC
    if (evc != tt_evc_current()) {
        TT_ERROR("different fs evc");
        return TT_BAD_PARAM;
    }
#endif

    if (!file->aio_enable) {
        TT_ERROR("file does not support aio");
        return TT_FAIL;
    }

    if (sys_f->closing) {
        return TT_BAD_PARAM;
    }

    for (i = 0; i < iov_num; ++i) {
        if ((blob[i].addr == NULL) || (blob[i].len == 0)) {
            TT_ERROR("invalid buffer, none to read");
            return TT_FAIL;
        }
    }

    // alloc aio
    ev = tt_thread_ev_create(EV_FREAD, sizeof(__fread_t), &__fread_itf);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __fread_t);
    fbuf_state = &aio->fbuf_state;

    // init aio
    aio->file = file;

    fbuf_state->iov_num = iov_num;
    if (iov_num <= __INLINE_FBUF_NUM) {
        fbuf_state->iov = (struct iovec *)aio->fbuf;
    } else {
        fbuf_state->iov = (struct iovec *)tt_malloc(__FBUF_SIZE(iov_num));
        if (fbuf_state->iov == NULL) {
            TT_ERROR("no mem for buf array");

            tt_free(ev);
            return TT_FAIL;
        }
    }
    for (i = 0; i < iov_num; ++i) {
        fbuf_state->iov[i].iov_base = blob[i].addr;
        fbuf_state->iov[i].iov_len = (size_t)blob[i].len;
    }
    fbuf_state->cur_idx = 0;
    fbuf_state->cur_pos = 0;
    fbuf_state->io_bytes = 0;

    aio->position = position;

    aio->result = TT_PROCEEDING;
    aio->on_fread = on_fread;
    aio->cb_param = cb_param;

    __FSAIO_DEBUG_FLAG_SET(aio, 0);

    // add to q
    tt_list_addtail(&sys_f->aio_q, &tev->node);

    // notify to process
    if ((tt_list_count(&sys_f->aio_q) == 1) &&
        (!TT_OK(tt_evc_sendto_thread(evc, sys_f->aio_ev)))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_file_tev_handler(IN struct tt_evpoller_s *evp, IN tt_ev_t *ev)
{
    switch (ev->ev_id) {
        // fcreate
        case EV_FCREATE: {
            __do_fcreate(TT_EV_DATA(ev, __fcreate_t));
        } break;

        // fremove
        case EV_FREMOVE: {
            __do_fremove(TT_EV_DATA(ev, __fremove_t));
            tt_ev_destroy(ev);
        } break;

        // fopen
        case EV_FOPEN: {
            __do_fopen(TT_EV_DATA(ev, __fopen_t));
            tt_ev_destroy(ev);
        } break;

        // faio q
        case EV_FAIO_Q: {
            __do_faio_q(&TT_EV_DATA(ev, __faio_q_t)->file->sys_file);
            // do not destroy the ev
        } break;

        // unknown
        default: {
            TT_ERROR("unknown event: %x", ev->ev_id);
            tt_ev_destroy(ev);
        } break;
    }
    return TT_SUCCESS;
}

tt_result_t tt_dcreate_async_ntv(IN const tt_char_t *path,
                                 IN tt_dir_attr_t *attr,
                                 IN tt_on_dcreate_t on_dcreate,
                                 IN void *cb_param)
{
    tt_evcenter_t *evc = tt_evc_current();
    tt_u32_t path_len;
    tt_ev_t *ev;
    __dcreate_t *aio;

    TT_ASSERT(path != NULL);
    TT_ASSERT(on_dcreate != NULL);

    if (evc == NULL) {
        TT_ERROR("not in evc");
        return TT_BAD_PARAM;
    }

    path_len = (tt_u32_t)strlen(path) + 1;

    // aio
    ev = tt_thread_ev_create(EV_DCREATE, sizeof(__dcreate_t) + path_len, NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __dcreate_t);

    // init aio
    aio->path = TT_PTR_INC(tt_char_t, aio, sizeof(__dcreate_t));
    tt_memcpy(aio->path, path, path_len);

    if (attr != NULL) {
        tt_memcpy(&aio->attr, attr, sizeof(tt_dir_attr_t));
    } else {
        tt_dir_attr_default(&aio->attr);
    }

    aio->on_dcreate = on_dcreate;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // post aio
    if (!TT_OK(tt_evc_sendto_thread(evc, ev))) {
        TT_ERROR("fail to post dcreate");

        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_dremove_async_ntv(IN const tt_char_t *path,
                                 IN tt_u32_t flag,
                                 IN tt_on_dremove_t on_dremove,
                                 IN void *cb_param)
{
    tt_evcenter_t *evc = tt_evc_current();
    tt_u32_t path_len;
    tt_ev_t *ev;
    __dremove_t *aio;

    TT_ASSERT(path != NULL);
    TT_ASSERT(on_dremove != NULL);

    if (evc == NULL) {
        TT_ERROR("not in evc");
        return TT_BAD_PARAM;
    }

    path_len = (tt_u32_t)strlen(path) + 1;

    // aio
    ev = tt_thread_ev_create(EV_DREMOVE, sizeof(__dremove_t) + path_len, NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __dremove_t);

    // init aio
    aio->path = TT_PTR_INC(tt_char_t, aio, sizeof(__dremove_t));
    tt_memcpy(aio->path, path, path_len);

    aio->flag = flag;
    aio->on_dremove = on_dremove;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // post aio
    if (!TT_OK(tt_evc_sendto_thread(evc, ev))) {
        TT_ERROR("fail to post dremove");

        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_dopen_async_ntv(IN tt_dir_t *dir,
                               IN const tt_char_t *path,
                               IN tt_dir_attr_t *attr,
                               IN tt_on_dopen_t on_dopen,
                               IN void *cb_param)
{
    tt_evcenter_t *evc = tt_evc_current();
    tt_u32_t path_len;
    tt_ev_t *ev;
    __dopen_t *aio;

    TT_ASSERT(dir != NULL);
    TT_ASSERT(path != NULL);
    TT_ASSERT(on_dopen != NULL);

    if (evc == NULL) {
        TT_ERROR("not in evc");
        return TT_BAD_PARAM;
    }

    path_len = (tt_u32_t)strlen(path) + 1;

    // aio
    ev = tt_thread_ev_create(EV_DOPEN, sizeof(__dopen_t) + path_len, NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __dopen_t);

    // init aio
    aio->dir = dir;
    aio->path = TT_PTR_INC(tt_char_t, aio, sizeof(__dopen_t));
    tt_memcpy(aio->path, path, path_len);

    if (attr != NULL) {
        tt_memcpy(&aio->attr, attr, sizeof(tt_dir_attr_t));
    } else {
        tt_dir_attr_default(&aio->attr);
    }

    aio->on_dopen = on_dopen;
    aio->evc = evc;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // post aio
    if (!TT_OK(tt_evc_sendto_thread(evc, ev))) {
        TT_ERROR("fail to post dopen");

        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_dclose_async_ntv(IN tt_dir_t *dir,
                                IN tt_on_dclose_t on_dclose,
                                IN void *cb_param)
{
    tt_dir_ntv_t *sys_d;
    tt_evcenter_t *evc;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __dclose_t *aio;

    TT_ASSERT(dir != NULL);

    sys_d = &dir->sys_dir;
    evc = sys_d->evc;

    if (on_dclose == NULL) {
        if (evc == tt_evc_current()) {
            TT_ERROR("can not brute dclose in fs evc");
            return TT_BAD_PARAM;
        }

        __free_daio_q(sys_d);
        tt_dclose(dir);
        return TT_SUCCESS;
    }

#ifdef __FSAIO_CHECK_EVC
    if (evc != tt_evc_current()) {
        TT_ERROR("different fs evc");
        return TT_BAD_PARAM;
    }
#endif

    if (sys_d->closing) {
        return TT_BAD_PARAM;
    }

    // aio
    ev = tt_thread_ev_create(EV_DCLOSE, sizeof(__dclose_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __dclose_t);

    // init aio
    aio->dir = dir;
    aio->on_dclose = on_dclose;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add to q
    tt_list_addtail(&sys_d->aio_q, &tev->node);

    // notify to process
    if ((tt_list_count(&sys_d->aio_q) == 1) &&
        !TT_OK(tt_evc_sendto_thread(evc, sys_d->aio_ev))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }

    sys_d->closing = TT_TRUE;
    return TT_SUCCESS;
}

tt_result_t tt_dread_async_ntv(IN tt_dir_t *dir,
                               IN tt_u32_t flag,
                               IN tt_dir_entry_t *dentry,
                               IN tt_u32_t dentry_num,
                               IN tt_on_dread_t on_dread,
                               IN void *cb_param)
{
    tt_dir_ntv_t *sys_d;
    tt_evcenter_t *evc;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __dread_t *aio;

    TT_ASSERT(dir != NULL);
    TT_ASSERT(on_dread != NULL);

    sys_d = &dir->sys_dir;
    evc = sys_d->evc;

#ifdef __FSAIO_CHECK_EVC
    if (evc != tt_evc_current()) {
        TT_ERROR("different fs evc");
        return TT_BAD_PARAM;
    }
#endif

    if (sys_d->closing) {
        return TT_BAD_PARAM;
    }

    // aio
    ev = tt_thread_ev_create(EV_DREAD, sizeof(__dread_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __dread_t);

    // init aio
    aio->dir = dir;
    aio->flag = flag;
    aio->on_dread = on_dread;

    if (dentry == NULL) {
        aio->dentry = &aio->__dentry;
        aio->dentry_num = 1;
    } else {
        TT_ASSERT(dentry_num != 0);

        aio->dentry = dentry;
        aio->dentry_num = dentry_num;
    }
    tt_memset(aio->dentry, 0, sizeof(tt_dir_entry_t) * aio->dentry_num);

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;
    aio->read_num = 0;

    // add to q
    tt_list_addtail(&sys_d->aio_q, &tev->node);

    // notify to process
    if ((tt_list_count(&sys_d->aio_q) == 1) &&
        !TT_OK(tt_evc_sendto_thread(evc, sys_d->aio_ev))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_dir_tev_handler(IN struct tt_evpoller_s *evp, IN tt_ev_t *ev)
{
    switch (ev->ev_id) {
        // dcreate
        case EV_DCREATE: {
            __do_dcreate(TT_EV_DATA(ev, __dcreate_t));
            tt_ev_destroy(ev);
        } break;

        // dremove
        case EV_DREMOVE: {
            __do_dremove(TT_EV_DATA(ev, __dremove_t));
            tt_ev_destroy(ev);
        } break;

        // dopen
        case EV_DOPEN: {
            __do_dopen(TT_EV_DATA(ev, __dopen_t));
            tt_ev_destroy(ev);
        } break;

        // aio q
        case EV_DAIO_Q: {
            __do_daio_q(&TT_EV_DATA(ev, __daio_q_t)->dir->sys_dir);
            // do not destroy the ev
        } break;

        // unknown
        default: {
            TT_ERROR("unknown event: %x", ev->ev_id);
            tt_ev_destroy(ev);
        } break;
    }
    return TT_SUCCESS;
}

void __do_fcreate(IN __fcreate_t *aio)
{
    tt_faioctx_t aioctx;

    // do aio
    aio->result = tt_fcreate(aio->path, aio->flag, &aio->attr);

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_fcreate(aio->path, aio->flag, &aio->attr, &aioctx);
}

void __do_fremove(IN __fremove_t *aio)
{
    tt_faioctx_t aioctx;

    // do removing
    aio->result = tt_fremove(aio->path);

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_fremove(aio->path, &aioctx);
}

void __do_fopen(IN __fopen_t *aio)
{
    tt_file_t *file = aio->file;
    tt_file_ntv_t *sys_f = &file->sys_file;
    tt_faioctx_t aioctx;

    // do aio
    aio->result = tt_fopen(file, aio->path, aio->flag, &aio->attr);

    // init async file
    if (TT_OK(aio->result)) {
        sys_f->aio_ev =
            tt_thread_ev_create(EV_FAIO_Q, sizeof(__faio_q_t), NULL);
        if (sys_f->aio_ev != NULL) {
            __faio_q_t *faio_q = TT_EV_DATA(sys_f->aio_ev, __faio_q_t);
            faio_q->file = file;
        } else {
            tt_fclose(file);
            aio->result = TT_FAIL;
        }
    }

    if (TT_OK(aio->result)) {
        sys_f->evc = aio->evc;

        file->aio_enable = TT_TRUE;
    }

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_fopen(aio->file, aio->path, aio->flag, &aio->attr, &aioctx);
}

tt_bool_t __do_fclose(IN __fclose_t *aio)
{
    tt_faioctx_t aioctx;

    aio->result = tt_fclose(aio->file);

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_fclose(aio->file, &aioctx);

    return TT_TRUE;
}

tt_bool_t __do_fseek(IN __fseek_t *aio)
{
    tt_faioctx_t aioctx;

    aio->result =
        tt_fseek(aio->file, aio->whence, aio->distance, &aio->position);

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_fseek(aio->file,
                  aio->whence,
                  aio->distance,
                  &aioctx,
                  aio->position);

    return TT_TRUE;
}

tt_bool_t __do_fwrite(IN __fwrite_t *aio)
{
    tt_faioctx_t aioctx;
    __fbuf_state_t *fbuf_state = &aio->fbuf_state;

    aio->result = __do_fwrite_io(aio);

    // convert buf array
    __iovec2blob_inline(fbuf_state->iov, fbuf_state->iov_num);

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_fwrite(aio->file,
                   (tt_blob_t *)fbuf_state->iov,
                   fbuf_state->iov_num,
                   aio->position,
                   &aioctx,
                   fbuf_state->io_bytes);

    return TT_TRUE;
}

tt_result_t __do_fwrite_io(IN __fwrite_t *aio)
{
    tt_file_ntv_t *sys_f = &aio->file->sys_file;
    __fbuf_state_t *fbuf_state = &aio->fbuf_state;
    tt_result_t result = TT_FAIL;

    TT_ASSERT_FSAIO(aio->result == TT_PROCEEDING);
    TT_ASSERT_FSAIO(fbuf_state->cur_idx == 0);
    TT_ASSERT_FSAIO(fbuf_state->cur_pos == 0);

    if (aio->position == TT_FPOS_NULL) {
        ssize_t write_num = 0;

        while (fbuf_state->cur_idx < fbuf_state->iov_num) {
            struct iovec *cur_v = &fbuf_state->iov[fbuf_state->cur_idx];
            struct iovec *iov;
            int iov_cnt;
            struct iovec __iov;

            if ((fbuf_state->cur_idx == 0) && (fbuf_state->cur_pos == 0)) {
                iov = fbuf_state->iov;
                iov_cnt = fbuf_state->iov_num;
            } else {
                __iov.iov_base =
                    TT_PTR_INC(void, cur_v->iov_base, fbuf_state->cur_pos);
                __iov.iov_len = cur_v->iov_len - fbuf_state->cur_pos;

                iov = &__iov;
                iov_cnt = 1;
            }

            write_num = writev(sys_f->fd, iov, iov_cnt);
            if (write_num > 0) {
                __update_fbuf_state(fbuf_state, (tt_u32_t)write_num);
                continue;
                // while loop would check if all are written
            } else if (write_num == 0) {
                TT_ERROR("write 0 bytes");
                break;
            } else {
                if (errno == EINTR) {
                    continue;
                } else {
                    TT_ERROR_NTV("fail to write");
                    break;
                }
            }
        }

        TT_ASSERT_FSAIO(fbuf_state->cur_idx <= fbuf_state->iov_num);
        if (fbuf_state->io_bytes > 0) {
            return TT_SUCCESS;
        } else {
            return TT_FAIL;
        }
    } else {
        ssize_t write_num = 0;
        off_t offset = (off_t)aio->position;

        while (fbuf_state->cur_idx < fbuf_state->iov_num) {
            struct iovec *cur_v = &fbuf_state->iov[fbuf_state->cur_idx];

            write_num =
                pwrite(sys_f->fd,
                       TT_PTR_INC(void, cur_v->iov_base, fbuf_state->cur_pos),
                       cur_v->iov_len - fbuf_state->cur_pos,
                       offset);
            if (write_num > 0) {
                __update_fbuf_state(fbuf_state, (tt_u32_t)write_num);

                offset += write_num;
                continue;
            } else if (write_num == 0) {
                TT_ERROR("write 0 bytes");
                break;
            } else {
                if (errno == EINTR) {
                    continue;
                } else {
                    TT_ERROR_NTV("fail to write");
                    break;
                }
            }
        }

        TT_ASSERT_FSAIO(fbuf_state->cur_idx <= fbuf_state->iov_num);
        if (fbuf_state->io_bytes > 0) {
            return TT_SUCCESS;
        } else {
            return TT_FAIL;
        }
    }
}

void __fwrite_on_destroy(IN struct tt_ev_s *ev)
{
    __fwrite_t *aio = TT_EV_DATA(ev, __fwrite_t);
    __fbuf_state_t *fbuf_state = &aio->fbuf_state;

    if (fbuf_state->iov != (struct iovec *)aio->fbuf) {
        tt_free(fbuf_state->iov);
    }
}

tt_bool_t __do_fread(IN __fread_t *aio)
{
    tt_faioctx_t aioctx;
    __fbuf_state_t *fbuf_state = &aio->fbuf_state;

    aio->result = __do_fread_io(aio);

    // convert buf array
    __iovec2blob_inline(fbuf_state->iov, fbuf_state->iov_num);

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_fread(aio->file,
                  (tt_blob_t *)fbuf_state->iov,
                  fbuf_state->iov_num,
                  aio->position,
                  &aioctx,
                  fbuf_state->io_bytes);

    return TT_TRUE;
}

tt_result_t __do_fread_io(IN __fread_t *aio)
{
    tt_file_ntv_t *sys_f = &aio->file->sys_file;
    __fbuf_state_t *fbuf_state = &aio->fbuf_state;
    tt_result_t result = TT_FAIL;

    TT_ASSERT_FSAIO(aio->result == TT_PROCEEDING);
    TT_ASSERT_FSAIO(fbuf_state->cur_idx == 0);
    TT_ASSERT_FSAIO(fbuf_state->cur_pos == 0);

    if (aio->position == TT_FPOS_NULL) {
        ssize_t read_num = 0;

        while (fbuf_state->cur_idx < fbuf_state->iov_num) {
            struct iovec *cur_v = &fbuf_state->iov[fbuf_state->cur_idx];
            struct iovec *iov;
            int iov_cnt;
            struct iovec __iov;

            if ((fbuf_state->cur_idx == 0) && (fbuf_state->cur_pos == 0)) {
                iov = fbuf_state->iov;
                iov_cnt = fbuf_state->iov_num;
            } else {
                __iov.iov_base =
                    TT_PTR_INC(void, cur_v->iov_base, fbuf_state->cur_pos);
                __iov.iov_len = cur_v->iov_len - fbuf_state->cur_pos;

                iov = &__iov;
                iov_cnt = 1;
            }

            read_num = readv(sys_f->fd, iov, iov_cnt);
            if (read_num > 0) {
                __update_fbuf_state(fbuf_state, (tt_u32_t)read_num);
                continue;
                // while loop would check if all are written
            } else if (read_num == 0) {
                result = TT_END;
                break;
            } else {
                if (errno == EINTR) {
                    continue;
                } else {
                    TT_ERROR_NTV("fail to read");
                    break;
                }
            }
        }

        TT_ASSERT_FSAIO(fbuf_state->cur_idx <= fbuf_state->iov_num);
        if (fbuf_state->io_bytes > 0) {
            return TT_SUCCESS;
        } else if (result == TT_END) {
            return TT_END;
        } else {
            return TT_FAIL;
        }
    } else {
        ssize_t read_num = 0;
        off_t offset = (off_t)aio->position;

        while (fbuf_state->cur_idx < fbuf_state->iov_num) {
            struct iovec *cur_v = &fbuf_state->iov[fbuf_state->cur_idx];

            read_num =
                pread(sys_f->fd,
                      TT_PTR_INC(void, cur_v->iov_base, fbuf_state->cur_pos),
                      cur_v->iov_len - fbuf_state->cur_pos,
                      offset);
            if (read_num > 0) {
                __update_fbuf_state(fbuf_state, (tt_u32_t)read_num);

                offset += read_num;
                continue;
            } else if (read_num == 0) {
                result = TT_END;
                break;
            } else {
                if (errno == EINTR) {
                    continue;
                } else {
                    TT_ERROR_NTV("fail to read");
                    break;
                }
            }
        }

        TT_ASSERT_FSAIO(fbuf_state->cur_idx <= fbuf_state->iov_num);
        if (fbuf_state->io_bytes > 0) {
            return TT_SUCCESS;
        } else if (result == TT_END) {
            return TT_END;
        } else {
            return TT_FAIL;
        }
    }
}

void __fread_on_destroy(IN struct tt_ev_s *ev)
{
    __fread_t *aio = TT_EV_DATA(ev, __fread_t);
    __fbuf_state_t *fbuf_state = &aio->fbuf_state;

    if (fbuf_state->iov != (struct iovec *)aio->fbuf) {
        tt_free(fbuf_state->iov);
    }
}

void __do_faio_q(IN tt_file_ntv_t *sys_f)
{
    tt_lnode_t *node;

    while ((node = tt_list_head(&sys_f->aio_q)) != NULL) {
        tt_ev_t *ev = TT_EV_OF(TT_CONTAINER(node, tt_thread_ev_t, node));
        tt_bool_t aio_done = TT_TRUE;

        switch (ev->ev_id) {
            case EV_FCLOSE: {
                __fclose_t *aio = TT_EV_DATA(ev, __fclose_t);

                TT_ASSERT_FSAIO(sys_f == &aio->file->sys_file);

                if (tt_list_count(&sys_f->aio_q) > 1) {
                    TT_WARN("more aio after fclose");
                }

                tt_list_remove(node);
                __do_fclose(aio);
                // file may have been freed in on_fclose
                tt_ev_destroy(ev);
                return;
            } break;
            case EV_FSEEK: {
                __fseek_t *aio = TT_EV_DATA(ev, __fseek_t);

                TT_ASSERT_FSAIO(sys_f == &aio->file->sys_file);

                aio_done = __do_fseek(aio);
            } break;
            case EV_FWRITE: {
                __fwrite_t *aio = TT_EV_DATA(ev, __fwrite_t);

                TT_ASSERT_FSAIO(sys_f == &aio->file->sys_file);

                aio_done = __do_fwrite(aio);
            } break;
            case EV_FREAD: {
                __fread_t *aio = TT_EV_DATA(ev, __fread_t);

                TT_ASSERT_FSAIO(sys_f == &aio->file->sys_file);

                aio_done = __do_fread(aio);
            } break;

            default: {
                TT_ERROR("unknown aio: %x", ev->ev_id);
            } break;
        }

        if (!aio_done) {
            // keep it in aio q
            return;
        }
        tt_list_remove(node);
        tt_ev_destroy(ev);
        // next ev
    }
}

void __free_faio_q(IN tt_file_ntv_t *sys_f)
{
    tt_lnode_t *node;
    while ((node = tt_list_pophead(&sys_f->aio_q)) != NULL) {
        tt_ev_destroy(TT_EV_OF(TT_CONTAINER(node, tt_thread_ev_t, node)));
    }
}

void __do_dcreate(IN __dcreate_t *aio)
{
    tt_faioctx_t aioctx;

    // do aio
    aio->result = tt_dcreate(aio->path, &aio->attr);

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_dcreate(aio->path, &aio->attr, &aioctx);
}

void __do_dremove(IN __dremove_t *aio)
{
    tt_faioctx_t aioctx;

    // do removing
    aio->result = tt_dremove(aio->path, aio->flag);

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_dremove(aio->path, &aioctx);
}

void __do_dopen(IN __dopen_t *aio)
{
    tt_dir_t *dir = aio->dir;
    tt_dir_ntv_t *sys_d = &dir->sys_dir;
    tt_faioctx_t aioctx;

    // do aio
    aio->result = tt_dopen(dir, aio->path, &aio->attr);

    // init async dir
    if (TT_OK(aio->result)) {
        sys_d->aio_ev =
            tt_thread_ev_create(EV_DAIO_Q, sizeof(__faio_q_t), NULL);
        if (sys_d->aio_ev != NULL) {
            __daio_q_t *daio_q = TT_EV_DATA(sys_d->aio_ev, __daio_q_t);
            daio_q->dir = dir;
        } else {
            tt_dclose(dir);
            aio->result = TT_FAIL;
        }
    }

    if (TT_OK(aio->result)) {
        sys_d->evc = aio->evc;
    }

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_dopen(aio->dir, aio->path, &aio->attr, &aioctx);
}

tt_bool_t __do_dclose(IN __dclose_t *aio)
{
    tt_faioctx_t aioctx;

    // do aio
    aio->result = tt_dclose(aio->dir);

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_dclose(aio->dir, &aioctx);

    return TT_TRUE;
}

tt_bool_t __do_dread(IN __dread_t *aio)
{
    tt_faioctx_t aioctx;
    tt_u32_t num;

    // do aio
    for (num = 0; num < aio->dentry_num; ++num) {
        aio->result = tt_dread(aio->dir, aio->flag, &aio->dentry[num]);
        if (!TT_OK(aio->result)) {
            break;
        }
    }
    aio->read_num = num;
    TT_ASSERT_FSAIO(aio->result != TT_PROCEEDING);
    TT_ASSERT_FSAIO(!TT_OK(aio->result) || (aio->read_num == aio->dentry_num));

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_dread(aio->dir, aio->flag, aio->dentry, aio->read_num, &aioctx);

    return TT_TRUE;
}

void __do_daio_q(IN tt_dir_ntv_t *sys_d)
{
    tt_lnode_t *node;

    while ((node = tt_list_head(&sys_d->aio_q)) != NULL) {
        tt_ev_t *ev = TT_EV_OF(TT_CONTAINER(node, tt_thread_ev_t, node));
        tt_bool_t aio_done = TT_TRUE;

        switch (ev->ev_id) {
            case EV_DCLOSE: {
                __dclose_t *aio = TT_EV_DATA(ev, __dclose_t);

                TT_ASSERT_FSAIO(sys_d == &aio->dir->sys_dir);

                tt_list_remove(node);
                __do_dclose(aio);
                // dir may have been freed in on_dclose
                tt_ev_destroy(ev);
                return;
            } break;
            case EV_DREAD: {
                __dread_t *aio = TT_EV_DATA(ev, __dread_t);

                TT_ASSERT_FSAIO(sys_d == &aio->dir->sys_dir);

                aio_done = __do_dread(aio);
            } break;

            default: {
                TT_ERROR("unknown aio: %x", ev->ev_id);
                return;
            } break;
        }

        if (!aio_done) {
            return;
        }
        tt_list_remove(node);
        tt_ev_destroy(ev);
        // next
    }
}

void __free_daio_q(IN tt_dir_ntv_t *sys_d)
{
    tt_lnode_t *node;
    while ((node = tt_list_pophead(&sys_d->aio_q)) != NULL) {
        tt_ev_destroy(TT_EV_OF(TT_CONTAINER(node, tt_thread_ev_t, node)));
    }
}

void __iovec2blob_inline(IN struct iovec *iov, IN tt_u32_t iov_num)
{
    tt_blob_t *blob = (tt_blob_t *)iov;
    tt_u32_t i;

    TT_ASSERT_FSAIO(sizeof(struct iovec) >= sizeof(tt_blob_t));

    for (i = 0; i < iov_num; ++i) {
        struct iovec v;
        v.iov_base = iov[i].iov_base;
        v.iov_len = iov[i].iov_len;

        blob[i].addr = (tt_u8_t *)v.iov_base;
        blob[i].len = (tt_u32_t)v.iov_len;
    }
}

// return true if buffers are full
tt_bool_t __update_fbuf_state(IN __fbuf_state_t *fbuf_state,
                              IN tt_u32_t io_bytes)
{
    // update send_len
    fbuf_state->io_bytes += io_bytes;

    // update buf array
    while (fbuf_state->cur_idx < fbuf_state->iov_num) {
        struct iovec *cur_iov = &fbuf_state->iov[fbuf_state->cur_idx];
        tt_u32_t left_space;

        TT_ASSERT_FSAIO(fbuf_state->cur_pos < cur_iov->iov_len);
        left_space = (tt_u32_t)(cur_iov->iov_len - fbuf_state->cur_pos);

        if (io_bytes < left_space) {
            fbuf_state->cur_pos += io_bytes;
            break;
        }
        io_bytes -= left_space;

        // buf full, move to next
        ++fbuf_state->cur_idx;
        fbuf_state->cur_pos = 0;
    };
    TT_ASSERT_FSAIO(fbuf_state->cur_idx <= fbuf_state->iov_num);

    if (fbuf_state->cur_idx == fbuf_state->iov_num) {
        TT_ASSERT_FSAIO(io_bytes == 0);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

#ifdef __SIMULATE_FS_AIO_FAIL

tt_result_t tt_evc_sendto_thread_SF(IN tt_evcenter_t *evc,
                                    IN tt_thread_ev_t *tev)
{
    if (rand() % 2) {
        return TT_FAIL;
    } else {
#undef tt_evc_sendto_thread
        return tt_evc_sendto_thread(evc, tev);
    }
}

ssize_t readv_SF(int fd, const struct iovec *iov, int iovcnt)
{
    int r = rand() % 3;
    if (r == 0) {
        return iov[0].iov_len - 1;
    } else if (r == 1) {
        return -1;
    } else {
#undef readv
        return readv(fd, iov, iovcnt);
    }
}

ssize_t writev_SF(int fd, const struct iovec *iov, int iovcnt)
{
    int r = rand() % 3;
    if (r == 0) {
        return iov[0].iov_len - 1;
    } else if (r == 1) {
        return -1;
    } else {
#undef writev
        return writev(fd, iov, iovcnt);
    }
}

ssize_t pread_SF(int fd, void *buf, size_t count, off_t offset)
{
    int r = rand() % 3;
    if (r == 0) {
        return count - 1;
    } else if (r == 1) {
        return -1;
    } else {
#undef pread
        return pread(fd, buf, count, offset);
    }
}

ssize_t pwrite_SF(int fd, const void *buf, size_t count, off_t offset)
{
    int r = rand() % 3;
    if (r == 0) {
        return count - 1;
    } else if (r == 1) {
        return -1;
    } else {
#undef pwrite
        return pwrite(fd, buf, count, offset);
    }
}

#endif
