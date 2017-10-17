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
    tt_s64_t start;
    tt_s64_t end;
    tt_s64_t current;
    zip_int64_t supports;
    struct zip_stat st;
    const tt_char_t *path;
    const tt_char_t *tmp_path;
    tt_file_t fin;
    tt_file_t fout;
    zip_error_t ze;
    zip_error_t stat_error;
    tt_bool_t fin_opened : 1;
    tt_bool_t fout_opened : 1;
} __zsf_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static zip_int64_t __zsf_itf(void *state,
                             void *data,
                             zip_uint64_t len,
                             zip_source_cmd_t cmd);

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static __zsf_t *__zsf_create(IN const tt_char_t *path);

static void __zsf_destroy(IN __zsf_t *zsf);

static void __zsf_reset(IN __zsf_t *zsf);

static zip_int64_t __zsf_begin_write(IN __zsf_t *zsf);

static zip_int64_t __zsf_commit_write(IN __zsf_t *zsf);

static void __zsf_close(IN __zsf_t *zsf);

static zip_int64_t __zsf_open(IN __zsf_t *zsf);

static zip_int64_t __zsf_read(IN __zsf_t *zsf,
                              IN tt_u8_t *data,
                              IN zip_uint64_t len);

static zip_int64_t __zsf_remove(IN __zsf_t *zsf);

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

tt_zipsrc_t *tt_zipsrc_file_create(IN const tt_char_t *path)
{
    __zsf_t *zsf;
    zip_source_t *zs;
    zip_error_t ze;

    TT_ASSERT(path != NULL);

    zsf = __zsf_create(path);
    if (zsf == NULL) {
        return NULL;
    }

    zs = zip_source_function_create(__zsf_itf, zsf, &ze);
    if (zs != NULL) {
        return zs;
    } else {
        TT_ERROR("fail to create zipsrc blob: %s", zip_error_strerror(&ze));
        __zsf_destroy(zsf);
        return NULL;
    }
}

zip_int64_t __zsf_itf(void *state,
                      void *data,
                      zip_uint64_t len,
                      zip_source_cmd_t cmd)
{
    __zsf_t *zsf;
    char *buf;
    zip_uint64_t n;
    size_t i;

    zsf = (__zsf_t *)state;
    buf = (char *)data;

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
            return zip_error_to_data(&zsf->ze, data, len);
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
            return __zsf_remove(zsf);
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
                                    &zsf->ze);
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
                                    &zsf->ze);
            if (args != NULL) {
                return __zsf_seek_write(zsf, args);
            } else {
                return -1;
            }
        }
        case ZIP_SOURCE_STAT: {
            if (len < sizeof(zsf->st))
                return -1;

            if (zip_error_code_zip(&zsf->stat_error) != 0) {
                zip_error_set(&zsf->ze,
                              zip_error_code_zip(&zsf->stat_error),
                              zip_error_code_system(&zsf->stat_error));
                return -1;
            }

            memcpy(data, &zsf->st, sizeof(zsf->st));
            return sizeof(zsf->st);
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
                zip_error_set(&zsf->ze, ZIP_ER_TELL, tt_last_error());
                return -1;
            }
        }
        case ZIP_SOURCE_WRITE: {
            return __zsf_write(zsf, (tt_u8_t *)data, len);
        }

        default: {
            zip_error_set(&zsf->ze, ZIP_ER_OPNOTSUPP, 0);
            return -1;
        }
    }
}

zip_int64_t __zsf_begin_write(IN __zsf_t *zsf)
{
    tt_u32_t n;
    tt_char_t *tp;

    __zsf_reset(zsf);

    n = tt_strlen(zsf->path) + 10;
    tp = tt_zalloc(n);
    if (tp == NULL) {
        zip_error_set(&zsf->ze, ZIP_ER_MEMORY, 0);
        return -1;
    }
    tt_snprintf(tp, n - 1, "%s.%d", zsf->path, tt_rand_u32() & 0xFFFF);
    TT_ASSERT(zsf->tmp_path == NULL);
    zsf->tmp_path = tp;

    TT_ASSERT(!zsf->fout_opened);
    if (!TT_OK(tt_fopen(&zsf->fout,
                        zsf->tmp_path,
                        TT_FO_CREAT | TT_FO_RDWR,
                        NULL))) {
        zip_error_set(&zsf->ze, ZIP_ER_TMPOPEN, 0);
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

    if (!TT_OK(tt_fs_rename(zsf->tmp_path, zsf->path))) {
        zip_error_set(&zsf->ze, ZIP_ER_RENAME, 0);
        return -1;
    }
    // need set permission of renamed file??
    tt_free((void *)zsf->tmp_path);
    zsf->tmp_path = NULL;

    return 0;
}

void __zsf_close(IN __zsf_t *zsf)
{
    if (zsf->fin_opened) {
        tt_fclose(&zsf->fin);
        zsf->fin_opened = TT_FALSE;
    }
}

zip_int64_t __zsf_open(IN __zsf_t *zsf)
{
    if (zsf->path != NULL) {
        TT_ASSERT(!zsf->fin_opened);
        if (!TT_OK(tt_fopen(&zsf->fin, zsf->path, TT_FO_READ, NULL))) {
            zip_error_set(&zsf->ze, ZIP_ER_OPEN, tt_last_error());
            return -1;
        }
        zsf->fin_opened = TT_TRUE;
    }

    if ((zsf->start > 0) &&
        !TT_OK(tt_fseek(&zsf->fin, TT_FSEEK_BEGIN, zsf->start, NULL))) {
        zip_error_set(&zsf->ze, ZIP_ER_SEEK, tt_last_error());
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

    if (!TT_OK(tt_fread(&zsf->fin, data, (tt_u32_t)n, &read_len))) {
        zsf->current += read_len;
        return (zip_int64_t)read_len;
    } else {
        zip_error_set(&zsf->ze, ZIP_ER_READ, tt_last_error());
        return -1;
    }
}

zip_int64_t __zsf_remove(IN __zsf_t *zsf)
{
    if (TT_OK(tt_fremove(zsf->path))) {
        return 0;
    } else {
        zip_error_set(&zsf->ze, ZIP_ER_REMOVE, tt_last_error());
        return -1;
    }
}

void __zsf_rollback_write(IN __zsf_t *zsf)
{
    if (zsf->fout_opened) {
        tt_fclose(&zsf->fout);
        zsf->fout_opened = TT_FALSE;
    }

    if (zsf->tmp_path != NULL) {
        tt_fremove(zsf->tmp_path);
        tt_free((void *)zsf->tmp_path);
        zsf->tmp_path = NULL;
    }
}

zip_int64_t __zsf_seek(IN __zsf_t *zsf, IN zip_source_args_seek_t *args)
{
    int need_seek = 1;
    zip_int64_t new_current;

    switch (args->whence) {
        case SEEK_SET: {
            new_current = args->offset;
        } break;
        case SEEK_END: {
            if (zsf->end == 0) {
                if (!TT_OK(tt_fseek(&zsf->fin,
                                    TT_FSEEK_END,
                                    args->offset,
                                    (tt_u64_t *)&new_current))) {
                    zip_error_set(&zsf->ze, ZIP_ER_SEEK, tt_last_error());
                    return -1;
                }
                new_current -= (zip_int64_t)zsf->start;
                need_seek = 0;
            } else {
                new_current = (zip_int64_t)zsf->end + args->offset;
            }
        } break;
        case SEEK_CUR: {
            new_current = (zip_int64_t)zsf->current + args->offset;
        } break;

        default: {
            zip_error_set(&zsf->ze, ZIP_ER_INVAL, 0);
            return -1;
        }
    }

    if ((new_current < 0) ||
        ((zsf->end != 0) && ((zip_uint64_t)new_current > zsf->end)) ||
        (((zip_uint64_t)new_current + zsf->start) < zsf->start)) {
        zip_error_set(&zsf->ze, ZIP_ER_INVAL, 0);
        return -1;
    }

    zsf->current = (zip_uint64_t)new_current;

    if (need_seek) {
        if (!TT_OK(tt_fseek(&zsf->fin,
                            TT_FSEEK_BEGIN,
                            zsf->current + zsf->start,
                            NULL))) {
            zip_error_set(&zsf->ze, ZIP_ER_SEEK, tt_last_error());
            return -1;
        }
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
            zip_error_set(&zsf->ze, ZIP_ER_INVAL, 0);
            return -1;
        }
    }

    if (!TT_OK(tt_fseek(&zsf->fout, whence, args->offset, NULL))) {
        zip_error_set(&zsf->ze, ZIP_ER_SEEK, tt_last_error());
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
        zip_error_set(&zsf->ze, ZIP_ER_WRITE, tt_last_error());
        return -1;
    }
}

__zsf_t *__zsf_create(IN const tt_char_t *path)
{
    tt_u32_t n;
    __zsf_t *zsf;

    n = tt_strlen(path);
    zsf = tt_malloc(sizeof(__zsf_t) + n + 1);
    if (zsf == NULL) {
        return NULL;
    }

    zsf->path = TT_PTR_INC(const tt_char_t, zsf, sizeof(__zsf_t));
    tt_memcpy((tt_char_t *)zsf->path, path, n + 1);

    return zsf;
}

void __zsf_destroy(IN __zsf_t *zsf)
{
    tt_free(zsf);
}

void __zsf_reset(IN __zsf_t *zsf)
{
}
