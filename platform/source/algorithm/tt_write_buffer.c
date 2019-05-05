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

#include <algorithm/tt_write_buffer.h>

#include <misc/tt_assert.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern enclaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface enclaration
////////////////////////////////////////////////////////////

static tt_result_t __wbuf_render(IN tt_wbuf_t *wbuf, IN void *to_render,
                                 OUT OPT tt_u8_t **rendered,
                                 OUT OPT tt_u32_t *len);

static tt_result_t __wbuf_encode(IN tt_wbuf_t *wbuf);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_wbuf_init(IN tt_wbuf_t *wbuf, IN OPT tt_wbuf_encode_itf_t *e_itf,
                  IN OPT void *e_param, IN tt_wbuf_render_itf_t *r_itf,
                  IN OPT void *r_param, IN OPT tt_wbuf_attr_t *attr)
{
    tt_wbuf_attr_t __attr;

    TT_ASSERT(wbuf != NULL);
    TT_ASSERT((e_itf == NULL) ||
              ((e_itf->prepare != NULL) && (e_itf->encode != NULL)));
    TT_ASSERT((r_itf != NULL) && (r_itf->prepare != NULL) &&
              (r_itf->render != NULL));

    if (attr == NULL) {
        tt_wbuf_attr_default(&__attr);
        attr = &__attr;
    }

    wbuf->e_itf = e_itf;
    wbuf->e_param = e_param;
    wbuf->r_itf = r_itf;
    wbuf->r_param = r_param;
    tt_buf_init(&wbuf->raw, &attr->rawbuf_attr);
    tt_buf_init(&wbuf->enc, &attr->encbuf_attr);
    wbuf->refine_threshold = attr->refine_threshold;
}

void tt_wbuf_destroy(IN tt_wbuf_t *wbuf)
{
    TT_ASSERT(wbuf != NULL);

    tt_buf_destroy(&wbuf->raw);
    tt_buf_destroy(&wbuf->enc);
}

void tt_wbuf_attr_default(IN tt_wbuf_attr_t *attr)
{
    tt_buf_attr_default(&attr->rawbuf_attr);
    tt_buf_attr_default(&attr->encbuf_attr);

    attr->refine_threshold = 1024;
}

tt_result_t tt_wbuf_render(IN tt_wbuf_t *wbuf, IN void *to_render,
                           OUT OPT tt_u8_t **rendered, OUT OPT tt_u32_t *len)
{
    if (!TT_OK(__wbuf_render(wbuf, to_render, rendered, len))) {
        return TT_FAIL;
    }
    // do not refine wbuf in this function, otherwise rendered and len
    // would be an invalid memory region

    TT_DO(__wbuf_encode(wbuf));

    return TT_SUCCESS;
}

tt_result_t tt_wbuf_put(IN tt_wbuf_t *wbuf, IN tt_u8_t *data,
                        IN tt_u32_t data_len)
{
    TT_DO(tt_buf_put(&wbuf->raw, data, data_len));

    TT_DO(__wbuf_encode(wbuf));

    return TT_SUCCESS;
}

void tt_wbuf_get_rptr(IN tt_wbuf_t *wbuf, IN tt_u8_t **p, IN tt_u32_t *len)
{
    if (wbuf->e_itf != NULL) {
        tt_buf_get_rptr(&wbuf->enc, p, len);
    } else {
        tt_buf_get_rptr(&wbuf->raw, p, len);
    }
}

tt_result_t tt_wbuf_inc_rp(IN tt_wbuf_t *wbuf, IN tt_u32_t num)
{
    if (wbuf->e_itf != NULL) {
        tt_buf_inc_rp(&wbuf->enc, num);
        tt_buf_try_refine(&wbuf->raw, wbuf->refine_threshold);
        tt_buf_try_refine(&wbuf->enc, wbuf->refine_threshold);
    } else {
        tt_buf_inc_rp(&wbuf->raw, num);
        tt_buf_try_refine(&wbuf->raw, wbuf->refine_threshold);
    }
    return TT_SUCCESS;
}

tt_result_t __wbuf_render(IN tt_wbuf_t *wbuf, IN void *to_render,
                          OUT OPT tt_u8_t **rendered, OUT OPT tt_u32_t *len)
{
    tt_wbuf_render_itf_t *r_itf = wbuf->r_itf;
    void *r_param = wbuf->r_param;
    tt_u32_t n, rp, wp;
    tt_buf_t *raw = &wbuf->raw;

    n = r_itf->prepare(to_render, r_param);
    if (n == 0) { return TT_FAIL; }

    tt_buf_backup_rwp(raw, &rp, &wp);
    if (TT_OK(r_itf->render(raw, n, to_render, r_param))) {
        TT_ASSERT(wp < raw->wpos);
        if ((raw->wpos - wp) != n) {
            TT_WARN("prepare %d, rendered %d", n, raw->rpos - rp);
            n = raw->wpos - wp;
        }
        TT_SAFE_ASSIGN(rendered, TT_BUF_WPOS(raw) - n);
        TT_SAFE_ASSIGN(len, n);

        return TT_SUCCESS;
    } else {
        tt_buf_restore_rwp(raw, &rp, &wp);
        return TT_FAIL;
    }
}

tt_result_t __wbuf_encode(IN tt_wbuf_t *wbuf)
{
    tt_wbuf_encode_itf_t *e_itf;
    void *e_param;
    tt_buf_t *raw, *enc;
    tt_bool_t has_enc = TT_FALSE;

    if (wbuf->e_itf == NULL) { return TT_SUCCESS; }
    e_itf = wbuf->e_itf;
    e_param = wbuf->e_param;

    raw = &wbuf->raw;
    enc = &wbuf->enc;
    while (!tt_buf_empty(raw)) {
        tt_u32_t rp, wp, len = 0, e_rp, e_wp;
        tt_result_t result;

        tt_buf_backup_rwp(raw, &rp, &wp);
        result = e_itf->prepare(raw, &len, e_param);
        tt_buf_restore_rwp(raw, &rp, &wp);
        if (result == TT_E_BUF_NOBUFS) {
            // can not return, as there may be data already encoded
            break;
        } else if (!TT_OK(result)) {
            // return fail even if there are encoded data, as it even
            // does not know how to do next
            return TT_FAIL;
        }
        TT_ASSERT((len != 0) && (len <= TT_BUF_RLEN(raw)));
        // now len indicates how many bytes in raw would be encoded

        tt_buf_backup_rwp(enc, &e_rp, &e_wp);
        if (!TT_OK(e_itf->encode(raw, len, enc, e_param))) {
            tt_buf_restore_rwp(raw, &rp, &wp);
            tt_buf_restore_rwp(enc, &e_rp, &e_wp);
            return TT_FAIL;
        }
        has_enc = TT_TRUE;
    }

    if (has_enc) {
        return TT_SUCCESS;
    } else {
        return TT_E_BUF_NOBUFS;
    }
}
