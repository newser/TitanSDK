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

#include <io/tt_fpath.h>
#include <log/io/tt_log_io.h>
#include <misc/tt_util.h>
#include <os/tt_thread.h>
#include <time/tt_date_format.h>
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

// ========================================
// log io file by index
// ========================================

static tt_u32_t __lio_fidx_output(IN tt_logio_t *lio,
                                  IN const tt_char_t *data,
                                  IN tt_u32_t data_len);

static tt_logio_itf_t tt_s_logio_fidx_itf = {
    TT_LOGIO_FILE,

    NULL,
    NULL,
    __lio_fidx_output,
};

// ========================================
// log io file by date
// ========================================

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

static tt_result_t __fidx_next(IN tt_logio_file_t *lf);

static tt_result_t __fdate_next(IN tt_logio_file_t *lf);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_logio_t *tt_logio_file_create(IN const tt_char_t *log_path,
                                 IN const tt_char_t *archive_path,
                                 IN OPT tt_logio_file_attr_t *attr)
{
    tt_logio_file_attr_t __attr;
    tt_logio_t *lio;
    tt_logio_file_t *lf;
    tt_result_t result;

    TT_ASSERT(log_path != NULL);
    TT_ASSERT(archive_path != NULL);

    if (attr == NULL) {
        tt_logio_file_attr_default(&__attr);
        attr = &__attr;
    }
    TT_ASSERT(TT_LOGFILE_SUFFIX_VALID(attr->log_suffix));
    TT_ASSERT(TT_LOGFILE_SUFFIX_VALID(attr->archive_suffix));

    if (attr->log_suffix == TT_LOGFILE_SUFFIX_INDEX) {
        lio = tt_logio_create(sizeof(tt_logio_file_t), &tt_s_logio_fidx_itf);
    } else {
        TT_ASSERT(attr->log_suffix == TT_LOGFILE_SUFFIX_DATE);
        TT_ASSERT(attr->date_format != NULL);
        lio = tt_logio_create(sizeof(tt_logio_file_t), &tt_s_logio_fdate_itf);
    }
    if (lio == NULL) {
        TT_ERROR("no mem for log io file");
        return NULL;
    }

    lf = TT_LOGIO_CAST(lio, tt_logio_file_t);

    // init common
    lf->keep_log_time = tt_time_ms2ref(attr->keep_log_sec * 1000);
    lf->keep_archive_time = tt_time_ms2ref(attr->keep_archive_sec * 1000);
    lf->log_path = log_path;
    lf->log_name = attr->log_name;
    lf->archive_path = archive_path;
    lf->archive_name = attr->archive_name;
    lf->log_suffix = attr->log_suffix;
    lf->archive_suffix = attr->archive_suffix;
    if (attr->max_log_size_order > 30) {
        lf->max_log_size = 1 << 30;
    } else {
        lf->max_log_size = 1 << attr->max_log_size_order;
    }
    lf->write_len = 0;
    lf->f_opened = TT_FALSE;

    // init specific
    tt_memset(&lf->u, 0, sizeof(lf->u));
    if (attr->log_suffix == TT_LOGFILE_SUFFIX_INDEX) {
        lf->u.fidx.index = 1;
        result = __fidx_next(lf);
    } else {
        TT_ASSERT(attr->log_suffix == TT_LOGFILE_SUFFIX_DATE);
        lf->u.fdate.date_format = attr->date_format;
        result = __fdate_next(lf);
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
    attr->date_format = "%C%N%DT%H%M%S";
    attr->log_suffix = TT_LOGFILE_SUFFIX_INDEX;
    attr->archive_suffix = TT_LOGFILE_SUFFIX_DATE;
    attr->keep_log_sec = 3600;
    attr->keep_archive_sec = 24 * 3600;
    attr->max_log_size_order = 20;
}

// ========================================
// log io file by index
// ========================================

tt_u32_t __lio_fidx_output(IN tt_logio_t *lio,
                           IN const tt_char_t *data,
                           IN tt_u32_t data_len)
{
    tt_logio_file_t *lf = TT_LOGIO_CAST(lio, tt_logio_file_t);
    tt_u32_t write_len = 0;
    tt_thread_t *t = tt_current_thread();

    // this function is already protected by log manger's lock

    t->log_std = TT_TRUE;

    if (!lf->f_opened) {
        // it ever failed to open log file, which is not expected. but here we
        // try to open the log file again
        __fidx_next(lf);
    }

    if (lf->f_opened &&
        TT_OK(tt_fwrite(&lf->f, (tt_u8_t *)data, data_len, &write_len))) {
        lf->write_len += write_len;
        if (lf->write_len >= lf->max_log_size) {
            __fidx_next(lf);
        }
    } else {
        // no way to recover anything, just give a warning
        TT_ERROR("log is lost");
    }

    t->log_std = TT_FALSE;
    return write_len;
}

tt_result_t __fidx_next(IN tt_logio_file_t *lf)
{
    tt_fpath_t path;
    tt_result_t result;

    if (lf->f_opened) {
        tt_fclose(&lf->f);
        lf->write_len = 0;
        lf->f_opened = TT_FALSE;
    }

    // construct log file name
    tt_fpath_init(&path, TT_FPATH_AUTO);
    if (!TT_OK(tt_fpath_set(&path, lf->log_path)) ||
        !TT_OK(tt_fpath_to_dir(&path)) ||
        !TT_OK(tt_fpath_set_basename(&path, lf->log_name))) {
        tt_fpath_destroy(&path);
        return TT_FAIL;
    }

    // open log file
    result = TT_FAIL;
    while (!TT_OK(result)) {
        tt_char_t ext[16];
        tt_u64_t size;

        tt_memset(ext, 0, sizeof(ext));
        tt_snprintf(ext, sizeof(ext) - 1, "%d", lf->u.fidx.index);
        if (!TT_OK(tt_fpath_set_extension(&path, ext))) {
            break;
        }

        if (!TT_OK(tt_fopen(&lf->f,
                            tt_fpath_cstr(&path),
                            TT_FO_WRITE | TT_FO_APPEND | TT_FO_CREAT,
                            NULL))) {
            break;
        }

        if (!TT_OK(tt_fseek(&lf->f, TT_FSEEK_END, 0, &size))) {
            tt_fclose(&lf->f);
            break;
        }
        ++lf->u.fidx.index;
        if (size < lf->max_log_size) {
            lf->write_len = (tt_u32_t)size;
            lf->f_opened = TT_TRUE;
            result = TT_SUCCESS;
        } else {
            tt_fclose(&lf->f);
        }
    }

    tt_fpath_destroy(&path);
    return result;
}

// ========================================
// log io file by date
// ========================================

tt_u32_t __lio_fdate_output(IN tt_logio_t *lio,
                            IN const tt_char_t *data,
                            IN tt_u32_t data_len)
{
    tt_logio_file_t *lf = TT_LOGIO_CAST(lio, tt_logio_file_t);
    tt_u32_t write_len = 0;
    tt_thread_t *t = tt_current_thread();

    // this function is already protected by log manger's lock

    t->log_std = TT_TRUE;

    if (!lf->f_opened) {
        // it ever failed to open log file, which is not expected. but here we
        // try to open the log file again
        __fdate_next(lf);
    }

    if (lf->f_opened &&
        TT_OK(tt_fwrite(&lf->f, (tt_u8_t *)data, data_len, &write_len))) {
        lf->write_len += write_len;
        if (lf->write_len >= lf->max_log_size) {
            __fdate_next(lf);
        }
    } else {
        TT_ERROR("log is lost");
    }

    t->log_std = TT_FALSE;
    return write_len;
}

tt_result_t __fdate_next(IN tt_logio_file_t *lf)
{
    tt_fpath_t path;
    tt_result_t result;
    tt_u32_t idx;

    if (lf->f_opened) {
        tt_fclose(&lf->f);
        lf->write_len = 0;
        lf->f_opened = TT_FALSE;
    }

    // construct log file name
    tt_fpath_init(&path, TT_FPATH_AUTO);
    if (!TT_OK(tt_fpath_set(&path, lf->log_path)) ||
        !TT_OK(tt_fpath_to_dir(&path)) ||
        !TT_OK(tt_fpath_set_basename(&path, lf->log_name))) {
        tt_fpath_destroy(&path);
        return TT_FAIL;
    }

    // open log file
    result = TT_FAIL;
    idx = 1;
    while (!TT_OK(result)) {
        tt_char_t ext[32]; // 8+1+6+1+[]
        tt_u32_t n;
        tt_u64_t size;

        tt_memset(ext, 0, sizeof(ext));
        n = tt_date_render_now(lf->u.fdate.date_format, ext, sizeof(ext) - 1);
        TT_ASSERT(n < sizeof(ext));
        tt_snprintf(ext + n, sizeof(ext) - 1 - n, ".%d", idx);
        if (!TT_OK(tt_fpath_set_extension(&path, ext))) {
            break;
        }

        if (!TT_OK(tt_fopen(&lf->f,
                            tt_fpath_cstr(&path),
                            TT_FO_WRITE | TT_FO_APPEND | TT_FO_CREAT,
                            NULL))) {
            break;
        }

        if (!TT_OK(tt_fseek(&lf->f, TT_FSEEK_END, 0, &size))) {
            tt_fclose(&lf->f);
            break;
        }
        ++idx;
        if (size < lf->max_log_size) {
            lf->write_len = (tt_u32_t)size;
            lf->f_opened = TT_TRUE;
            result = TT_SUCCESS;
        } else {
            tt_fclose(&lf->f);
        }
    }

    tt_fpath_destroy(&path);
    return result;
}
