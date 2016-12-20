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

#include <log/layout/tt_log_layout_pattern.h>

#include <algorithm/tt_list.h>
#include <log/layout/tt_log_pattern_field.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_char_t *patn;
    tt_u32_t patn_len; // including ending 0
    tt_list_t fields;
    tt_u32_t seq_num;
} tt_loglyt_patn_t;

typedef tt_result_t (*__lytpatn_on_field_t)(IN tt_loglyt_patn_t *lytpatn,
                                            IN const tt_char_t *start,
                                            IN const tt_char_t *end);

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __lytpatn_destroy(IN tt_loglyt_t *lyt);

static tt_result_t __lytpatn_format(IN tt_loglyt_t *lyt,
                                    OUT struct tt_buf_s *outbuf,
                                    IN const tt_char_t *func,
                                    IN tt_u32_t line,
                                    IN const tt_char_t *format,
                                    IN va_list ap);

static tt_loglyt_itf_t __lytpatn_itf = {
    NULL, __lytpatn_destroy, __lytpatn_format,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __lytpatn_parse(IN tt_loglyt_patn_t *lp,
                                   IN const tt_char_t *pattern,
                                   IN __lytpatn_on_field_t on_field,
                                   IN OPT tt_char_t *out,
                                   IN OPT tt_u32_t *out_len);
static tt_result_t __lytpatn_on_field(IN tt_loglyt_patn_t *lp,
                                      IN const tt_char_t *start,
                                      IN const tt_char_t *end);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_loglyt_t *tt_loglyt_pattern_create(IN OPT const tt_char_t *logger,
                                      IN const tt_char_t *pattern)
{
    tt_loglyt_t *lyt;
    tt_loglyt_patn_t *lp;

    TT_ASSERT(pattern != NULL);

    lyt = tt_loglyt_create(sizeof(tt_loglyt_patn_t), logger, &__lytpatn_itf);
    if (lyt == NULL) {
        return NULL;
    }

    lp = TT_LOGLYT_CAST(lyt, tt_loglyt_patn_t);

    lp->patn = tt_mem_alloc((tt_u32_t)tt_strlen(pattern) + 1);
    if (lp->patn == NULL) {
        TT_ERROR("fail to copy pattern");
        tt_mem_free(lyt);
        return NULL;
    }
    lp->patn_len = 0;
    tt_list_init(&lp->fields);
    lp->seq_num = 0;
    // init done, can use tt_loglyt_destroy() now

    if (!TT_OK(__lytpatn_parse(lp,
                               pattern,
                               __lytpatn_on_field,
                               lp->patn,
                               &lp->patn_len))) {
        TT_ERROR("invalid log field");
        tt_loglyt_destroy(lyt);
        return NULL;
    }

    return lyt;
}

void __lytpatn_destroy(IN tt_loglyt_t *lyt)
{
    tt_loglyt_patn_t *lp = TT_LOGLYT_CAST(lyt, tt_loglyt_patn_t);
}

tt_result_t __lytpatn_format(IN tt_loglyt_t *lyt,
                             OUT struct tt_buf_s *outbuf,
                             IN const tt_char_t *func,
                             IN tt_u32_t line,
                             IN const tt_char_t *format,
                             IN va_list ap)
{
    return TT_SUCCESS;
}

tt_result_t __lytpatn_parse(IN tt_loglyt_patn_t *lp,
                            IN const tt_char_t *pattern,
                            IN __lytpatn_on_field_t on_field,
                            IN OPT tt_char_t *out,
                            IN OPT tt_u32_t *out_len)
{
    const tt_char_t *p = pattern;
    tt_char_t c;
    const tt_char_t *f_start = NULL, *f_end;

    const tt_char_t *seg_start = pattern;
    tt_u32_t seg_len = 0;
    tt_char_t *out_pos = out;
    tt_u32_t __out_len = 0;

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

                    // save out content
                    //  - each field is converted to a '\0', so pattern:
                    //    "aaa${field1}bb${field2}${field3}\0" is converted
                    //    to "aaa\0bb\0\0\0"
                    //  - lyt->pattern has size of the raw pattern, which
                    //    is always larger than size to store converted
                    //    pattern, so in this function, size is not checked.
                    //    stack overflow??
                    seg_len = (tt_u32_t)(tt_ptrdiff_t)(f_start - seg_start);
                    seg_len -= 1; // discard $
                    if (out_pos != NULL) {
                        tt_memcpy(out_pos, seg_start, seg_len);
                        out_pos += seg_len;
                        *out_pos++ = 0;
                    }
                    __out_len += (seg_len + 1);
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

                    if (!TT_OK(on_field(lp, f_start, f_end))) {
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
        TT_ERROR("invalid log field");
        return TT_FAIL;
    }

    seg_len = (tt_u32_t)(tt_ptrdiff_t)(p - seg_start);
    if (out_pos != NULL) {
        tt_memcpy(out_pos, seg_start, seg_len);
        out_pos += seg_len;
        *out_pos++ = 0;
    }
    __out_len += (seg_len + 1);

    if (out_len != NULL) {
        *out_len = __out_len;
    }

    return TT_SUCCESS;
}

tt_result_t __lytpatn_on_field(IN tt_loglyt_patn_t *lytpatn,
                               IN const tt_char_t *start,
                               IN const tt_char_t *end)
{
    tt_lpfld_t *lpfld = tt_lpfld_create(start, end);
    if (lpfld != NULL) {
        tt_list_addtail(&lytpatn->fields, &lpfld->node);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

void __lytpatn_list_destroy(IN tt_list_t *lfld_list)
{
    tt_lnode_t *node;
    while ((node = tt_list_pophead(lfld_list)) != NULL) {
        tt_logfld_destroy(TT_CONTAINER(node, tt_logfld_t, node));
    }
}
