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

#include <log/tt_log_field.h>

#include <log/tt_log_field_time.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

// lfe: name2type entry
typedef tt_logfld_t *(*__logfld_create_t)(IN struct tt_logfmt_s *lfmt,
                                          IN tt_logfld_type_t type,
                                          IN const tt_char_t *fmt_start,
                                          IN const tt_char_t *fmt_end);
typedef void (*__logfld_destroy_t)(IN tt_logfld_t *lfld);

typedef tt_result_t (*__logfld_validate_t)(IN struct tt_logfmt_s *lfmt,
                                           IN const tt_char_t *fmt_start,
                                           IN const tt_char_t *fmt_end);

typedef tt_u32_t (*__logfld_output_t)(IN tt_logfld_t *lfld,
                                      IN OUT tt_char_t *pos,
                                      IN OUT tt_u32_t left_len,
                                      IN struct tt_logfmt_s *lfmt);

typedef struct
{
    const tt_char_t *name;
    tt_logfld_type_t type;
    const tt_char_t *default_format;

    __logfld_create_t create;
    __logfld_destroy_t destroy;
    __logfld_validate_t validate;
    __logfld_output_t output;
} __logfld_entry_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static const tt_char_t *__s_level_desc[] = {
    "Detail", "Info", "Warn", "Error", "Fatal",
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// common
static tt_logfld_t *__logfld_create(IN struct tt_logfmt_s *lfmt,
                                    IN tt_logfld_type_t type,
                                    IN const tt_char_t *fmt_start,
                                    IN const tt_char_t *fmt_end);
static void __logfld_destroy(IN tt_logfld_t *lfld);
static tt_result_t __logfld_validate(IN struct tt_logfmt_s *lfmt,
                                     IN const tt_char_t *start,
                                     IN const tt_char_t *end);

// seq_no
static tt_u32_t __logfld_seq_no_output(IN tt_logfld_t *lfld,
                                       IN OUT tt_char_t *pos,
                                       IN OUT tt_u32_t left_len,
                                       IN struct tt_logfmt_s *lfmt);

// logger
static tt_u32_t __logfld_logger_output(IN tt_logfld_t *lfld,
                                       IN OUT tt_char_t *pos,
                                       IN OUT tt_u32_t left_len,
                                       IN struct tt_logfmt_s *lfmt);

// level
static tt_u32_t __logfld_level_output(IN tt_logfld_t *lfld,
                                      IN OUT tt_char_t *pos,
                                      IN OUT tt_u32_t left_len,
                                      IN struct tt_logfmt_s *lfmt);

static __logfld_entry_t __logfld_table[] = {
    {
        TT_LOGFLD_SEQ_NO_KEY,
        TT_LOGFLD_SEQ_NO,
        "%d",
        __logfld_create,
        __logfld_destroy,
        __logfld_validate,
        __logfld_seq_no_output,
    },

    {
        TT_LOGFLD_TIME_KEY,
        TT_LOGFLD_TIME,
        "%Y-%m-%d %H:%M:%S",
        __logfld_create,
        __logfld_destroy,
        __logfld_validate,
        tt_logfld_time_output,
    },

    {
        TT_LOGFLD_LOGGER_KEY,
        TT_LOGFLD_LOGGER,
        "%s",
        __logfld_create,
        __logfld_destroy,
        __logfld_validate,
        __logfld_logger_output,
    },

    {
        TT_LOGFLD_LEVEL_KEY,
        TT_LOGFLD_LEVEL,
        "%s",
        __logfld_create,
        __logfld_destroy,
        __logfld_validate,
        __logfld_level_output,
    },

    {
        TT_LOGFLD_CONTENT_KEY,
        TT_LOGFLD_CONTENT,
        "%s",
        __logfld_create,
        __logfld_destroy,
        __logfld_validate,
        NULL,
    },

    {
        TT_LOGFLD_FUNC_KEY,
        TT_LOGFLD_FUNC,
        "%s",
        __logfld_create,
        __logfld_destroy,
        __logfld_validate,
        NULL,
    },

    {
        TT_LOGFLD_LINE_KEY,
        TT_LOGFLD_LINE,
        "%d",
        __logfld_create,
        __logfld_destroy,
        __logfld_validate,
        NULL,
    },
};

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_logfld_t *tt_logfld_create(IN struct tt_logfmt_s *lfmt,
                              IN const tt_char_t *start,
                              IN const tt_char_t *end)
{
    tt_u32_t flen;
    tt_u32_t i;

    if ((end - 1) <= start) {
        return NULL;
    }
    flen = (tt_u32_t)(tt_ptrdiff_t)(end - start);

    // ignore {}
    start += 1;
    flen -= 1;

    for (i = 0; i < sizeof(__logfld_table) / sizeof(__logfld_table[0]); ++i) {
        __logfld_entry_t *lfe = &__logfld_table[i];
        tt_u32_t name_len = (tt_u32_t)tt_strlen(lfe->name);

        if (tt_strncmp(start, lfe->name, name_len) == 0) {
            return lfe->create(lfmt, lfe->type, start + name_len, end);
        }
    }
    return NULL;
}

void tt_logfld_destroy(IN tt_logfld_t *lfld)
{
    __logfld_entry_t *lfe;

    if (!TT_LOGFLD_TYPE_VALID(lfld->type)) {
        return;
    }

    lfe = &__logfld_table[lfld->type];
    lfe->destroy(lfld);
}

tt_result_t tt_logfld_validate(IN struct tt_logfmt_s *lfmt,
                               IN const tt_char_t *start,
                               IN const tt_char_t *end)
{
    tt_u32_t flen;
    tt_u32_t i;

    if ((end - 1) <= start) {
        return TT_BAD_PARAM;
    }
    flen = (tt_u32_t)(tt_ptrdiff_t)(end - start);

    // ignore {}
    start += 1;
    flen -= 1;

    for (i = 0; i < sizeof(__logfld_table) / sizeof(__logfld_table[0]); ++i) {
        __logfld_entry_t *lfe = &__logfld_table[i];
        tt_u32_t name_len = (tt_u32_t)tt_strlen(lfe->name);

        if (tt_strncmp(start, lfe->name, name_len) == 0) {
            return lfe->validate(lfmt, start + name_len, end);
        }
    }
    return TT_NOT_EXIST;
}

tt_u32_t tt_logfld_output(IN tt_logfld_t *lfld,
                          IN OUT tt_char_t *pos,
                          IN OUT tt_u32_t left_len,
                          IN struct tt_logfmt_s *lfmt)
{
    __logfld_entry_t *lfe;

    lfe = &__logfld_table[lfld->type];
    if (lfe->output != NULL) {
        return lfe->output(lfld, pos, left_len, lfmt);
    } else {
        return 0;
    }
}

tt_logfld_t *__logfld_create(IN struct tt_logfmt_s *lfmt,
                             IN tt_logfld_type_t type,
                             IN const tt_char_t *fmt_start,
                             IN const tt_char_t *fmt_end)
{
    tt_logfld_t *lfld;
    tt_u32_t size = sizeof(tt_logfld_t);
    tt_u32_t fmt_size = 0;

    if (fmt_start + 1 < fmt_end) {
        // ignore ":"
        ++fmt_start;
        fmt_size = (tt_u32_t)(tt_ptrdiff_t)(fmt_end - fmt_start);
    } else {
        fmt_start = __logfld_table[type].default_format;
        fmt_size = (tt_u32_t)tt_strlen(__logfld_table[type].default_format);
    }
    size += fmt_size + 1;

    lfld = (tt_logfld_t *)tt_malloc(size);
    if (lfld != NULL) {
        tt_memset(lfld, 0, size);

        tt_lnode_init(&lfld->node);
        lfld->type = type;

        lfld->format = TT_PTR_INC(const tt_char_t, lfld, sizeof(tt_logfld_t));
        tt_memcpy((tt_u8_t *)lfld->format, fmt_start, fmt_size);

        return lfld;
    } else {
        return NULL;
    }
}

void __logfld_destroy(IN tt_logfld_t *lfld)
{
    tt_free(lfld);
}

tt_result_t __logfld_validate(IN struct tt_logfmt_s *lfmt,
                              IN const tt_char_t *start,
                              IN const tt_char_t *end)
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

// seq no
tt_u32_t __logfld_seq_no_output(IN tt_logfld_t *lfld,
                                IN OUT tt_char_t *pos,
                                IN OUT tt_u32_t left_len,
                                IN struct tt_logfmt_s *lfmt)
{
    return tt_snprintf(pos, left_len, lfld->format, lfmt->seq_no++);
}

// logger
tt_u32_t __logfld_logger_output(IN tt_logfld_t *lfld,
                                IN OUT tt_char_t *pos,
                                IN OUT tt_u32_t left_len,
                                IN struct tt_logfmt_s *lfmt)
{
    if (lfmt->logger != NULL) {
        return tt_snprintf(pos, left_len, lfld->format, lfmt->logger);
    } else {
        return 0;
    }
}

// level
tt_u32_t __logfld_level_output(IN tt_logfld_t *lfld,
                               IN OUT tt_char_t *pos,
                               IN OUT tt_u32_t left_len,
                               IN struct tt_logfmt_s *lfmt)
{
    return tt_snprintf(pos,
                       left_len,
                       lfld->format,
                       __s_level_desc[lfmt->level]);
}
