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

#include <io/tt_file_system_event.h>

#include <os/tt_fiber.h>

#include <tt_file_system_native.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef void (*__fs_handler_t)(IN tt_io_ev_t *io_ev);

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __do_fopen(IN tt_io_ev_t *io_ev);

static __fs_handler_t __fs_handler[TT_FS_NUM] = {
    __do_fopen,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_fs_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t fs)
{
    io_ev->src = tt_current_fiber();
    tt_dnode_init(&io_ev->node);
    io_ev->io = TT_IO_FS;
    io_ev->ev = fs;
}

void tt_fs_ev_handler(IN tt_io_ev_t *io_ev)
{
    __fs_handler[io_ev->io](io_ev);

    if (io_ev->src != NULL) {
        tt_fiber_activate(io_ev->src);
    }
}

void __do_fopen(IN tt_io_ev_t *io_ev)
{
    tt_fs_fopen_t *fopen = (tt_fs_fopen_t *)io_ev;

    /*
    fopen->result =
        tt_fopen_ntv(fopen->file, fopen->path, fopen->attr);
     */
}
