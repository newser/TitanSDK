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

#include <log/layout/tt_log_field.h>

#include <algorithm/tt_buffer_format.h>
#include <memory/tt_memory_alloc.h>
#include <time/tt_time_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_logfld_t *(*__lf_create_t)(IN tt_logfld_type_t type,
                                      IN const tt_char_t *fmt_start,
                                      IN const tt_char_t *fmt_end);

typedef void (*__lf_destroy_t)(IN tt_logfld_t *lf);

typedef tt_result_t (*__lf_check_t)(IN const tt_char_t *fmt_start,
                                    IN const tt_char_t *fmt_end);

typedef tt_result_t (*__lf_output_t)(IN tt_logfld_t *lf,
                                     IN tt_log_entry_t *entry,
                                     OUT tt_buf_t *outbuf);

typedef struct
{
    const tt_char_t *name;
    tt_logfld_type_t type;
    const tt_char_t *default_format;

    __lf_create_t create;
    __lf_destroy_t destroy;
    __lf_check_t check;
    __lf_output_t output;
} __lf_entry_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_logfld_t *__lf_create(IN tt_logfld_type_t type,
                                IN const tt_char_t *fmt_start,
                                IN const tt_char_t *fmt_end);

static void __lf_destroy(IN tt_logfld_t *lf);

static tt_result_t __lf_check(IN const tt_char_t *start,
                              IN const tt_char_t *end);

static tt_result_t __lf_output_seq_num(IN tt_logfld_t *lf,
                                       IN tt_log_entry_t *entry,
                                       OUT tt_buf_t *outbuf);

static tt_result_t __lf_output_time(IN tt_logfld_t *lf,
                                    IN tt_log_entry_t *entry,
                                    OUT tt_buf_t *outbuf);

static tt_result_t __lf_output_logger(IN tt_logfld_t *lf,
                                      IN tt_log_entry_t *entry,
                                      OUT tt_buf_t *outbuf);

static tt_result_t __lf_output_level(IN tt_logfld_t *lf,
                                     IN tt_log_entry_t *entry,
                                     OUT tt_buf_t *outbuf);

static tt_result_t __lf_output_content(IN tt_logfld_t *lf,
                                       IN tt_log_entry_t *entry,
                                       OUT tt_buf_t *outbuf);

static tt_result_t __lf_output_func(IN tt_logfld_t *lf,
                                    IN tt_log_entry_t *entry,
                                    OUT tt_buf_t *outbuf);

static tt_result_t __lf_output_line(IN tt_logfld_t *lf,
                                    IN tt_log_entry_t *entry,
                                    OUT tt_buf_t *outbuf);

static __lf_entry_t __lf_table[] = {
    {
        TT_LOGFLD_SEQ_NUM_KEY,
        TT_LOGFLD_SEQ_NUM,
        "%d",

        __lf_create,
        __lf_destroy,
        __lf_check,
        __lf_output_seq_num,
    },

    {
        TT_LOGFLD_TIME_KEY,
        TT_LOGFLD_TIME,
        "%Y-%m-%d %H:%M:%S",

        __lf_create,
        __lf_destroy,
        __lf_check,
        __lf_output_time,
    },

    {
        TT_LOGFLD_LOGGER_KEY,
        TT_LOGFLD_LOGGER,
        "%s",

        __lf_create,
        __lf_destroy,
        __lf_check,
        __lf_output_logger,
    },

    {
        TT_LOGFLD_LEVEL_KEY,
        TT_LOGFLD_LEVEL,
        "%s",

        __lf_create,
        __lf_destroy,
        __lf_check,
        __lf_output_level,
    },

    {
        TT_LOGFLD_CONTENT_KEY,
        TT_LOGFLD_CONTENT,
        "%s",

        __lf_create,
        __lf_destroy,
        __lf_check,
        __lf_output_content,
    },

    {
        TT_LOGFLD_FUNC_KEY,
        TT_LOGFLD_FUNC,
        "%s",

        __lf_create,
        __lf_destroy,
        __lf_check,
        __lf_output_func,
    },

    {
        TT_LOGFLD_LINE_KEY,
        TT_LOGFLD_LINE,
        "%d",

        __lf_create,
        __lf_destroy,
        __lf_check,
        __lf_output_line,
    },
};

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_logfld_t *tt_logfld_create(IN const tt_char_t *start,
                              IN const tt_char_t *end)
{
    tt_u32_t flen;
    tt_u32_t i;

    if (start >= (end - 1)) {
        return NULL;
    }
    flen = (tt_u32_t)(tt_ptrdiff_t)(end - start);

    // discard {}
    start += 1;
    flen -= 1;

    for (i = 0; i < sizeof(__lf_table) / sizeof(__lf_table[0]); ++i) {
        __lf_entry_t *lfe = &__lf_table[i];
        tt_u32_t name_len = (tt_u32_t)tt_strlen(lfe->name);

        if ((name_len <= flen) &&
            (tt_strncmp(start, lfe->name, name_len) == 0) &&
            TT_OK(lfe->check(start + name_len, end))) {
            return lfe->create(lfe->type, start + name_len, end);
        }
    }
    return NULL;
}

void tt_logfld_destroy(IN tt_logfld_t *lf)
{
    __lf_entry_t *lfe;

    lfe = &__lf_table[lf->type];
    lfe->destroy(lf);
}

tt_result_t tt_logfld_check(IN const tt_char_t *start, IN const tt_char_t *end)
{
    tt_u32_t flen;
    tt_u32_t i;

    if (start >= (end - 1)) {
        return TT_FAIL;
    }
    flen = (tt_u32_t)(tt_ptrdiff_t)(end - start);

    // discard {}
    start += 1;
    flen -= 1;

    for (i = 0; i < sizeof(__lf_table) / sizeof(__lf_table[0]); ++i) {
        __lf_entry_t *lfe = &__lf_table[i];
        tt_u32_t name_len = (tt_u32_t)tt_strlen(lfe->name);

        if ((name_len <= flen) &&
            (tt_strncmp(start, lfe->name, name_len) == 0)) {
            return lfe->check(start + name_len, end);
        }
    }
    return TT_FAIL;
}

tt_result_t tt_logfld_output(IN tt_logfld_t *lf,
                             IN tt_log_entry_t *entry,
                             OUT struct tt_buf_s *outbuf)
{
    __lf_entry_t *lfe = &__lf_table[lf->type];

    return TT_COND(lfe->output != NULL,
                   lfe->output(lf, entry, outbuf),
                   TT_FAIL);
}

tt_logfld_t *__lf_create(IN tt_logfld_type_t type,
                         IN const tt_char_t *fmt_start,
                         IN const tt_char_t *fmt_end)
{
    tt_u32_t fmt_len;
    tt_logfld_t *lf;

    if (fmt_start + 1 < fmt_end) {
        // ignore ":"
        ++fmt_start;
        fmt_len = (tt_u32_t)(tt_ptrdiff_t)(fmt_end - fmt_start);
    } else {
        fmt_start = __lf_table[type].default_format;
        fmt_len = (tt_u32_t)tt_strlen(__lf_table[type].default_format);
    }

    lf = (tt_logfld_t *)tt_malloc(sizeof(tt_logfld_t) + fmt_len + 1);
    if (lf != NULL) {
        tt_lnode_init(&lf->node);
        lf->type = type;

        lf->format = TT_PTR_INC(tt_char_t, lf, sizeof(tt_logfld_t));
        tt_memcpy(lf->format, fmt_start, fmt_len);
        lf->format[fmt_len] = 0;

        return lf;
    } else {
        return NULL;
    }
}

void __lf_destroy(IN tt_logfld_t *lf)
{
    tt_free(lf);
}

tt_result_t __lf_check(IN const tt_char_t *start, IN const tt_char_t *end)
{
    // valid field format:
    //  - ""
    //  - ":"
    //  - ":any string"

    if ((start > end) || (*end != '}') || ((start < end) && (*start != ':'))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __lf_output_seq_num(IN tt_logfld_t *lf,
                                IN tt_log_entry_t *entry,
                                OUT tt_buf_t *outbuf)
{
    return tt_buf_putf(outbuf, lf->format, entry->seq_num);
}

tt_result_t __lf_output_time(IN tt_logfld_t *lf,
                             IN tt_log_entry_t *entry,
                             OUT tt_buf_t *outbuf)
{
    tt_u8_t *p;
    tt_u32_t len;

    // todo: read time from entry and format to outbuf

    // reserving 100 bytes should be enough
    TT_DO(tt_buf_reserve(outbuf, 100));
    p = TT_BUF_WPOS(outbuf);
    len = TT_BUF_WLEN(outbuf);

    len = tt_time_localfmt((tt_char_t *)p, len, lf->format);
    tt_buf_inc_wp(outbuf, len);

    return TT_SUCCESS;
}

tt_result_t __lf_output_logger(IN tt_logfld_t *lf,
                               IN tt_log_entry_t *entry,
                               OUT tt_buf_t *outbuf)
{
    if (entry->logger != NULL) {
        return tt_buf_putf(outbuf, lf->format, entry->logger);
    } else {
        return TT_SUCCESS;
    }
}

tt_result_t __lf_output_level(IN tt_logfld_t *lf,
                              IN tt_log_entry_t *entry,
                              OUT tt_buf_t *outbuf)
{
    return tt_buf_putf(outbuf, lf->format, tt_g_log_level_name[entry->level]);
}

tt_result_t __lf_output_content(IN tt_logfld_t *lf,
                                IN tt_log_entry_t *entry,
                                OUT tt_buf_t *outbuf)
{
    if (entry->content != NULL) {
        return tt_buf_putf(outbuf, lf->format, entry->content);
    } else {
        return TT_SUCCESS;
    }
}

tt_result_t __lf_output_func(IN tt_logfld_t *lf,
                             IN tt_log_entry_t *entry,
                             OUT tt_buf_t *outbuf)
{
    if (entry->function != NULL) {
        return tt_buf_putf(outbuf, lf->format, entry->function);
    } else {
        return TT_SUCCESS;
    }
}

tt_result_t __lf_output_line(IN tt_logfld_t *lf,
                             IN tt_log_entry_t *entry,
                             OUT tt_buf_t *outbuf)
{
    return tt_buf_putf(outbuf, lf->format, entry->line);
}
