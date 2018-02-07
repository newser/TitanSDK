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

#include <log/layout/tt_log_layout_pattern.h>

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_list.h>
#include <log/layout/tt_log_field.h>
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
} tt_loglyt_patn_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __llp_destroy(IN tt_loglyt_t *ll);

static tt_result_t __llp_format(IN tt_loglyt_t *ll,
                                IN tt_log_entry_t *entry,
                                OUT tt_buf_t *outbuf);

static tt_loglyt_itf_t __llp_itf = {
    NULL, __llp_destroy, __llp_format,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __llp_parse(IN tt_loglyt_patn_t *llp,
                               IN const tt_char_t *pattern,
                               IN OPT tt_char_t *out,
                               IN OPT tt_u32_t *out_len);

static tt_result_t __llp_create_field(IN tt_loglyt_patn_t *llp,
                                      IN const tt_char_t *start,
                                      IN const tt_char_t *end);

static void __llp_fields_destroy(IN tt_list_t *fields);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_loglyt_t *tt_loglyt_pattern_create(IN const tt_char_t *pattern)
{
    tt_loglyt_t *ll;
    tt_loglyt_patn_t *llp;

    if (pattern == NULL) {
        return NULL;
    }

    ll = tt_loglyt_create(sizeof(tt_loglyt_patn_t), &__llp_itf);
    if (ll == NULL) {
        return NULL;
    }

    llp = TT_LOGLYT_CAST(ll, tt_loglyt_patn_t);

    llp->patn = tt_malloc((tt_u32_t)tt_strlen(pattern) + 1);
    if (llp->patn == NULL) {
        tt_free(ll);
        return NULL;
    }
    llp->patn_len = 0;
    tt_list_init(&llp->fields);
    // init done, can use tt_loglyt_release() now

    if (!TT_OK(__llp_parse(llp, pattern, llp->patn, &llp->patn_len))) {
        tt_loglyt_release(ll);
        return NULL;
    }

    return ll;
}

void __llp_destroy(IN tt_loglyt_t *ll)
{
    tt_loglyt_patn_t *llp = TT_LOGLYT_CAST(ll, tt_loglyt_patn_t);

    __llp_fields_destroy(&llp->fields);
    tt_free(llp->patn);
}

tt_result_t __llp_format(IN tt_loglyt_t *ll,
                         IN tt_log_entry_t *entry,
                         OUT tt_buf_t *outbuf)
{
    tt_loglyt_patn_t *llp = TT_LOGLYT_CAST(ll, tt_loglyt_patn_t);
    tt_char_t *pos, *prev_pos, *end;
    tt_lnode_t *fnode;

    pos = llp->patn;
    prev_pos = pos;
    end = pos + llp->patn_len;
    fnode = tt_list_head(&llp->fields);

    while (pos < end) {
        if (*pos != 0) {
            ++pos;
            continue;
        }

        TT_DO(tt_buf_put(outbuf,
                         (tt_u8_t *)prev_pos,
                         (tt_u32_t)(pos - prev_pos)));
        ++pos;
        prev_pos = pos;

        if (fnode != NULL) {
            tt_logfld_output(TT_CONTAINER(fnode, tt_logfld_t, node),
                             entry,
                             outbuf);
            fnode = fnode->next;
        }
    }
    TT_DO(tt_buf_put(outbuf, (tt_u8_t *)prev_pos, (tt_u32_t)(pos - prev_pos)));

    return TT_SUCCESS;
}

tt_result_t __llp_parse(IN tt_loglyt_patn_t *llp,
                        IN const tt_char_t *pattern,
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
                    //  - ll->pattern has size of the raw pattern, which
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

                    if (!TT_OK(__llp_create_field(llp, f_start, f_end))) {
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

tt_result_t __llp_create_field(IN tt_loglyt_patn_t *llp,
                               IN const tt_char_t *start,
                               IN const tt_char_t *end)
{
    tt_logfld_t *lfld = tt_logfld_create(start, end);
    if (lfld != NULL) {
        tt_list_push_tail(&llp->fields, &lfld->node);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

void __llp_fields_destroy(IN tt_list_t *fields)
{
    tt_lnode_t *node;
    while ((node = tt_list_pop_head(fields)) != NULL) {
        tt_logfld_destroy(TT_CONTAINER(node, tt_logfld_t, node));
    }
}
