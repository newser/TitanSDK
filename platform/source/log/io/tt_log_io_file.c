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
#include <os/tt_fiber_event.h>
#include <os/tt_task.h>
#include <os/tt_thread.h>
#include <time/tt_date_format.h>
#include <time/tt_date_format.h>
#include <time/tt_time_reference.h>
#include <time/tt_timer.h>
#include <zip/tt_zip.h>
#include <zip/tt_zip_source.h>
#include <zip/tt_zip_source_file.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __LIOFW_NAME "logio file worker"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __T_EXIT,
    __T_ARCHIVE,
    __T_PURGE_REMOVE,
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// ========================================
// log io file by index
// ========================================

static void __lio_fidx_destroy(IN tt_logio_t *lio);

static tt_u32_t __lio_fidx_output(IN tt_logio_t *lio,
                                  IN tt_log_entry_t *entry,
                                  IN const tt_char_t *data,
                                  IN tt_u32_t data_len);

static tt_logio_itf_t tt_s_logio_fidx_itf = {
    TT_LOGIO_FILE,

    NULL,
    __lio_fidx_destroy,
    __lio_fidx_output,
};

// ========================================
// log io file by date
// ========================================

static void __lio_fdate_destroy(IN tt_logio_t *lio);

static tt_u32_t __lio_fdate_output(IN tt_logio_t *lio,
                                   IN tt_log_entry_t *entry,
                                   IN const tt_char_t *data,
                                   IN tt_u32_t data_len);

static tt_logio_itf_t tt_s_logio_fdate_itf = {
    TT_LOGIO_FILE,

    NULL,
    __lio_fdate_destroy,
    __lio_fdate_output,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __fidx_next(IN tt_logio_file_t *lf);

static tt_result_t __fdate_next(IN tt_logio_file_t *lf);

static tt_result_t __liof_worker(IN void *param);

static void __liof_w_exit(IN tt_task_t *worker);

static void __liof_w_archive(IN tt_logio_file_t *lf);

static void __liof_w_purge_remove(IN tt_logio_file_t *lf);

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
    lf->log_path = log_path;
    lf->log_name = attr->log_name;
    lf->archive_path = archive_path;
    lf->archive_name = attr->archive_name;
    lf->log_suffix = attr->log_suffix;
    lf->log_purge = attr->log_purge;
    lf->keep_log_sec = attr->keep_log_sec;
    lf->keep_archive_sec = attr->keep_archive_sec;
    if ((attr->max_log_size_order != 30) && (attr->max_log_size_order > 30)) {
        lf->max_log_size = 1 << 30;
    } else {
        lf->max_log_size = 1 << attr->max_log_size_order;
    }
    lf->write_len = 0;
    lf->f_opened = TT_FALSE;
    lf->worker_running = TT_FALSE;

    // init specific
    tt_memset(&lf->u_suffix, 0, sizeof(lf->u_suffix));
    if (attr->log_suffix == TT_LOGFILE_SUFFIX_INDEX) {
        lf->u_suffix.fidx.index = 1;
        result = __fidx_next(lf);
    } else {
        TT_ASSERT(attr->log_suffix == TT_LOGFILE_SUFFIX_DATE);
        lf->u_suffix.fdate.date_format = attr->date_format;
        result = __fdate_next(lf);
    }
    if (!TT_OK(result)) {
        tt_free(lio);
        return NULL;
    }
    // now we can use tt_logio_destroy()

    // create worker if specified keep time
    if ((lf->max_log_size != 0) && (lf->keep_log_sec != 0)) {
        if (!TT_OK(tt_task_create(&lf->worker, NULL)) ||
            !TT_OK(tt_task_add_fiber(&lf->worker,
                                     __LIOFW_NAME,
                                     __liof_worker,
                                     lf,
                                     NULL)) ||
            !TT_OK(tt_task_run(&lf->worker))) {
            __logio_destroy(lio);
            return NULL;
        }
        lf->worker_running = TT_TRUE;
    }

    return lio;
}

void tt_logio_file_attr_default(IN tt_logio_file_attr_t *attr)
{
    attr->log_name = "log";
    attr->archive_name = "archive";
    attr->date_format = "%C%N%DT%H%M%S";
    attr->log_suffix = TT_LOGFILE_SUFFIX_INDEX;
    attr->log_purge = TT_LOGFILE_PURGE_NONE;
    attr->keep_log_sec = 0;
    attr->keep_archive_sec = 0;
    attr->max_log_size_order = 0;
}

// ========================================
// log io file by index
// ========================================

void __lio_fidx_destroy(IN tt_logio_t *lio)
{
    tt_logio_file_t *lf = TT_LOGIO_CAST(lio, tt_logio_file_t);

    if (lf->worker_running) {
        __liof_w_exit(&lf->worker);
    }
}

tt_u32_t __lio_fidx_output(IN tt_logio_t *lio,
                           IN tt_log_entry_t *entry,
                           IN const tt_char_t *data,
                           IN tt_u32_t data_len)
{
    tt_logio_file_t *lf = TT_LOGIO_CAST(lio, tt_logio_file_t);
    tt_u32_t write_len = 0;
    tt_thread_t *t = tt_current_thread();
    tt_thread_log_t l;

    // this function is already protected by log manger's lock

    l = tt_thread_set_log(t, TT_THREAD_LOG_PRINTF);

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

    tt_thread_set_log(t, l);
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
        tt_snprintf(ext, sizeof(ext) - 1, "%d", lf->u_suffix.fidx.index);
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
        ++lf->u_suffix.fidx.index;
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

void __lio_fdate_destroy(IN tt_logio_t *lio)
{
    tt_logio_file_t *lf = TT_LOGIO_CAST(lio, tt_logio_file_t);

    if (lf->worker_running) {
        __liof_w_exit(&lf->worker);
    }
}

tt_u32_t __lio_fdate_output(IN tt_logio_t *lio,
                            IN tt_log_entry_t *entry,
                            IN const tt_char_t *data,
                            IN tt_u32_t data_len)
{
    tt_logio_file_t *lf = TT_LOGIO_CAST(lio, tt_logio_file_t);
    tt_u32_t write_len = 0;
    tt_thread_t *t = tt_current_thread();
    tt_thread_log_t l;

    // this function is already protected by log manger's lock

    l = tt_thread_set_log(t, TT_THREAD_LOG_PRINTF);

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

    tt_thread_set_log(t, l);
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
        n = tt_date_render_now(lf->u_suffix.fdate.date_format,
                               ext,
                               sizeof(ext) - 1);
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

// ========================================
// log io worker
// ========================================

tt_result_t __liof_worker(IN void *param)
{
    tt_logio_file_t *lf;
    tt_fiber_ev_t *fev;
    tt_tmr_t *t;
    tt_fiber_t *cur;

    lf = (tt_logio_file_t *)param;

    t = tt_tmr_create(lf->keep_log_sec * 1000, __T_ARCHIVE, NULL);
    if (!TT_OK(tt_tmr_start(t))) {
        TT_FATAL("fail to start archive timer");
        // conintue as it may create other timers
    }

    if (lf->keep_archive_sec != 0) {
        if (lf->log_purge == TT_LOGFILE_PURGE_REMOVE) {
            t = tt_tmr_create(lf->keep_archive_sec * 1000,
                              __T_PURGE_REMOVE,
                              NULL);
            if (!TT_OK(tt_tmr_start(t))) {
                TT_FATAL("fail to start purge remove timer");
                // conintue as it may create other timers
            }
        }
    }

    cur = tt_current_fiber();
    while (tt_fiber_recv(cur, TT_TRUE, &fev, &t)) {
        if (fev != NULL) {
            switch (fev->ev) {
                case __T_EXIT: {
                    tt_fiber_finish(fev);
                    return TT_SUCCESS;
                } break;

                default: {
                    TT_ERROR("unknown log io file event: %d", fev->ev);
                } break;
            }

            tt_fiber_finish(fev);
        }

        if (t != NULL) {
            switch (t->ev) {
                case __T_ARCHIVE: {
                    __liof_w_archive(lf);
                } break;
                case __T_PURGE_REMOVE: {
                    __liof_w_purge_remove(lf);
                } break;

                default: {
                    TT_ERROR("unknown log io file timer: %d", t->ev);
                } break;
            }

            if (!TT_OK(tt_tmr_start(t))) {
                TT_FATAL("fail to start log io file worker timer: %d", t->ev);
                // continue as other timers may be working
            }
        }
    }

    // should not reach here
    TT_FATAL("exiting log io file worker");
    return TT_FAIL;
}

void __liof_w_exit(IN tt_task_t *worker)
{
    tt_fiber_t *fb = tt_task_find_fiber(worker, __LIOFW_NAME);
    if (fb != NULL) {
        // can not simply exit worker task, as the worker may interacting
        // with fs worker threads, must exit worker in a synchronous way
        tt_fiber_ev_t fev;
        tt_fiber_ev_init(&fev, __T_EXIT);
        tt_fiber_send_ev(fb, &fev, TT_TRUE);

        // the worker has only one fiber, it would exit when the only
        // fiber terminates
        tt_task_wait(worker);
    }
}

void __liof_w_archive(IN tt_logio_file_t *lf)
{
    tt_fpath_t old_p, new_p, tmp_p, tmp_p2;
    tt_date_t min, max;
    tt_zipsrc_t *zs;
    tt_zip_t *z;
    tt_dir_t d;
    tt_dirent_t entry;
    tt_u32_t n, t;
    tt_char_t *p;
    tt_bool_t archived = TT_FALSE, flush = TT_FALSE;
    tt_char_t zname[64] = {0};

    tt_u32_t done = 0;
#define __LA_OLD_P (1 << 0)
#define __LA_NEW_P (1 << 1)
#define __LA_TMP_P (1 << 2)
#define __LA_TMP_P2 (1 << 3)
#define __LA_ZS (1 << 4)
#define __LA_Z (1 << 5)
#define __LA_DIR (1 << 6)

    TT_ASSERT(lf->max_log_size != 0);
    TT_ASSERT(lf->keep_log_sec != 0);

    tt_fpath_init(&old_p, TT_FPATH_AUTO);
    done |= __LA_OLD_P;
    if (!TT_OK(tt_fpath_set(&old_p, lf->archive_path)) ||
        !TT_OK(tt_fpath_to_dir(&old_p)) ||
        !TT_OK(tt_fpath_set_basename(&old_p, lf->archive_name)) ||
        !TT_OK(tt_fpath_set_extension(&old_p, "ttarctmp"))) {
        goto done;
    }

    tt_fpath_init(&new_p, TT_FPATH_AUTO);
    done |= __LA_NEW_P;

    tt_fpath_init(&tmp_p, TT_FPATH_AUTO);
    done |= __LA_TMP_P;
    if (!TT_OK(tt_fpath_set(&tmp_p, lf->log_path)) ||
        !TT_OK(tt_fpath_to_dir(&tmp_p))) {
        goto done;
    }

    tt_fpath_init(&tmp_p2, TT_FPATH_AUTO);
    done |= __LA_TMP_P2;
    if (!TT_OK(tt_fpath_set(&tmp_p2, lf->log_path)) ||
        !TT_OK(tt_fpath_to_dir(&tmp_p2))) {
        goto done;
    }

    tt_date_set(&min, 2199, TT_DECEMBER, 31, 11, 59, 59);
    tt_date_set(&max, 1900, TT_JANUARY, 1, 0, 0, 0);

    // first use old_p as zip file name, as we don't know date range yet
    TT_DONN_G(done, zs = tt_zipsrc_file_create(tt_fpath_cstr(&old_p), 0, 0));
    done |= __LA_ZS;

    TT_DONN_G(done, z = tt_zip_create(zs, TT_ZA_CREAT | TT_ZA_EXCL, 0));
    // zs is now manged by z, no need to care z
    done &= ~__LA_ZS;
    done |= __LA_Z;

    TT_DO_G(done, tt_dopen(&d, lf->log_path, NULL));
    done |= __LA_DIR;
    while (TT_OK(tt_dread(&d, &entry))) {
        tt_fstat_t fst;
        tt_zipsrc_t *__zs;

        if (tt_strcmp(entry.name, ".") == 0) {
            continue;
        }
        if (tt_strcmp(entry.name, "..") == 0) {
            continue;
        }

        // - must be a file
        // - size should already exceed max_log_size
        // - time of last modification should be before keep_log_sec seconds
        if (!TT_OK(tt_fpath_set_basename(&tmp_p, entry.name)) ||
            !TT_OK(tt_fstat_path(tt_fpath_cstr(&tmp_p), &fst)) ||
            !fst.is_file || (fst.size < lf->max_log_size) ||
            (-tt_date_diff_now_second(&fst.modified) <
             (tt_s64_t)lf->keep_log_sec)) {
            continue;
        }

        n = (tt_u32_t)tt_strlen(entry.name);
        if ((n >= 9) && (tt_strcmp(entry.name + n - 9, ".ttarctmp") == 0)) {
            // this is a left file as last archiving failed
            p = entry.name;
            n -= 9;
        } else {
            // - note the trick: set basename to the whole file name, and
            //   .ttarctmp as extension
            // - rename the file to .ttarctmp as a mark, all .ttarctmp file
            //   would be removed when archiving is done.
            // - if renaming failed, the log file is left there and may be
            //   archived to next zip file
            if (!TT_OK(tt_fpath_set_basename(&tmp_p2, entry.name)) ||
                !TT_OK(tt_fpath_set_extension(&tmp_p2, "ttarctmp")) ||
                !TT_OK(tt_fs_rename(tt_fpath_cstr(&tmp_p),
                                    tt_fpath_cstr(&tmp_p2)))) {
                continue;
            }
            p = (tt_char_t *)tt_fpath_cstr(&tmp_p2);
        }
        if ((__zs = tt_zipsrc_file_create(p, 0, 0)) == NULL) {
            continue;
        }

        // - tt_zipsrc_file_create() would save the path name, p can be
        //   modified now
        // - use original log file name, do not include .ttarctmp suffix
        entry.name[n] = 0;
        if (tt_zip_add_file(z, entry.name, __zs, 0) == TT_POS_NULL) {
            tt_zipsrc_release(__zs);
        }
        archived = TT_TRUE;

        if (tt_date_cmp(&fst.created, &min) < 0) {
            tt_date_copy(&min, &fst.created);
        }
        if (tt_date_cmp(&fst.modified, &max) > 0) {
            tt_date_copy(&max, &fst.created);
        }
    }
    if (!archived) {
        goto done;
    }

    p = zname;
    n = sizeof(zname) - 1;
    t = tt_date_render(&min, "%C%N%DT%H%M%S", p, n);
    tt_date_render(&max, "-%C%N%DT%H%M%S.zip", p + t, n - t);
    if (!TT_OK(tt_fpath_set(&new_p, lf->archive_path)) ||
        !TT_OK(tt_fpath_to_dir(&new_p)) ||
        !TT_OK(tt_fpath_set_basename(&new_p, lf->archive_name)) ||
        !TT_OK(tt_fpath_set_extension(&new_p, zname))) {
        // renamed log files would be archived next time
        goto done;
    }

    // all done, old_p and new_p and tmp_p would be used to remove
    // arhived log files
    done &= ~(__LA_OLD_P | __LA_NEW_P | __LA_TMP_P);
    flush = TT_TRUE;
done:
    if (done & __LA_OLD_P) {
        tt_fpath_destroy(&old_p);
    }

    if (done & __LA_NEW_P) {
        tt_fpath_destroy(&new_p);
    }

    if (done & __LA_TMP_P) {
        tt_fpath_destroy(&tmp_p);
    }

    if (done & __LA_TMP_P2) {
        tt_fpath_destroy(&tmp_p2);
    }

    if (done & __LA_ZS) {
        tt_zipsrc_release(zs);
    }

    if (done & __LA_Z) {
        tt_zip_destroy(z, flush);
    }

    if (done & __LA_DIR) {
        tt_dclose(&d);
    }

    if (flush) {
        TT_ASSERT(!(done & (__LA_OLD_P | __LA_NEW_P | __LA_TMP_P)));
        tt_fs_rename(tt_fpath_cstr(&old_p), tt_fpath_cstr(&new_p));
        tt_fpath_destroy(&old_p);
        tt_fpath_destroy(&new_p);

        if (TT_OK(tt_dopen(&d, lf->log_path, NULL))) {
            while (TT_OK(tt_dread(&d, &entry))) {
                tt_u32_t n;

                n = (tt_u32_t)tt_strlen(entry.name);
                if ((n < 9) ||
                    (tt_strcmp(entry.name + n - 9, ".ttarctmp") != 0)) {
                    continue;
                }

                if (!TT_OK(tt_fpath_set_filename(&tmp_p, entry.name)) ||
                    !TT_OK(tt_fremove(tt_fpath_cstr(&tmp_p)))) {
                    entry.name[n - 9] = 0;
                    TT_ERROR("fail to remove %s, would be redundant log files",
                             entry.name);
                }
            }
            tt_dclose(&d);
        }
        tt_fpath_destroy(&tmp_p);
    }
}

void __liof_w_purge_remove(IN tt_logio_file_t *lf)
{
    tt_fpath_t ap;
    tt_u32_t len;
    tt_dir_t d;
    tt_dirent_t entry;

    tt_u32_t __done = 0;
#define __LPR_AP (1 << 0)
#define __LPR_DIR (1 << 1)

    tt_fpath_init(&ap, TT_FPATH_AUTO);
    __done |= __LPR_AP;
    TT_DO_G(done, tt_fpath_set(&ap, lf->archive_path));

    len = (tt_u32_t)tt_strlen(lf->archive_name);

    TT_DO_G(done, tt_dopen(&d, lf->archive_path, NULL));
    __done |= __LPR_DIR;
    while (TT_OK(tt_dread(&d, &entry))) {
        tt_fstat_t fst;

        if (tt_strncmp(entry.name, lf->archive_name, len) != 0) {
            continue;
        }

        if (!TT_OK(tt_fpath_set_filename(&ap, entry.name)) ||
            !TT_OK(tt_fstat_path(tt_fpath_cstr(&ap), &fst)) || !fst.is_file ||
            (-tt_date_diff_now_second(&fst.modified) <
             (tt_s64_t)lf->keep_archive_sec)) {
            continue;
        }

        tt_fremove(tt_fpath_cstr(&ap));
    }

done:
    if (__done & __LPR_AP) {
        tt_fpath_destroy(&ap);
    }

    if (__done & __LPR_DIR) {
        tt_dclose(&d);
    }
}
