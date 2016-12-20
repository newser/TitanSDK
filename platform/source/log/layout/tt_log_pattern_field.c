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

#include <log/layout/tt_log_pattern_field.h>

#include <algorithm/tt_buffer_format.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <timer/tt_time_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_lpfld_t *(*__lpf_create_t)(IN tt_lpfld_type_t type,
                                      IN const tt_char_t *fmt_start,
                                      IN const tt_char_t *fmt_end);

typedef void (*__lpf_destroy_t)(IN tt_lpfld_t *lpf);

typedef tt_result_t (*__lpf_check_t)(IN const tt_char_t *fmt_start,
                                     IN const tt_char_t *fmt_end);

typedef tt_result_t (*__lpf_output_t)(IN tt_lpfld_t *lpf, OUT tt_buf_t *outbuf);

typedef tt_result_t (*__lpf_output_cstr_t)(IN tt_lpfld_t *lpf,
                                        IN const tt_char_t *cstr_val,
                                        OUT tt_buf_t *outbuf);

typedef tt_result_t (*__lpf_output_s32_t)(IN tt_lpfld_t *lpf,
                                       IN tt_s32_t s32_val,
                                       OUT tt_buf_t *outbuf);

typedef struct
{
    const tt_char_t *name;
    tt_lpfld_type_t type;
    const tt_char_t *default_format;

    __lpf_create_t create;
    __lpf_destroy_t destroy;
    __lpf_check_t check;

    __lpf_output_t output;
    __lpf_output_cstr_t output_cstr;
    __lpf_output_s32_t output_s32;
} __lpf_entry_t;

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

static tt_lpfld_t *__lpf_create(IN tt_lpfld_type_t type,
                                IN const tt_char_t *fmt_start,
                                IN const tt_char_t *fmt_end);

static void __lpf_destroy(IN tt_lpfld_t *lpf);

static tt_result_t __lpf_check(IN const tt_char_t *start,
                               IN const tt_char_t *end);

static tt_result_t __lpf_output_s32(IN tt_lpfld_t *lpf,
                                    IN tt_s32_t seq_num,
                                    OUT tt_buf_t *outbuf);

static tt_result_t __lpf_output_cstr(IN tt_lpfld_t *lpf,
                                     IN const tt_char_t *cstr_val,
                                     OUT tt_buf_t *outbuf);

static tt_result_t __lpf_time_output(IN tt_lpfld_t *lpf, OUT tt_buf_t *outbuf);

static __lpf_entry_t __lpf_table[] = {
    {
        TT_LPFLD_SEQ_NUM_KEY,
        TT_LPFLD_SEQ_NUM,
        "%d",

        __lpf_create,
        __lpf_destroy,
        __lpf_check,

        NULL,
        NULL,
        __lpf_output_s32,
    },

    {
        TT_LPFLD_TIME_KEY,
        TT_LPFLD_TIME,
        "%Y-%m-%d %H:%M:%S",

        __lpf_create,
        __lpf_destroy,
        __lpf_check,

        __lpf_time_output,
        NULL,
        NULL,
    },

    {
        TT_LPFLD_LOGGER_KEY,
        TT_LPFLD_LOGGER,
        "%s",

        __lpf_create,
        __lpf_destroy,
        __lpf_check,

        NULL,
        __lpf_output_cstr,
        NULL,
    },

    {
        TT_LPFLD_LEVEL_KEY,
        TT_LPFLD_LEVEL,
        "%s",

        __lpf_create,
        __lpf_destroy,
        __lpf_check,

        NULL,
        __lpf_output_cstr,
        NULL,
    },

    {
        TT_LPFLD_CONTENT_KEY,
        TT_LPFLD_CONTENT,
        "%s",

        __lpf_create,
        __lpf_destroy,
        __lpf_check,

        NULL,
        __lpf_output_cstr,
        NULL,
    },

    {
        TT_LPFLD_FUNC_KEY,
        TT_LPFLD_FUNC,
        "%s",

        __lpf_create,
        __lpf_destroy,
        __lpf_check,

        NULL,
        __lpf_output_cstr,
        NULL,
    },

    {
        TT_LPFLD_LINE_KEY,
        TT_LPFLD_LINE,
        "%d",

        __lpf_create,
        __lpf_destroy,
        __lpf_check,

        NULL,
        NULL,
        __lpf_output_s32,
    },
};

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_lpfld_t *tt_lpfld_create(IN const tt_char_t *start, IN const tt_char_t *end)
{
    tt_u32_t flen;
    tt_u32_t i;

    TT_ASSERT(start < (end - 1));
    flen = (tt_u32_t)(tt_ptrdiff_t)(end - start);

    // discard {}
    start += 1;
    flen -= 1;

    for (i = 0; i < sizeof(__lpf_table) / sizeof(__lpf_table[0]); ++i) {
        __lpf_entry_t *lpfe = &__lpf_table[i];
        tt_u32_t name_len = (tt_u32_t)tt_strlen(lpfe->name);

        if ((name_len <= flen) &&
            (tt_strncmp(start, lpfe->name, name_len) == 0) &&
            TT_OK(lpfe->check(start + name_len, end))) {
            return lpfe->create(lpfe->type, start + name_len, end);
        }
    }
    return NULL;
}

void tt_lpfld_destroy(IN tt_lpfld_t *lpf)
{
    __lpf_entry_t *lpfe;

    TT_ASSERT(TT_LPFLD_TYPE_VALID(lpf->type));

    lpfe = &__lpf_table[lpf->type];
    lpfe->destroy(lpf);
}

tt_result_t tt_lpfld_check(IN const tt_char_t *start, IN const tt_char_t *end)
{
    tt_u32_t flen;
    tt_u32_t i;

    TT_ASSERT(start < (end - 1));
    flen = (tt_u32_t)(tt_ptrdiff_t)(end - start);

    // discard {}
    start += 1;
    flen -= 1;

    for (i = 0; i < sizeof(__lpf_table) / sizeof(__lpf_table[0]); ++i) {
        __lpf_entry_t *lpfe = &__lpf_table[i];
        tt_u32_t name_len = (tt_u32_t)tt_strlen(lpfe->name);

        if ((name_len <= flen) &&
            (tt_strncmp(start, lpfe->name, name_len) == 0)) {
            return lpfe->check(start + name_len, end);
        }
    }
    return TT_FAIL;
}

tt_result_t tt_lpfld_output(IN tt_lpfld_t *lpf, OUT struct tt_buf_s *outbuf)
{
    __lpf_entry_t *lpfe = &__lpf_table[lpf->type];

    return TT_COND(lpfe->output != NULL,
                   lpfe->output(lpf, outbuf),
                   TT_FAIL);
}

tt_result_t tt_lpfld_output_cstr(IN tt_lpfld_t *lpf,
                                 IN const tt_char_t *cstr_val,
                                 OUT tt_buf_t *outbuf)
{
    __lpf_entry_t *lpfe = &__lpf_table[lpf->type];

    return TT_COND(lpfe->output_cstr != NULL,
                   lpfe->output_cstr(lpf, cstr_val, outbuf),
                   TT_FAIL);
}

tt_result_t tt_lpfld_output_s32(IN tt_lpfld_t *lpf,
                                IN tt_s32_t s32_val,
                                OUT tt_buf_t *outbuf)
{
    __lpf_entry_t *lpfe = &__lpf_table[lpf->type];

    return TT_COND(lpfe->output_s32 != NULL,
                   lpfe->output_s32(lpf, s32_val, outbuf),
                   TT_FAIL);
}

tt_lpfld_t *__lpf_create(IN tt_lpfld_type_t type,
                         IN const tt_char_t *fmt_start,
                         IN const tt_char_t *fmt_end)
{
    tt_u32_t fmt_len;
    tt_lpfld_t *lpf;

    if (fmt_start + 1 < fmt_end) {
        // ignore ":"
        ++fmt_start;
        fmt_len = (tt_u32_t)(tt_ptrdiff_t)(fmt_end - fmt_start);
    } else {
        fmt_start = __lpf_table[type].default_format;
        fmt_len = (tt_u32_t)tt_strlen(__lpf_table[type].default_format);
    }

    lpf = (tt_lpfld_t *)tt_mem_alloc(sizeof(tt_lpfld_t) + fmt_len + 1);
    if (lpf != NULL) {
        tt_lnode_init(&lpf->node);
        lpf->type = type;

        lpf->format = TT_PTR_INC(tt_char_t, lpf, sizeof(tt_lpfld_t));
        tt_memcpy(lpf->format, fmt_start, fmt_len);
        lpf->format[fmt_len] = 0;

        return lpf;
    } else {
        return NULL;
    }
}

void __lpf_destroy(IN tt_lpfld_t *lpf)
{
    TT_ASSERT(lpf->node.lst == NULL);

    tt_mem_free(lpf);
}

tt_result_t __lpf_check(IN const tt_char_t *start, IN const tt_char_t *end)
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

tt_result_t __lpf_output_s32(IN tt_lpfld_t *lpf,
                             IN tt_s32_t s32_val,
                             OUT tt_buf_t *outbuf)
{
    return tt_buf_vput(outbuf, lpf->format, s32_val);
}

tt_result_t __lpf_output_cstr(IN tt_lpfld_t *lpf,
                              IN const tt_char_t *cstr_val,
                              OUT tt_buf_t *outbuf)
{
    return tt_buf_vput(outbuf, lpf->format, cstr_val);
}

tt_result_t __lpf_time_output(IN tt_lpfld_t *lpf, OUT tt_buf_t *outbuf)
{
    tt_u8_t *p;
    tt_u32_t len;

    // reserving 100 bytes should be enough
    TT_DO(tt_buf_reserve(outbuf, 100));
    p = TT_BUF_WPOS(outbuf);
    len = TT_BUF_WLEN(outbuf);

    len = tt_time_localfmt((tt_char_t *)p, len, lpf->format);
    tt_buf_inc_wp(outbuf, len);

    return TT_SUCCESS;
}
