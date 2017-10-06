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

#include <log/io/tt_log_io_file.h>

#include <algorithm/tt_string_common.h>
#include <io/tt_fpath.h>
#include <log/io/tt_log_io.h>
#include <misc/tt_util.h>
#include <time/tt_time_reference.h>

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

static tt_u32_t __lio_fidx_output(IN tt_logio_t *lio,
                                  IN const tt_char_t *data,
                                  IN tt_u32_t data_len);

static tt_logio_itf_t tt_s_logio_fidx_itf = {
    TT_LOGIO_FILE,

    NULL,
    NULL,
    __lio_fidx_output,
};

static tt_u32_t __lio_fdate_output(IN tt_logio_t *lio,
                                   IN const tt_char_t *data,
                                   IN tt_u32_t data_len);

static tt_logio_itf_t tt_s_logio_fdate_itf = {
    TT_LOGIO_FILE,

    NULL,
    NULL,
    __lio_fdate_output,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __fidx_next(IN tt_logio_file_t *lio_file);

static tt_result_t __fdate_next(IN tt_logio_file_t *lio_file);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_logio_t *tt_logio_file_create(IN const tt_char_t *log_path,
                                 IN const tt_char_t *archive_path,
                                 IN OPT tt_logio_file_attr_t *attr)
{
    tt_logio_file_attr_t __attr;
    tt_logio_t *lio;
    tt_logio_file_t *lio_file;
    tt_result_t result;

    TT_ASSERT(log_path != NULL);
    TT_ASSERT(archive_path != NULL);

    if (attr == NULL) {
        tt_logio_file_attr_default(&__attr);
        attr = &__attr;
    }

    TT_ASSERT(TT_LOGFILE_SUFFIX_VALID(attr->log_suffix));
    if (attr->log_suffix == TT_LOGFILE_SUFFIX_INDEX) {
        lio = tt_logio_create(sizeof(tt_logio_file_t), &tt_s_logio_fidx_itf);
    } else if (attr->log_suffix == TT_LOGFILE_SUFFIX_DATE) {
        lio = tt_logio_create(sizeof(tt_logio_file_t), &tt_s_logio_fdate_itf);
    } else {
        // never reach here
        lio = NULL;
    }
    if (lio == NULL) {
        return NULL;
    }

    lio_file = TT_LOGIO_CAST(lio, tt_logio_file_t);

    // init common
    lio_file->keep_log_time = tt_time_ms2ref(attr->keep_log_sec * 1000);
    lio_file->keep_archive_time = tt_time_ms2ref(attr->keep_archive_sec * 1000);
    lio_file->log_path = log_path;
    lio_file->log_name = attr->log_name;
    lio_file->archive_path = archive_path;
    lio_file->archive_name = attr->archive_name;
    lio_file->log_suffix = attr->log_suffix;
    TT_ASSERT(TT_LOGFILE_SUFFIX_VALID(attr->archive_suffix));
    lio_file->archive_suffix = attr->archive_suffix;
    if (attr->max_log_size_order > 30) {
        attr->max_log_size_order = 30;
    }
    lio_file->max_log_size = 1 << attr->max_log_size_order;
    lio_file->write_len = 0;

    // init specific
    result = TT_FAIL;
    if (attr->log_suffix == TT_LOGFILE_SUFFIX_INDEX) {
        lio_file->index = 0;

        result = __fidx_next(lio_file);
    } else if (attr->log_suffix == TT_LOGFILE_SUFFIX_DATE) {
        result = __fdate_next(lio_file);
    }

    if (TT_OK(result)) {
        return lio;
    } else {
        tt_free(lio);
        return NULL;
    }
}

void tt_logio_file_attr_default(IN tt_logio_file_attr_t *attr)
{
    attr->log_name = "log";
    attr->archive_name = "archive";
    attr->log_suffix = TT_LOGFILE_SUFFIX_INDEX;
    attr->archive_suffix = TT_LOGFILE_SUFFIX_DATE;
    attr->keep_log_sec = 3600;
    attr->keep_archive_sec = 24 * 3600;
    attr->max_log_size_order = 20;
}

tt_u32_t __lio_fidx_output(IN tt_logio_t *lio,
                           IN const tt_char_t *data,
                           IN tt_u32_t data_len)
{
    tt_logio_file_t *lio_file = TT_LOGIO_CAST(lio, tt_logio_file_t);
    tt_u32_t write_len;

    // this function is already protected by log manger's lock

    if (TT_OK(tt_fwrite(&lio_file->f, (tt_u8_t *)data, data_len, &write_len))) {
        lio_file->write_len += write_len;
    }

    if (lio_file->write_len >= lio_file->max_log_size) {
        tt_fclose(&lio_file->f);
        __fidx_next(lio_file);
    }

    return write_len;
}

tt_u32_t __lio_fdate_output(IN tt_logio_t *lio,
                            IN const tt_char_t *data,
                            IN tt_u32_t data_len)
{
    tt_logio_file_t *lio_file = TT_LOGIO_CAST(lio, tt_logio_file_t);
    tt_u32_t write_len;

    // this function is already protected by log manger's lock

    if (TT_OK(tt_fwrite(&lio_file->f, (tt_u8_t *)data, data_len, &write_len))) {
        lio_file->write_len += write_len;
    }

    if (lio_file->write_len >= lio_file->max_log_size) {
        tt_fclose(&lio_file->f);
        __fdate_next(lio_file);
    }

    return write_len;
}

tt_result_t __fidx_next(IN tt_logio_file_t *lio_file)
{
    tt_fpath_t path;
    tt_result_t result = TT_FAIL;

    // construct log file path
    tt_fpath_init(&path, TT_FPATH_AUTO);
    TT_DO_G(path_done, tt_fpath_set(&path, lio_file->log_path));
    TT_DO_G(path_done, tt_fpath_to_dir(&path));
    TT_DO_G(path_done, tt_fpath_set_basename(&path, lio_file->log_name));
    TT_DO_G(path_done, tt_fpath_set_extension(&path, lio_file->index));
    result = TT_SUCCESS;
path_done:
    if (!TT_OK(result)) {
        tt_fpath_destroy(&path);
        return result;
    }

    result = tt_fopen(&lio_file->f,
                      tt_fpath_cstr(&path),
                      TT_FO_WRITE | TT_FO_APPEND | TT_FO_EXCL,
                      NULL);
    tt_fpath_destroy(&path);
    lio_file->write_len = 0;
    return result;
}

tt_result_t __fdate_next(IN tt_logio_file_t *lio_file)
{
    tt_fpath_t path;
    tt_result_t result = TT_FAIL;

    // construct log file path
    tt_fpath_init(&path, TT_FPATH_AUTO);
    TT_DO_G(path_done, tt_fpath_set(&path, lio_file->log_path));
    TT_DO_G(path_done, tt_fpath_to_dir(&path));
    TT_DO_G(path_done, tt_fpath_set_basename(&path, lio_file->log_name));
    TT_DO_G(path_done, tt_fpath_set_extension(&path, lio_file->index));
    result = TT_SUCCESS;
path_done:
    if (!TT_OK(result)) {
        tt_fpath_destroy(&path);
        return result;
    }

    result = tt_fopen(&lio_file->f,
                      tt_fpath_cstr(&path),
                      TT_FO_WRITE | TT_FO_APPEND | TT_FO_EXCL,
                      NULL);
    tt_fpath_destroy(&path);
    lio_file->write_len = 0;
    return result;
}
