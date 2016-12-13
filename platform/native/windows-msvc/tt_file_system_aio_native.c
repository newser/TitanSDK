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
#include <memory/tt_memory_alloc.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

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

#define __FSAIO_CHECK_EVC

#define TT_ASSERT_FSAIO TT_ASSERT

#define __INLINE_FBLOB_NUM 4
#define __FBLOB_SIZE(n) ((tt_u32_t)(sizeof(tt_blob_t) * (n)))

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

typedef struct
{
    tt_blob_t *blob;
    tt_u32_t blob_num;
    tt_u32_t cur_idx;
    tt_u32_t cur_pos;
    tt_u32_t io_bytes;
} __fblob_state_t;

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
    OVERLAPPED ov;

    tt_result_t result;
    void *cb_param;
} __fclose_t;

typedef struct
{
    tt_file_t *file;
    tt_u32_t whence;
    tt_s64_t distance;
    tt_on_fseek_t on_fseek;
    OVERLAPPED ov;

    tt_result_t result;
    void *cb_param;
    tt_u64_t position;
} __fseek_t;

typedef struct
{
    tt_file_t *file;
    __fblob_state_t fblob_state;
    tt_blob_t fblob[__INLINE_FBLOB_NUM];
    tt_u64_t position;
    OVERLAPPED ov;

    tt_result_t result;
    tt_on_fwrite_t on_fwrite;
    void *cb_param;

    __FSAIO_DEBUG_FLAG_DECLARE
} __fwrite_t;

typedef struct
{
    tt_file_t *file;
    __fblob_state_t fblob_state;
    tt_u64_t position;
    OVERLAPPED ov;

    tt_result_t result;
    tt_on_fread_t on_fread;
    void *cb_param;
    tt_blob_t fblob[__INLINE_FBLOB_NUM];

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
static tt_bool_t __do_fclose(IN __fclose_t *aio, IN tt_result_t iocp_result);

// fseek
static tt_bool_t __do_fseek(IN __fseek_t *aio, IN tt_result_t iocp_result);

// fwrite
static tt_bool_t __do_fwrite(IN __fwrite_t *aio,
                             IN tt_result_t iocp_result,
                             IN tt_u32_t last_io_bytes);
static tt_result_t __do_fwrite_io(IN __fwrite_t *aio,
                                  IN tt_u32_t last_io_bytes);

// fread
static tt_bool_t __do_fread(IN __fread_t *aio,
                            IN tt_result_t iocp_result,
                            IN tt_u32_t last_io_bytes);
static tt_result_t __do_fread_io(IN __fread_t *aio, IN tt_u32_t last_io_bytes);

// faio queue
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
static tt_bool_t __update_fblob_state(IN __fblob_state_t *fblob_state,
                                      IN tt_u32_t io_bytes);

#ifdef __SIMULATE_FS_AIO_FAIL

#define tt_evc_sendto_thread tt_evc_sendto_thread__SF
static tt_result_t tt_evc_sendto_thread__SF(IN tt_evcenter_t *evc,
                                            IN tt_thread_ev_t *tev);

#define PostQueuedCompletionStatus PQCS__SF
static BOOL PQCS__SF(HANDLE CompletionPort,
                     DWORD dwNumberOfBytesTransferred,
                     ULONG_PTR dwCompletionKey,
                     LPOVERLAPPED lpOverlapped);

#define WriteFile WriteFile__SF
static BOOL WriteFile__SF(HANDLE hFile,
                          LPCVOID lpBuffer,
                          DWORD nNumberOfBytesToWrite,
                          LPDWORD lpNumberOfBytesWritten,
                          LPOVERLAPPED lpOverlapped);

#define ReadFile ReadFile__SF
static BOOL ReadFile__SF(HANDLE hFile,
                         LPVOID lpBuffer,
                         DWORD nNumberOfBytesToRead,
                         LPDWORD lpNumberOfBytesRead,
                         LPOVERLAPPED lpOverlapped);

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

    path_len = (tt_u32_t)tt_strlen(path) + 1;

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
        CloseHandle(sys_f->hf);
        sys_f->hf = INVALID_HANDLE_VALUE;
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
    tt_memset(&aio->ov, 0, sizeof(OVERLAPPED));

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add to q
    tt_list_addtail(&sys_f->aio_q, &tev->node);

    // notify to process
    if ((tt_list_count(&sys_f->aio_q) == 1) &&
        !PostQueuedCompletionStatus(evc->sys_evc.iocp,
                                    0,
                                    (ULONG_PTR)&sys_f->ev_mark,
                                    &aio->ov)) {
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
    tt_memset(&aio->ov, 0, sizeof(OVERLAPPED));

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;
    aio->position = -1;

    // add to q
    tt_list_addtail(&sys_f->aio_q, &tev->node);

    // notify to process
    if ((tt_list_count(&sys_f->aio_q) == 1) &&
        !PostQueuedCompletionStatus(evc->sys_evc.iocp,
                                    0,
                                    (ULONG_PTR)&sys_f->ev_mark,
                                    &aio->ov)) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_fwrite_async_ntv(IN tt_file_t *file,
                                IN tt_blob_t *blob,
                                IN tt_u32_t blob_num,
                                IN tt_u64_t position,
                                IN tt_on_fwrite_t on_fwrite,
                                IN void *cb_param)
{
    tt_file_ntv_t *sys_f;
    tt_evcenter_t *evc;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __fwrite_t *aio;
    __fblob_state_t *fblob_state;
    tt_u32_t i;

    TT_ASSERT(file != NULL);
    TT_ASSERT(blob != NULL);
    TT_ASSERT(blob_num != 0);
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

    for (i = 0; i < blob_num; ++i) {
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
    fblob_state = &aio->fblob_state;

    // init aio
    aio->file = file;

    fblob_state->blob_num = blob_num;
    if (blob_num <= __INLINE_FBLOB_NUM) {
        fblob_state->blob = aio->fblob;
    } else {
        fblob_state->blob = (tt_blob_t *)tt_mem_alloc(__FBLOB_SIZE(blob_num));
        if (fblob_state->blob == NULL) {
            TT_ERROR("no mem for buf array");

            // can not use tt_ev_destroy, as ev data is not consistent now
            tt_mem_free(ev);
            return TT_FAIL;
        }
    }
    for (i = 0; i < blob_num; ++i) {
        fblob_state->blob[i].addr = blob[i].addr;
        fblob_state->blob[i].len = blob[i].len;
    }
    fblob_state->cur_idx = 0;
    fblob_state->cur_pos = 0;
    fblob_state->io_bytes = 0;

    aio->position = position;
    tt_memset(&aio->ov, 0, sizeof(OVERLAPPED));

    aio->result = TT_PROCEEDING;
    aio->on_fwrite = on_fwrite;
    aio->cb_param = cb_param;

    __FSAIO_DEBUG_FLAG_SET(aio, 0);

    // add to q
    tt_list_addtail(&sys_f->aio_q, &tev->node);

    // notify to process
    if ((tt_list_count(&sys_f->aio_q) == 1) &&
        (__do_fwrite_io(aio, 0) != TT_PROCEEDING)) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_fread_async_ntv(IN tt_file_t *file,
                               IN tt_blob_t *blob,
                               IN tt_u32_t blob_num,
                               IN tt_u64_t position,
                               IN tt_on_fread_t on_fread,
                               IN void *cb_param)
{
    tt_file_ntv_t *sys_f;
    tt_evcenter_t *evc;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __fread_t *aio;
    __fblob_state_t *fblob_state;
    tt_u32_t i;

    TT_ASSERT(file != NULL);
    TT_ASSERT(blob != NULL);
    TT_ASSERT(blob_num != 0);
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

    for (i = 0; i < blob_num; ++i) {
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
    fblob_state = &aio->fblob_state;

    // init aio
    aio->file = file;

    fblob_state->blob_num = blob_num;
    if (blob_num <= __INLINE_FBLOB_NUM) {
        fblob_state->blob = aio->fblob;
    } else {
        fblob_state->blob = (tt_blob_t *)tt_mem_alloc(__FBLOB_SIZE(blob_num));
        if (fblob_state->blob == NULL) {
            TT_ERROR("no mem for buf array");

            tt_mem_free(ev);
            return TT_FAIL;
        }
    }
    for (i = 0; i < blob_num; ++i) {
        fblob_state->blob[i].addr = blob[i].addr;
        fblob_state->blob[i].len = blob[i].len;
    }
    fblob_state->cur_idx = 0;
    fblob_state->cur_pos = 0;
    fblob_state->io_bytes = 0;

    aio->position = position;
    tt_memset(&aio->ov, 0, sizeof(OVERLAPPED));

    aio->result = TT_PROCEEDING;
    aio->on_fread = on_fread;
    aio->cb_param = cb_param;

    __FSAIO_DEBUG_FLAG_SET(aio, 0);

    // add to q
    tt_list_addtail(&sys_f->aio_q, &tev->node);

    // notify to process
    if ((tt_list_count(&sys_f->aio_q) == 1) &&
        (__do_fread_io(aio, 0) != TT_PROCEEDING)) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
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

tt_result_t tt_file_iocp_handler(IN tt_file_t *file,
                                 IN DWORD NumberOfBytes,
                                 IN OVERLAPPED *Overlapped,
                                 IN tt_result_t iocp_result)
{
    tt_file_ntv_t *sys_f = &file->sys_file;
    tt_lnode_t *node;

    while ((node = tt_list_head(&sys_f->aio_q)) != NULL) {
        tt_thread_ev_t *tev = TT_CONTAINER(node, tt_thread_ev_t, node);
        tt_ev_t *ev = TT_EV_OF(tev);
        tt_bool_t aio_done = TT_TRUE;

        switch (ev->ev_id) {
            case EV_FCLOSE: {
                __fclose_t *aio = TT_EV_DATA(ev, __fclose_t);

                TT_ASSERT_FSAIO(sys_f == &aio->file->sys_file);
                TT_ASSERT_FSAIO((Overlapped == NULL) ||
                                (Overlapped == &aio->ov));

                if (tt_list_count(&sys_f->aio_q) != 1) {
                    TT_WARN("more aio after fclose");
                }

                tt_list_remove(node);
                __do_fclose(aio, iocp_result);
                tt_ev_destroy(ev);
                return TT_SUCCESS;
            } break;
            case EV_FSEEK: {
                __fseek_t *aio = TT_EV_DATA(ev, __fseek_t);

                TT_ASSERT_FSAIO(sys_f == &aio->file->sys_file);
                TT_ASSERT_FSAIO((Overlapped == NULL) ||
                                (Overlapped == &aio->ov));

                aio_done = __do_fseek(aio, iocp_result);
            } break;
            case EV_FWRITE: {
                __fwrite_t *aio = TT_EV_DATA(ev, __fwrite_t);

                TT_ASSERT_FSAIO(sys_f == &aio->file->sys_file);
                TT_ASSERT_FSAIO((Overlapped == NULL) ||
                                (Overlapped == &aio->ov));

                aio_done = __do_fwrite(aio, iocp_result, NumberOfBytes);
            } break;
            case EV_FREAD: {
                __fread_t *aio = TT_EV_DATA(ev, __fread_t);

                TT_ASSERT_FSAIO(sys_f == &aio->file->sys_file);
                TT_ASSERT_FSAIO((Overlapped == NULL) ||
                                (Overlapped == &aio->ov));

                aio_done = __do_fread(aio, iocp_result, NumberOfBytes);
            } break;

            default: {
                TT_ERROR("unknown aio: %x", ev->ev_id);
            } break;
        }

        if (!aio_done) {
            return TT_SUCCESS;
        }
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);

        // pass to next aio
        NumberOfBytes = 0;
        Overlapped = NULL;
        iocp_result = TT_SUCCESS;
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

        // unknown
        default: {
            TT_ERROR("unknown event: %x", ev->ev_id);
            tt_ev_destroy(ev);
        } break;
    }
    return TT_SUCCESS;
}

tt_result_t tt_dir_iocp_handler(IN tt_dir_t *dir,
                                IN DWORD NumberOfBytes,
                                IN OVERLAPPED *Overlapped,
                                IN tt_result_t iocp_result)
{
    TT_ERROR("should not call tt_dir_iocp_handler");
    return TT_PROCEEDING;
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
        sys_f->evc = aio->evc;

#if 0 // by test, FILE_SKIP_COMPLETION_PORT_ON_SUCCESS never take effect...
        // set skip completion
        if (!SetFileCompletionNotificationModes(sys_f->hf,
                                    FILE_SKIP_COMPLETION_PORT_ON_SUCCESS))
        {
            TT_ERROR_NTV("fail to set file modes");
            aio->result = TT_FAIL;
            tt_fclose(file);
            break;
        }
#endif

        // associating file with iocp
        if (CreateIoCompletionPort(sys_f->hf,
                                   aio->evc->sys_evc.iocp,
                                   (ULONG_PTR)(&sys_f->ev_mark),
                                   0) == NULL) {
            TT_ERROR_NTV("fail to associate file with iocp");
            aio->result = TT_FAIL;
            tt_fclose(file);
        }

        file->aio_enable = TT_TRUE;
    }

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_fopen(aio->file, aio->path, aio->flag, &aio->attr, &aioctx);
}

tt_bool_t __do_fclose(IN __fclose_t *aio, IN tt_result_t iocp_result)
{
    tt_faioctx_t aioctx;

    aio->result = tt_fclose(aio->file);

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_fclose(aio->file, &aioctx);

    return TT_TRUE;
}

tt_bool_t __do_fseek(IN __fseek_t *aio, IN tt_result_t iocp_result)
{
    tt_file_ntv_t *sys_f = &aio->file->sys_file;
    tt_faioctx_t aioctx;
    tt_s64_t position;

    if (TT_OK(iocp_result)) {
        switch (aio->whence) {
            case TT_FPOS_BEGIN: {
                position = aio->distance;
            } break;
            case TT_FPOS_CUR: {
                position = sys_f->pos + aio->distance;
            } break;
            case TT_FPOS_END: {
                tt_u64_t n;
                if (TT_OK(tt_fseek(aio->file, TT_FPOS_END, 0, &n))) {
                    position = n + aio->distance;
                } else {
                    position = -1;
                }
            } break;
            default: {
                position = -1;
            } break;
        }
        if (position >= 0) {
            sys_f->pos = position;

            aio->result = TT_SUCCESS;
            aio->position = position;
        } else {
            aio->result = TT_FAIL;
            aio->position = -1;
        }
    } else {
        aio->result = iocp_result;
    }
    TT_ASSERT(aio->result != TT_PROCEEDING);

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

tt_bool_t __do_fwrite(IN __fwrite_t *aio,
                      IN tt_result_t iocp_result,
                      IN tt_u32_t last_io_bytes)
{
    tt_faioctx_t aioctx;
    __fblob_state_t *fblob_state = &aio->fblob_state;

    if (TT_OK(iocp_result)) {
        aio->result = __do_fwrite_io(aio, last_io_bytes);
        if (aio->result == TT_PROCEEDING) {
            return TT_FALSE;
        }
    } else if (fblob_state->io_bytes > 0) {
        aio->result = TT_SUCCESS;
    } else {
        aio->result = iocp_result;
    }

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_fwrite(aio->file,
                   (tt_blob_t *)fblob_state->blob,
                   fblob_state->blob_num,
                   aio->position,
                   &aioctx,
                   fblob_state->io_bytes);

    return TT_TRUE;
}

tt_result_t __do_fwrite_io(IN __fwrite_t *aio, IN tt_u32_t last_io_bytes)
{
    tt_file_ntv_t *sys_f = &aio->file->sys_file;
    __fblob_state_t *fblob_state = &aio->fblob_state;
    tt_bool_t all_done = TT_FALSE;
    OVERLAPPED *ov = &aio->ov;

    all_done = __update_fblob_state(fblob_state, last_io_bytes);
    sys_f->pos += last_io_bytes;
    if (all_done) {
        return TT_SUCCESS;
    }
    TT_ASSERT_FSAIO(fblob_state->cur_idx < fblob_state->blob_num);

    if (aio->position == TT_FPOS_NULL) {
        tt_blob_t *cur_b = &fblob_state->blob[fblob_state->cur_idx];
        DWORD NumberOfBytesWritten = 0;
        DWORD dwError;

        TT_ASSERT_FSAIO(fblob_state->cur_pos < cur_b->len);

        ov->Offset = (tt_u32_t)sys_f->pos;
        ov->OffsetHigh = (tt_u32_t)(sys_f->pos >> 32);

        if (!WriteFile(sys_f->hf,
                       TT_PTR_INC(tt_u8_t, cur_b->addr, fblob_state->cur_pos),
                       cur_b->len - fblob_state->cur_pos,
                       &NumberOfBytesWritten,
                       ov) &&
            ((dwError = GetLastError()) != ERROR_IO_PENDING)) {
            TT_ERROR_NTV("fwrite fail");

            if (fblob_state->io_bytes > 0) {
                return TT_SUCCESS;
            } else {
                return TT_FAIL;
            }
        }
        return TT_PROCEEDING;
    } else {
        tt_blob_t *cur_b = &fblob_state->blob[fblob_state->cur_idx];
        tt_s64_t new_pos = aio->position + fblob_state->io_bytes;
        DWORD NumberOfBytesWritten = 0;
        DWORD dwError;

        TT_ASSERT_FSAIO(fblob_state->cur_pos < cur_b->len);

        ov->Offset = (tt_u32_t)new_pos;
        ov->OffsetHigh = (tt_u32_t)(new_pos >> 32);

        if (!WriteFile(sys_f->hf,
                       TT_PTR_INC(tt_u8_t, cur_b->addr, fblob_state->cur_pos),
                       cur_b->len - fblob_state->cur_pos,
                       &NumberOfBytesWritten,
                       ov) &&
            ((dwError = GetLastError()) != ERROR_IO_PENDING)) {
            TT_ERROR_NTV("fwrite fail");

            if (fblob_state->io_bytes > 0) {
                return TT_SUCCESS;
            } else {
                return TT_FAIL;
            }
        }
        return TT_PROCEEDING;
    }
}

void __fwrite_on_destroy(IN struct tt_ev_s *ev)
{
    __fwrite_t *aio = TT_EV_DATA(ev, __fwrite_t);
    __fblob_state_t *fblob_state = &aio->fblob_state;

    if (fblob_state->blob != (tt_blob_t *)aio->fblob) {
        tt_mem_free(fblob_state->blob);
    }
}

tt_bool_t __do_fread(IN __fread_t *aio,
                     IN tt_result_t iocp_result,
                     IN tt_u32_t last_io_bytes)
{
    tt_faioctx_t aioctx;
    __fblob_state_t *fblob_state = &aio->fblob_state;

    if (TT_OK(iocp_result)) {
        aio->result = __do_fread_io(aio, last_io_bytes);
        if (aio->result == TT_PROCEEDING) {
            return TT_FALSE;
        }
    } else if (fblob_state->io_bytes > 0) {
        aio->result = TT_SUCCESS;
    } else {
        aio->result = iocp_result;
    }

    // notification
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_fread(aio->file,
                  (tt_blob_t *)fblob_state->blob,
                  fblob_state->blob_num,
                  aio->position,
                  &aioctx,
                  fblob_state->io_bytes);

    return TT_TRUE;
}

tt_result_t __do_fread_io(IN __fread_t *aio, IN tt_u32_t last_io_bytes)
{
    tt_file_ntv_t *sys_f = &aio->file->sys_file;
    __fblob_state_t *fblob_state = &aio->fblob_state;
    tt_bool_t all_done = TT_FALSE;
    OVERLAPPED *ov = &aio->ov;

    all_done = __update_fblob_state(fblob_state, last_io_bytes);
    sys_f->pos += last_io_bytes;
    if (all_done) {
        return TT_SUCCESS;
    }
    TT_ASSERT_FSAIO(fblob_state->cur_idx < fblob_state->blob_num);

    if (aio->position == TT_FPOS_NULL) {
        tt_blob_t *cur_b = &fblob_state->blob[fblob_state->cur_idx];
        DWORD NumberOfBytesRead = 0;
        DWORD dwError;

        TT_ASSERT_FSAIO(fblob_state->cur_pos < cur_b->len);

        ov->Offset = (tt_u32_t)sys_f->pos;
        ov->OffsetHigh = (tt_u32_t)(sys_f->pos >> 32);

        if (!ReadFile(sys_f->hf,
                      TT_PTR_INC(tt_u8_t, cur_b->addr, fblob_state->cur_pos),
                      cur_b->len - fblob_state->cur_pos,
                      &NumberOfBytesRead,
                      ov) &&
            ((dwError = GetLastError()) != ERROR_IO_PENDING)) {
            if (fblob_state->io_bytes > 0) {
                return TT_SUCCESS;
            } else if (dwError == ERROR_HANDLE_EOF) {
                return TT_END;
            } else {
                return TT_FAIL;
            }
        }
        return TT_PROCEEDING;
    } else {
        tt_blob_t *cur_b = &fblob_state->blob[fblob_state->cur_idx];
        tt_s64_t new_pos = aio->position + fblob_state->io_bytes;
        DWORD NumberOfBytesRead = 0;
        DWORD dwError;

        TT_ASSERT_FSAIO(fblob_state->cur_pos < cur_b->len);

        ov->Offset = (tt_u32_t)new_pos;
        ov->OffsetHigh = (tt_u32_t)(new_pos >> 32);

        if (!ReadFile(sys_f->hf,
                      TT_PTR_INC(tt_u8_t, cur_b->addr, fblob_state->cur_pos),
                      cur_b->len - fblob_state->cur_pos,
                      &NumberOfBytesRead,
                      ov) &&
            ((dwError = GetLastError()) != ERROR_IO_PENDING)) {
            if (fblob_state->io_bytes > 0) {
                return TT_SUCCESS;
            } else if (dwError == ERROR_HANDLE_EOF) {
                return TT_END;
            } else {
                return TT_FAIL;
            }
        }
        return TT_PROCEEDING;
    }
}

void __fread_on_destroy(IN struct tt_ev_s *ev)
{
    __fread_t *aio = TT_EV_DATA(ev, __fread_t);
    __fblob_state_t *fblob_state = &aio->fblob_state;

    if (fblob_state->blob != (tt_blob_t *)aio->fblob) {
        tt_mem_free(fblob_state->blob);
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
            tt_thread_ev_create(EV_DAIO_Q, sizeof(__daio_q_t), NULL);
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

void __free_faio_q(IN tt_file_ntv_t *sys_f)
{
    tt_lnode_t *node;
    while ((node = tt_list_pophead(&sys_f->aio_q)) != NULL) {
        tt_ev_destroy(TT_EV_OF(TT_CONTAINER(node, tt_thread_ev_t, node)));
    }
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

// return true if buffers are full
tt_bool_t __update_fblob_state(IN __fblob_state_t *fblob_state,
                               IN tt_u32_t io_bytes)
{
    // update send_len
    fblob_state->io_bytes += io_bytes;

    // update buf array
    while (fblob_state->cur_idx < fblob_state->blob_num) {
        tt_blob_t *cur_b = &fblob_state->blob[fblob_state->cur_idx];
        tt_u32_t left_space;

        TT_ASSERT_FSAIO(fblob_state->cur_pos < cur_b->len);
        left_space = cur_b->len - fblob_state->cur_pos;

        if (io_bytes < left_space) {
            fblob_state->cur_pos += io_bytes;
            break;
        }
        io_bytes -= left_space;

        // buf full, move to next
        ++fblob_state->cur_idx;
        fblob_state->cur_pos = 0;
    };
    TT_ASSERT_FSAIO(fblob_state->cur_idx <= fblob_state->blob_num);

    if (fblob_state->cur_idx == fblob_state->blob_num) {
        TT_ASSERT_FSAIO(io_bytes == 0);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

#ifdef __SIMULATE_FS_AIO_FAIL

tt_result_t tt_evc_sendto_thread__SF(IN tt_evcenter_t *evc,
                                     IN tt_thread_ev_t *tev)
{
    if ((rand() % 4) == 0) {
        return TT_FAIL;
    } else {
#undef tt_evc_sendto_thread
        return tt_evc_sendto_thread(evc, tev);
    }
}

BOOL PQCS__SF(HANDLE CompletionPort,
              DWORD dwNumberOfBytesTransferred,
              ULONG_PTR dwCompletionKey,
              LPOVERLAPPED lpOverlapped)
{
    if ((rand() % 4) == 0) {
        return FALSE;
    } else {
#undef PostQueuedCompletionStatus
        return PostQueuedCompletionStatus(CompletionPort,
                                          dwNumberOfBytesTransferred,
                                          dwCompletionKey,
                                          lpOverlapped);
    }
}

BOOL WriteFile__SF(HANDLE hFile,
                   LPCVOID lpBuffer,
                   DWORD nNumberOfBytesToWrite,
                   LPDWORD lpNumberOfBytesWritten,
                   LPOVERLAPPED lpOverlapped)
{
    int r = rand() % 3;
    if (r == 0) {
        *lpNumberOfBytesWritten = nNumberOfBytesToWrite - 1;
        return TRUE;
    } else if (r == 1) {
        *lpNumberOfBytesWritten = ~0;

        // must set last error, otherwise caller may get io_pending
        // which is actually the result of LAST io request
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    } else {
#undef WriteFile
        return WriteFile(hFile,
                         lpBuffer,
                         nNumberOfBytesToWrite,
                         lpNumberOfBytesWritten,
                         lpOverlapped);
    }
}

BOOL ReadFile__SF(HANDLE hFile,
                  LPVOID lpBuffer,
                  DWORD nNumberOfBytesToRead,
                  LPDWORD lpNumberOfBytesRead,
                  LPOVERLAPPED lpOverlapped)
{
    int r = rand() % 3;
    if (r == 0) {
        *lpNumberOfBytesRead = nNumberOfBytesToRead - 1;
        return TRUE;
    } else if (r == 1) {
        *lpNumberOfBytesRead = ~0;

        // must set last error, otherwise caller may get io_pending
        // which is actually the result of LAST io request
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    } else {
#undef ReadFile
        return ReadFile(hFile,
                        lpBuffer,
                        nNumberOfBytesToRead,
                        lpNumberOfBytesRead,
                        lpOverlapped);
    }
}
#endif
