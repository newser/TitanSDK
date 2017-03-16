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
@file tt_file_system_event.h
@brief file system io io_event
*/

#ifndef __TT_FILE_SYSTEM_EVENT__
#define __TT_FILE_SYSTEM_EVENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_io_event.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_file_s;
struct tt_file_attr_s;

enum
{
    TT_FS_FCREATE,
    TT_FS_FOPEN,

    TT_FS_NUM
};

typedef struct
{
    tt_io_ev_t io_ev;

    const tt_char_t *path;
    struct tt_file_attr_s *attr;

    tt_result_t result;
} tt_fs_fcreate_t;

typedef struct
{
    tt_io_ev_t io_ev;

    struct tt_file_s *file;
    const tt_char_t *path;
    struct tt_file_attr_s *attr;

    tt_result_t result;
} tt_fs_fopen_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_fs_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t fs_ev);

extern void tt_fs_ev_handler(IN tt_io_ev_t *io_ev);

#endif // __TT_FILE_SYSTEM_EVENT__
