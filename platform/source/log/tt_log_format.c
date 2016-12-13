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

#include <log/tt_log_format.h>

#include <misc/tt_util.h>
#include <os/tt_spinlock.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

// one line of source code is generally less than 100
#define __LOGFMT_SIZE TT_LOG_BUF_INIT_SIZE

#define __LOGFMT_MAX_SIZE TT_LOG_BUF_MAX_SIZE
#if (__LOGFMT_MAX_SIZE < __LOGFMT_SIZE)
#error max log size is less than initial size
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_result_t (*__logfld_do_t)(IN tt_logfmt_t *lfmt,
                                     IN const tt_char_t *start,
                                     IN const tt_char_t *end);

typedef struct
{
    tt_lnode_t node;
    tt_logio_t *lio;
} __logio_link_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __log_pattern_travel(IN tt_logfmt_t *lfmt,
                                        IN const tt_char_t *pattern,
                                        IN __logfld_do_t field_do,
                                        IN OPT tt_char_t *extra,
                                        IN OPT tt_u32_t *extra_len);

static tt_result_t __log_pattern_parse(IN tt_logfmt_t *lfmt,
                                       IN const tt_char_t *start,
                                       IN const tt_char_t *end);

static void __logio_list_destroy(IN tt_list_t *lio_list);

static void __logfld_list_destroy(IN tt_list_t *lfld_list);

static void __logfmt_input_flf(IN tt_logfmt_t *lfmt,
                               IN const tt_char_t *func,
                               IN tt_u32_t line,
                               IN const tt_char_t *format,
                               IN va_list args);

static void __logfmt_output(IN tt_logfmt_t *lfmt);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_logfmt_create(IN tt_logfmt_t *lfmt,
                             IN tt_log_level_t level,
                             IN const tt_char_t *pattern,
                             IN OPT tt_u32_t buf_size,
                             IN OPT const tt_char_t *logger)
{
    tt_result_t result;

    if (lfmt == NULL) {
        return TT_BAD_PARAM;
    }

    if (pattern == NULL) {
        return TT_BAD_PARAM;
    }

    tt_list_init(&lfmt->lio_list);

    lfmt->lock = NULL;

    if (buf_size == 0) {
        buf_size = __LOGFMT_SIZE;
        lfmt->can_expand = TT_TRUE;
    } else {
        lfmt->can_expand = TT_FALSE;
    }

    // log may be created when other components are not
    // initialized, so use malloc
    lfmt->buf = (tt_char_t *)tt_malloc(buf_size);
    if (lfmt->buf == NULL) {
        return TT_NO_RESOURCE;
    }
    lfmt->buf_size = buf_size - 1;
    lfmt->buf_pos = 0;

    // pattern is initialized later
    lfmt->pattern_len = (tt_u32_t)tt_strlen(pattern) + 1;
    lfmt->pattern_content_len = 0;
    lfmt->pattern = (tt_char_t *)tt_malloc(lfmt->pattern_len);
    if (lfmt->pattern == NULL) {
        tt_free(lfmt->buf);
        return TT_NO_RESOURCE;
    }
    tt_list_init(&lfmt->lfld_list);

    lfmt->seq_no = 0;

    lfmt->logger = logger;

    lfmt->level = level;

    lfmt->enabled = TT_TRUE;

    // validate pattern
    result =
        __log_pattern_travel(lfmt, pattern, tt_logfld_validate, NULL, NULL);
    if (!TT_OK(result)) {
        tt_free(lfmt->buf);
        tt_free(lfmt->pattern);
        return TT_FAIL;
    }

    // parse pattern
    result = __log_pattern_travel(lfmt,
                                  pattern,
                                  __log_pattern_parse,
                                  lfmt->pattern,
                                  &lfmt->pattern_content_len);
    if (!TT_OK(result)) {
        __logfld_list_destroy(&lfmt->lfld_list);
        tt_free(lfmt->buf);
        tt_free(lfmt->pattern);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_logfmt_destroy(IN tt_logfmt_t *lfmt)
{
    __logio_list_destroy(&lfmt->lio_list);

    if (lfmt->lock != NULL) {
        tt_spinlock_destroy(lfmt->lock);
        tt_free(lfmt->lock);
    }

    __logfld_list_destroy(&lfmt->lfld_list);

    tt_free(lfmt->buf);

    tt_free(lfmt->pattern);
}

tt_result_t tt_logfmt_add_io(IN tt_logfmt_t *lfmt, IN tt_logio_t *lio)
{
    tt_lnode_t *node;
    __logio_link_t *ll;

    node = tt_list_head(&lfmt->lio_list);
    while (node != NULL) {
        __logio_link_t *ll = TT_CONTAINER(node, __logio_link_t, node);
        node = node->next;

        if (ll->lio == lio) {
            return TT_ALREADY_EXIST;
        }
    }

    // use __logio_link_t could add single logio to multple logfmt
    ll = tt_malloc(sizeof(__logio_link_t));
    if (ll == NULL) {
        return TT_FAIL;
    }

    tt_lnode_init(&ll->node);
    ll->lio = lio;

    tt_list_addtail(&lfmt->lio_list, &ll->node);

    return TT_SUCCESS;
}

void tt_logfmt_input_flf(IN tt_logfmt_t *lfmt,
                         IN const tt_char_t *func,
                         IN tt_u32_t line,
                         IN const tt_char_t *format,
                         ...)
{
    va_list args;

    if ((lfmt == NULL) || !lfmt->enabled) {
        return;
    }

#ifndef TT_LOG_ENABLE
    return;
#endif

    // hold lock as multiple threads may input at same time
    if (lfmt->lock != NULL) {
        tt_spinlock_acquire(lfmt->lock);
    }

    va_start(args, format);
    __logfmt_input_flf(lfmt, func, line, format, args);
    va_end(args);

    if (lfmt->lock != NULL) {
        tt_spinlock_release(lfmt->lock);
    }
}

void tt_logfmt_output(IN tt_logfmt_t *lfmt)
{
    if ((lfmt == NULL) || !lfmt->enabled) {
        return;
    }

#ifndef TT_LOG_ENABLE
    return;
#endif

    // hold lock so other thread can not modify log that
    // is to be output
    if (lfmt->lock != NULL) {
        tt_spinlock_acquire(lfmt->lock);
    }

    // each io has its own lock
    __logfmt_output(lfmt);

    if (lfmt->lock != NULL) {
        tt_spinlock_release(lfmt->lock);
    }
}

tt_result_t tt_logfmt_enable_lock(IN tt_logfmt_t *lfmt)
{
    tt_spinlock_t *lock;

    // this function should be called when platform initialization
    // is finished

    if (lfmt->lock != NULL) {
        return TT_SUCCESS;
    }

    lock = tt_malloc(sizeof(tt_spinlock_t));
    if (lock == NULL) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_spinlock_create(lock, NULL))) {
        tt_free(lock);
        return TT_FAIL;
    }

    lfmt->lock = lock;
    return TT_SUCCESS;
}

tt_result_t __log_pattern_travel(IN tt_logfmt_t *lfmt,
                                 IN const tt_char_t *pattern,
                                 IN __logfld_do_t field_do,
                                 IN OPT tt_char_t *extra,
                                 IN OPT tt_u32_t *extra_len)
{
    const tt_char_t *p = pattern;
    tt_char_t c;
    const tt_char_t *f_start = NULL, *f_end;

    const tt_char_t *seg_start = pattern;
    tt_u32_t seg_len = 0;
    tt_char_t *extra_pos = extra;
    tt_u32_t __extra_len = 0;

#define __LPP_INIT 0
#define __LPP_DOLLER 1 // see "$"
#define __LPP_FIELD 2 // see "${"
    tt_u32_t state = __LPP_INIT;

    while ((c = *p) != 0) {
        switch (c) {
            case '$': {
                if (state == __LPP_INIT) {
                    // "$"
                    state = __LPP_DOLLER;
                }
            } break;
            case '{': {
                if (state == __LPP_DOLLER) {
                    // "${"
                    state = __LPP_FIELD;
                    f_start = p;

                    // save extra content
                    //  - each field is converted to a '\0', so pattern:
                    //    "aaa${field1}bb${field2}${field3}\0" is converted
                    //    to "aaa\0bb\0\0\0"
                    //  - lfmt->pattern has size of the raw pattern, which
                    //    is always larger than size to store converted
                    //    pattern, so in this function, size is not checked.
                    //    stack overflow??
                    seg_len = (tt_u32_t)(tt_ptrdiff_t)(f_start - seg_start);
                    seg_len -= 1; // discard $
                    if (extra_pos != NULL) {
                        tt_memcpy(extra_pos, seg_start, seg_len);
                        extra_pos += seg_len;
                        *extra_pos++ = 0;
                    }
                    __extra_len += (seg_len + 1);
                }
            } break;
            case '}': {
                if (state == __LPP_DOLLER) {
                    // "$}"
                    state = __LPP_INIT;
                } else if (state == __LPP_FIELD) {
                    // "${...}"
                    state = __LPP_INIT;
                    f_end = p;

                    if (!TT_OK(field_do(lfmt, f_start, f_end))) {
                        return TT_FAIL;
                    }

                    seg_start = (p + 1);
                }
            } break;

            default: {
                if (state == __LPP_DOLLER) {
                    state = __LPP_INIT;
                }
            } break;
        }

        ++p;
    }

    if (state == __LPP_FIELD) {
        // partial field: ${...\0
        return TT_FAIL;
    }

    seg_len = (tt_u32_t)(tt_ptrdiff_t)(p - seg_start);
    if (extra_pos != NULL) {
        tt_memcpy(extra_pos, seg_start, seg_len);
        extra_pos += seg_len;
        *extra_pos++ = 0;
    }
    __extra_len += (seg_len + 1);

    if (extra_len != NULL) {
        *extra_len = __extra_len;
    }

    return TT_SUCCESS;
}

tt_result_t __log_pattern_parse(IN tt_logfmt_t *lfmt,
                                IN const tt_char_t *start,
                                IN const tt_char_t *end)
{
    tt_logfld_t *lfld = tt_logfld_create(lfmt, start, end);
    if (lfld != NULL) {
        tt_list_addtail(&lfmt->lfld_list, &lfld->node);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

void __logio_list_destroy(IN tt_list_t *lio_list)
{
    tt_lnode_t *node;
    while ((node = tt_list_pophead(lio_list)) != NULL) {
        // note here it only free the __logio_link_t, as the logio does
        // not belong to the logfmt
        tt_free(TT_CONTAINER(node, __logio_link_t, node));
    }
}

void __logfld_list_destroy(IN tt_list_t *lfld_list)
{
    tt_lnode_t *node;
    while ((node = tt_list_pophead(lfld_list)) != NULL) {
        tt_logfld_destroy(TT_CONTAINER(node, tt_logfld_t, node));
    }
}

void __logfmt_input_flf(IN tt_logfmt_t *lfmt,
                        IN const tt_char_t *func,
                        IN tt_u32_t line,
                        IN const tt_char_t *format,
                        IN va_list args)
{
    tt_lnode_t *lfld_node;
    tt_char_t *in_pos, *out_pos;
    tt_u32_t in_left_len, out_left_len;
    tt_bool_t need_expand;

    lfld_node = tt_list_head(&lfmt->lfld_list);

    in_pos = lfmt->pattern;
    in_left_len = lfmt->pattern_content_len;

save_ag:

    out_pos = &lfmt->buf[lfmt->buf_pos];
    out_left_len = lfmt->buf_size - lfmt->buf_pos;

    need_expand = TT_FALSE;

    while ((out_left_len > 0) && (in_left_len > 0)) {
        if (*in_pos != 0) {
            *out_pos++ = *in_pos++;
            --out_left_len;
            --in_left_len;
        } else {
            tt_logfld_t *lfld;
            tt_u32_t n = 0;

            // 0 in lfmt->pattern means a log field

            if (lfld_node == NULL) {
                ++in_pos;
                --in_left_len;
                continue;
            }

            lfld = TT_CONTAINER(lfld_node, tt_logfld_t, node);
            switch (lfld->type) {
                case TT_LOGFLD_FUNC: {
                    if (func != NULL) {
                        n = tt_snprintf(out_pos, out_left_len, "%s", func);
                    }
                } break;
                case TT_LOGFLD_LINE: {
                    n = tt_snprintf(out_pos, out_left_len, "%d", line);
                } break;
                case TT_LOGFLD_CONTENT: {
                    n = tt_vsnprintf(out_pos, out_left_len, format, args);
                } break;
                default: {
                    n = tt_logfld_output(lfld, out_pos, out_left_len, lfmt);
                } break;
            }

            TT_ASSERT(out_left_len >= n);
            if ((out_left_len - n) <= 1) {
                // tt_vsnprintf would always append terminating null and
                // return (out_left_len - 1) when buf is full, so here
                // we'll force a buf expanding and all other pos and len
                // are not updated
                need_expand = TT_TRUE;
                break;
            }

            lfld_node = lfld_node->next;

            ++in_pos;
            --in_left_len;

            out_pos += n;
            out_left_len -= n;
        }
    }

    TT_ASSERT(out_left_len <= lfmt->buf_size);
    lfmt->buf_pos = lfmt->buf_size - out_left_len;

    if (((out_left_len == 0) || need_expand) && lfmt->can_expand &&
        (lfmt->buf_size < __LOGFMT_MAX_SIZE)) {
        tt_u32_t new_buf_size;
        tt_char_t *new_buf;

        new_buf_size = lfmt->buf_size * 2;
        if (new_buf_size > __LOGFMT_MAX_SIZE) {
            new_buf_size = __LOGFMT_MAX_SIZE;
        }

        new_buf = tt_malloc(new_buf_size);
        if (new_buf != NULL) {
            tt_memcpy(new_buf, lfmt->buf, lfmt->buf_size);
            tt_free(lfmt->buf);
            lfmt->buf = new_buf;
            lfmt->buf_size = new_buf_size;
            goto save_ag;
        }
    } else if (out_left_len > 0) {
        *out_pos = 0;
    } else {
        lfmt->buf[lfmt->buf_size - 1] = 0;
    }
}

void __logfmt_output(IN tt_logfmt_t *lfmt)
{
    tt_lnode_t *node;

    node = tt_list_head(&lfmt->lio_list);
    while (node != NULL) {
        tt_logio_t *lio = TT_CONTAINER(node, __logio_link_t, node)->lio;
        node = node->next;

        lio->itf.output(lio, (tt_u8_t *)lfmt->buf, lfmt->buf_pos);
    }

    // sending or saving log is the responsibility of
    // log io but not log format, so assume all are sent
    lfmt->buf_pos = 0;
}
