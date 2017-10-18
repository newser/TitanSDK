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

#include <zip/tt_zip_source_file.h>

#include <algorithm/tt_string_common.h>
#include <io/tt_file_system.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <os/tt_thread.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    zip_int64_t start;
    zip_int64_t end;
    zip_int64_t current;
    zip_int64_t supports;
    const tt_char_t *name;
    zip_error_t zerr;
    zip_error_t stat_error;
    struct zip_stat stat;
    tt_file_t f;
    tt_file_t fout;
    tt_string_t tmp_name;
    tt_bool_t f_opened : 1;
    tt_bool_t fout_opened : 1;
} __zsf_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static __zsf_t *__zsf_create(IN const tt_char_t *path,
                             IN tt_u64_t from,
                             IN tt_u64_t len);

static void __zsf_destroy(IN __zsf_t *zsf);

static zip_int64_t __zsf_itf(void *state,
                             void *data,
                             zip_uint64_t len,
                             zip_source_cmd_t cmd);

static zip_int64_t __zsf_begin_write(IN __zsf_t *zsf);

static zip_int64_t __zsf_commit_write(IN __zsf_t *zsf);

static void __zsf_close(IN __zsf_t *zsf);

static zip_int64_t __zsf_open(IN __zsf_t *zsf);

static zip_int64_t __zsf_read(IN __zsf_t *zsf,
                              IN tt_u8_t *data,
                              IN zip_uint64_t len);

static void __zsf_rollback_write(IN __zsf_t *zsf);

static zip_int64_t __zsf_seek(IN __zsf_t *zsf, IN zip_source_args_seek_t *args);

static zip_int64_t __zsf_seek_write(IN __zsf_t *zsf,
                                    IN zip_source_args_seek_t *args);

static zip_int64_t __zsf_write(IN __zsf_t *zsf,
                               IN tt_u8_t *data,
                               IN zip_uint64_t len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_zipsrc_t *tt_zipsrc_file_create(IN const tt_char_t *path,
                                   IN tt_u64_t from,
                                   IN tt_u64_t len)
{
    __zsf_t *zsf;
    zip_source_t *zs;
    zip_error_t zerr;

    TT_ASSERT(path != NULL);

    zsf = __zsf_create(path, from, len);
    if (zsf == NULL) {
        return NULL;
    }

    zs = zip_source_function_create(__zsf_itf, zsf, &zerr);
    if (zs != NULL) {
        return zs;
    } else {
        TT_ERROR("fail to create zipsrc blob: %s", zip_error_strerror(&zerr));
        __zsf_destroy(zsf);
        return NULL;
    }
}

__zsf_t *__zsf_create(IN const tt_char_t *path,
                      IN tt_u64_t from,
                      IN tt_u64_t len)
{
    tt_u32_t n;
    __zsf_t *zsf;
    zip_stat_t *st;
    tt_fstat_t fst;

    n = tt_strlen(path);
    zsf = tt_zalloc(sizeof(__zsf_t) + n + 1);
    if (zsf == NULL) {
        return NULL;
    }

    zsf->start = from;
    zsf->end = len;
    zsf->current = 0;
    zsf->supports = ZIP_SOURCE_SUPPORTS_READABLE |
                    zip_source_make_command_bitmap(ZIP_SOURCE_SUPPORTS,
                                                   ZIP_SOURCE_TELL,
                                                   -1);

    zsf->name = TT_PTR_INC(const tt_char_t, zsf, sizeof(__zsf_t));
    tt_memcpy((tt_char_t *)zsf->name, path, n + 1);

    zip_error_init(&zsf->zerr);
    zip_error_init(&zsf->stat_error);
    zip_stat_init(&zsf->stat);
    tt_string_init(&zsf->tmp_name, NULL);
    zsf->f_opened = TT_FALSE;
    zsf->fout_opened = TT_FALSE;

    // set stat
    st = &zsf->stat;

    if (zsf->end > 0) {
        st->size = zsf->end;
        st->valid |= ZIP_STAT_SIZE;
    }

    if (TT_OK(tt_fstat_path(zsf->name, &fst))) {
        if (!(st->valid & ZIP_STAT_MTIME)) {
            st->mtime = tt_date_diff_epoch_second(&fst.modified);
            st->valid |= ZIP_STAT_MTIME;
        }

        if (fst.is_file) {
            zsf->supports = ZIP_SOURCE_SUPPORTS_SEEKABLE;

            if ((zsf->start + zsf->end) > fst.size) {
                __zsf_destroy(zsf);
                return NULL;
            }

            if (zsf->end == 0) {
                st->size = fst.size - zsf->start;
                st->valid |= ZIP_STAT_SIZE;

                if (zsf->start == 0) {
                    zsf->supports = ZIP_SOURCE_SUPPORTS_WRITABLE;
                }
            }
        }
    } else {
        if ((zsf->start == 0) && (zsf->end == 0)) {
            zsf->supports = ZIP_SOURCE_SUPPORTS_WRITABLE;
        }

        zip_error_set(&zsf->stat_error, ZIP_ER_READ, tt_get_sys_err());
    }

    return zsf;
}

void __zsf_destroy(IN __zsf_t *zsf)
{
    zip_error_fini(&zsf->zerr);
    zip_error_fini(&zsf->stat_error);

    if (zsf->f_opened) {
        tt_fclose(&zsf->f);
    }

    if (zsf->fout_opened) {
        tt_fclose(&zsf->fout);
    }

    tt_string_destroy(&zsf->tmp_name);

    tt_free(zsf);
}

zip_int64_t __zsf_itf(void *state,
                      void *data,
                      zip_uint64_t len,
                      zip_source_cmd_t cmd)
{
    __zsf_t *zsf = (__zsf_t *)state;

    switch (cmd) {
        case ZIP_SOURCE_BEGIN_WRITE: {
            return __zsf_begin_write(zsf);
        }
        case ZIP_SOURCE_COMMIT_WRITE: {
            return __zsf_commit_write(zsf);
        }
        case ZIP_SOURCE_CLOSE: {
            __zsf_close(zsf);
            return 0;
        }
        case ZIP_SOURCE_ERROR: {
            return zip_error_to_data(&zsf->zerr, data, len);
        }
        case ZIP_SOURCE_FREE: {
            __zsf_destroy(zsf);
            return 0;
        }
        case ZIP_SOURCE_OPEN: {
            return __zsf_open(zsf);
        }
        case ZIP_SOURCE_READ: {
            return __zsf_read(zsf, (tt_u8_t *)data, len);
        }
        case ZIP_SOURCE_REMOVE: {
            tt_fremove(zsf->name);
            return 0;
        }
        case ZIP_SOURCE_ROLLBACK_WRITE: {
            __zsf_rollback_write(zsf);
            return 0;
        }
        case ZIP_SOURCE_SEEK: {
            zip_source_args_seek_t *args =
                ZIP_SOURCE_GET_ARGS(zip_source_args_seek_t,
                                    data,
                                    len,
                                    &zsf->zerr);
            if (args != NULL) {
                return __zsf_seek(zsf, args);
            } else {
                return -1;
            }
        }
        case ZIP_SOURCE_SEEK_WRITE: {
            zip_source_args_seek_t *args =
                ZIP_SOURCE_GET_ARGS(zip_source_args_seek_t,
                                    data,
                                    len,
                                    &zsf->zerr);
            if (args != NULL) {
                return __zsf_seek_write(zsf, args);
            } else {
                return -1;
            }
        }
        case ZIP_SOURCE_STAT: {
            if (len < sizeof(zsf->stat))
                return -1;

            if (zip_error_code_zip(&zsf->stat_error) != 0) {
                zip_error_set(&zsf->zerr,
                              zip_error_code_zip(&zsf->stat_error),
                              zip_error_code_system(&zsf->stat_error));
                return -1;
            }

            tt_memcpy(data, &zsf->stat, sizeof(zsf->stat));
            return sizeof(zsf->stat);
        }
        case ZIP_SOURCE_SUPPORTS: {
            return zsf->supports;
        }
        case ZIP_SOURCE_TELL: {
            return (zip_int64_t)zsf->current;
        }
        case ZIP_SOURCE_TELL_WRITE: {
            tt_u64_t location;
            if (TT_OK(tt_ftell(&zsf->fout, &location))) {
                return location;
            } else {
                zip_error_set(&zsf->zerr, ZIP_ER_TELL, tt_get_sys_err());
                return -1;
            }
        }
        case ZIP_SOURCE_WRITE: {
            return __zsf_write(zsf, (tt_u8_t *)data, len);
        }

        default: {
            zip_error_set(&zsf->zerr, ZIP_ER_OPNOTSUPP, 0);
            return -1;
        }
    }
}

zip_int64_t __zsf_begin_write(IN __zsf_t *zsf)
{
    tt_string_t *s = &zsf->tmp_name;

    tt_string_clear(s);
    if (!TT_OK(tt_string_append(s, zsf->name)) ||
        !TT_OK(tt_string_append_f(s, ".%d", tt_rand_u32()))) {
        zip_error_set(&zsf->zerr, ZIP_ER_MEMORY, 0);
        return -1;
    }

    if (zsf->fout_opened) {
        TT_WARN("closed zsf fout");
        tt_fclose(&zsf->fout);
        zsf->fout_opened = TT_FALSE;
    }
    if (!TT_OK(tt_fopen(&zsf->fout,
                        tt_string_cstr(s),
                        TT_FO_CREAT | TT_FO_RDWR,
                        NULL))) {
        zip_error_set(&zsf->zerr, ZIP_ER_TMPOPEN, tt_get_sys_err());
        return -1;
    }
    zsf->fout_opened = TT_TRUE;

    return 0;
}

zip_int64_t __zsf_commit_write(IN __zsf_t *zsf)
{
    TT_ASSERT(zsf->fout_opened);
    tt_fclose(&zsf->fout);
    zsf->fout_opened = TT_FALSE;

    if (!TT_OK(tt_fs_rename(tt_string_cstr(&zsf->tmp_name), zsf->name))) {
        zip_error_set(&zsf->zerr, ZIP_ER_RENAME, 0);
        return -1;
    }
    // need set permission of renamed file??

    return 0;
}

void __zsf_close(IN __zsf_t *zsf)
{
    if (zsf->f_opened) {
        tt_fclose(&zsf->f);
        zsf->f_opened = TT_FALSE;
    }
}

zip_int64_t __zsf_open(IN __zsf_t *zsf)
{
    if (zsf->name != NULL) {
        if (zsf->f_opened) {
            tt_fclose(&zsf->f);
            zsf->f_opened = TT_FALSE;
        }
        if (!TT_OK(tt_fopen(&zsf->f, zsf->name, TT_FO_READ, NULL))) {
            zip_error_set(&zsf->zerr, ZIP_ER_OPEN, tt_get_sys_err());
            return -1;
        }
        zsf->f_opened = TT_TRUE;
    }

    if ((zsf->start > 0) &&
        !TT_OK(tt_fseek(&zsf->f, TT_FSEEK_BEGIN, zsf->start, NULL))) {
        zip_error_set(&zsf->zerr, ZIP_ER_SEEK, tt_get_sys_err());
        return -1;
    }
    zsf->current = 0;

    return 0;
}

zip_int64_t __zsf_read(IN __zsf_t *zsf, IN tt_u8_t *data, IN zip_uint64_t len)
{
    zip_uint64_t n;
    tt_u32_t read_len;

    if (zsf->end > 0) {
        TT_ASSERT(zsf->current <= zsf->end);
        n = zsf->end - zsf->current;
        if (n > len) {
            n = len;
        }
    } else {
        n = len;
    }
    if (n > 0x7FFFFFFF) {
        n = 0x7FFFFFFF;
    }

    if (TT_OK(tt_fread(&zsf->f, data, (tt_u32_t)n, &read_len))) {
        zsf->current += read_len;
        return (zip_int64_t)read_len;
    } else {
        zip_error_set(&zsf->zerr, ZIP_ER_READ, tt_get_sys_err());
        return -1;
    }
}

void __zsf_rollback_write(IN __zsf_t *zsf)
{
    if (zsf->fout_opened) {
        tt_fclose(&zsf->fout);
        zsf->fout_opened = TT_FALSE;
    }

    tt_fremove(tt_string_cstr(&zsf->tmp_name));
}

zip_int64_t __zsf_seek(IN __zsf_t *zsf, IN zip_source_args_seek_t *args)
{
    tt_bool_t to_seek;
    zip_int64_t pos;

    to_seek = TT_TRUE;
    switch (args->whence) {
        case SEEK_SET: {
            pos = args->offset;
        } break;
        case SEEK_END: {
            if (zsf->end == 0) {
                if (!TT_OK(tt_fseek(&zsf->f,
                                    TT_FSEEK_END,
                                    args->offset,
                                    (tt_u64_t *)&pos))) {
                    zip_error_set(&zsf->zerr, ZIP_ER_SEEK, tt_get_sys_err());
                    return -1;
                }
                pos -= zsf->start;
                to_seek = TT_FALSE;
            } else {
                pos = zsf->end + args->offset;
            }
        } break;
        case SEEK_CUR: {
            pos = zsf->current + args->offset;
        } break;

        default: {
            zip_error_set(&zsf->zerr, ZIP_ER_INVAL, 0);
            return -1;
        } break;
    }
    if ((pos < 0) || ((zsf->end != 0) && (pos > zsf->end)) ||
        ((pos + zsf->start) < zsf->start)) {
        zip_error_set(&zsf->zerr, ZIP_ER_INVAL, 0);
        return -1;
    }

    zsf->current = (zip_uint64_t)pos;
    if (to_seek &&
        !TT_OK(tt_fseek(&zsf->f,
                        TT_FSEEK_BEGIN,
                        zsf->current + zsf->start,
                        NULL))) {
        zip_error_set(&zsf->zerr, ZIP_ER_SEEK, tt_get_sys_err());
        return -1;
    }
    return 0;
}

zip_int64_t __zsf_seek_write(IN __zsf_t *zsf, IN zip_source_args_seek_t *args)
{
    tt_u32_t whence;
    switch (args->whence) {
        case SEEK_SET: {
            whence = TT_FSEEK_BEGIN;
        } break;
        case SEEK_END: {
            whence = TT_FSEEK_END;
        } break;
        case SEEK_CUR: {
            whence = TT_FSEEK_CUR;
        } break;

        default: {
            zip_error_set(&zsf->zerr, ZIP_ER_INVAL, 0);
            return -1;
        }
    }

    if (!TT_OK(tt_fseek(&zsf->fout, whence, args->offset, NULL))) {
        zip_error_set(&zsf->zerr, ZIP_ER_SEEK, tt_get_sys_err());
        return -1;
    }

    return 0;
}

zip_int64_t __zsf_write(IN __zsf_t *zsf, IN tt_u8_t *data, IN zip_uint64_t len)
{
    tt_u32_t write_len;

    if (len > 0x7FFFFFFF) {
        len = 0x7FFFFFFF;
    }

    if (TT_OK(tt_fwrite(&zsf->fout, data, len, &write_len))) {
        return (zip_int64_t)write_len;
    } else {
        zip_error_set(&zsf->zerr, ZIP_ER_WRITE, tt_get_sys_err());
        return -1;
    }
}
