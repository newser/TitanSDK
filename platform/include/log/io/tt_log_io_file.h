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

/**
@file tt_log_io_file.h
@brief log io file

this file defines log io file output
*/

#ifndef __TT_LOG_IO_FILE__
#define __TT_LOG_IO_FILE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_file_system.h>
#include <os/tt_task.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_logio_s;

typedef enum {
    TT_LOGFILE_SUFFIX_INDEX,
    TT_LOGFILE_SUFFIX_DATE,

    TT_LOGFILE_SUFFIX_NUM
} tt_logfile_suffix_t;
#define TT_LOGFILE_SUFFIX_VALID(n) ((n) < TT_LOGFILE_SUFFIX_NUM)

typedef enum {
    TT_LOGFILE_ARCHIVE_NONE,
    TT_LOGFILE_ARCHIVE_ZIP,

    TT_LOGFILE_ARCHIVE_NUM
} tt_logfile_archive_t;
#define TT_LOGFILE_ARCHIVE_VALID(n) ((n) < TT_LOGFILE_ARCHIVE_NUM)

typedef enum {
    TT_LOGFILE_PURGE_NONE,
    TT_LOGFILE_PURGE_REMOVE,
    TT_LOGFILE_PURGE_SFTP,

    TT_LOGFILE_PURGE_NUM
} tt_logfile_purge_t;
#define TT_LOGFILE_PURGE_VALID(n) ((n) < TT_LOGFILE_PURGE_NUM)

typedef struct
{
    const tt_char_t *log_name;
    const tt_char_t *archive_name;
    const tt_char_t *date_format;
    tt_logfile_suffix_t log_suffix;
    tt_logfile_purge_t log_purge;
    tt_u32_t keep_log_sec;
    tt_u32_t keep_archive_sec;
    tt_u32_t max_log_size_order;
} tt_logio_file_attr_t;

typedef struct
{
    const tt_char_t *log_path;
    const tt_char_t *log_name;
    const tt_char_t *archive_path;
    const tt_char_t *archive_name;
    tt_task_t worker;
    tt_file_t f;
    tt_logfile_suffix_t log_suffix;
    tt_logfile_purge_t log_purge;
    tt_u32_t keep_log_sec;
    tt_u32_t keep_archive_sec;
    tt_u32_t max_log_size;
    tt_u32_t write_len;

    union
    {
        struct
        {
            tt_u32_t index;
        } fidx;
        struct
        {
            const tt_char_t *date_format;
        } fdate;
    } u_suffix;

    union
    {
        struct
        {
            tt_u32_t reserved;
        } sftp;
    } u_purge;

    tt_bool_t f_opened : 1;
    tt_bool_t worker_running : 1;
} tt_logio_file_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_logio_s *tt_logio_file_create(
    IN const tt_char_t *log_path,
    IN const tt_char_t *archive_path,
    IN OPT tt_logio_file_attr_t *attr);

tt_export void tt_logio_file_attr_default(IN tt_logio_file_attr_t *attr);

#endif /* __TT_LOG_IO_FILE__ */
