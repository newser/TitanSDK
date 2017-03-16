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
#include <io/tt_io_event.h>
#include <io/tt_io_worker_group.h>
#include <os/tt_fiber.h>

#include <tt_util_native.h>

#include <fcntl.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __FCREATE,
    __FOPEN,

    __FS_IO_NUM,
};

typedef struct
{
    tt_io_ev_t io_ev;

    const tt_char_t *path;
    struct tt_file_attr_s *attr;

    tt_result_t result;
} __fcreate_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __do_fcreate(IN tt_io_ev_t *io_ev);

static tt_io_handler_t __fs_io_handler[__FS_IO_NUM] = {
    __do_fcreate,
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

    tt_iowg_push_ev(&tt_g_iowg, &fcreate.io_ev);
    tt_fiber_yield();
    return fcreate.result;
}

tt_result_t tt_fremove_ntv(IN const tt_char_t *path)
{
    if (unlink(path) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to remove %s", path);
        return TT_FAIL;
    }
}

// this function can not open directory
tt_result_t tt_fopen_ntv(OUT tt_file_ntv_t *file,
                         IN const tt_char_t *path,
                         IN tt_u32_t flag,
                         IN struct tt_file_attr_s *attr)
{
}

tt_result_t tt_fclose_ntv(IN tt_file_ntv_t *file);

tt_result_t tt_fseek_ntv(IN tt_file_ntv_t *file,
                         IN tt_u32_t whence,
                         IN tt_s64_t distance,
                         OUT tt_u64_t *position)
{
}

// avoid calling fread and fwrite by multi thread at same time
tt_result_t tt_fread_ntv(IN tt_file_ntv_t *file,
                         OUT tt_u8_t *buf,
                         IN tt_u32_t buf_len,
                         OUT tt_u32_t *read_len)
{
    ssize_t __read_len;

    __read_len = read(file->fd, buf, buf_len);
    if (__read_len > 0) {
        if (read_len != NULL) {
            *read_len = __read_len;
        }
        return TT_SUCCESS;
    } else if (__read_len == 0) {
        return TT_END;
    } else {
        TT_ERROR_NTV("read fail");
        return TT_FAIL;
    }
}

tt_result_t tt_fwrite_ntv(IN tt_file_ntv_t *file,
                          IN tt_u8_t *buf,
                          IN tt_u32_t buf_len,
                          OUT tt_u32_t *write_len)
{
    ssize_t __write_len;

    __write_len = write(file->fd, buf, buf_len);
    if (__write_len >= 0) {
        if (write_len != NULL) {
            *write_len = __write_len;
        }
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("write fail");
        return TT_FAIL;
    }
}

void tt_fs_io_handler(IN tt_io_ev_t *io_ev)
{
    __fs_io_handler[io_ev->ev](io_ev);
}

void __fs_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev)
{
    io_ev->src = tt_current_fiber();
    tt_dnode_init(&io_ev->node);
    io_ev->io = TT_IO_FS;
    io_ev->ev = ev;
}

void __do_fcreate(IN tt_io_ev_t *io_ev)
{
    __fcreate_t *fcreate = (__fcreate_t *)io_ev;

    int fd, oflag = O_CREAT | O_EXCL;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

again:
    fd = open(fcreate->path, oflag, mode);
    if (fd >= 0) {
        __RETRY_IF_EINTR(close(fd));
        fcreate->result = TT_SUCCESS;
    } else if (errno == EINTR) {
        goto again;
    } else {
        TT_ERROR_NTV("fail to create file: %s", fcreate->path);
        fcreate->result = TT_FAIL;
    }
}
