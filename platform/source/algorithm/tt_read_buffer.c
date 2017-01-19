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

#include <algorithm/tt_read_buffer.h>

#include <misc/tt_assert.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __rbuf_decode(IN tt_rbuf_t *rbuf, OUT tt_buf_t **data);

static tt_result_t __rbuf_parse(IN tt_rbuf_t *rbuf, IN tt_buf_t *data);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_rbuf_init(IN tt_rbuf_t *rbuf,
                  IN OPT tt_rbuf_decode_itf_t *d_itf,
                  IN OPT void *d_param,
                  IN tt_rbuf_parse_itf_t *p_itf,
                  IN OPT void *p_param,
                  IN OPT tt_rbuf_attr_t *attr)
{
    tt_rbuf_attr_t __attr;

    TT_ASSERT(rbuf != NULL);
    TT_ASSERT((d_itf == NULL) ||
              ((d_itf->prepare != NULL) && (d_itf->decode != NULL)));
    TT_ASSERT((p_itf != NULL) && (p_itf->prepare != NULL) &&
              (p_itf->parse != NULL) && (p_itf->done != NULL));

    if (attr == NULL) {
        tt_rbuf_attr_default(&__attr);
        attr = &__attr;
    }

    rbuf->d_itf = d_itf;
    rbuf->d_param = d_param;
    rbuf->p_itf = p_itf;
    rbuf->p_param = p_param;
    tt_buf_init(&rbuf->raw, &attr->rawbuf_attr);
    tt_buf_init(&rbuf->dec, &attr->decbuf_attr);
    rbuf->refine_threshold = attr->refine_threshold;
}

void tt_rbuf_destroy(IN tt_rbuf_t *rbuf)
{
    TT_ASSERT(rbuf != NULL);

    tt_buf_destroy(&rbuf->raw);
    tt_buf_destroy(&rbuf->dec);
}

void tt_rbuf_attr_default(IN tt_rbuf_attr_t *attr)
{
    tt_buf_attr_default(&attr->rawbuf_attr);
    tt_buf_attr_default(&attr->decbuf_attr);

    attr->refine_threshold = 1024;
}

tt_result_t tt_rbuf_inc_wp(IN tt_rbuf_t *rbuf, IN tt_u32_t num)
{
    tt_buf_t *data;

    tt_buf_inc_wp(&rbuf->raw, num);

    TT_DO(__rbuf_decode(rbuf, &data));
    TT_ASSERT(data != NULL);

    TT_DO(__rbuf_parse(rbuf, data));

    return TT_SUCCESS;
}

tt_result_t __rbuf_decode(IN tt_rbuf_t *rbuf, OUT tt_buf_t **data)
{
    tt_rbuf_decode_itf_t *d_itf;
    void *d_param;
    tt_buf_t *raw, *dec;
    tt_bool_t has_dec;

    if (rbuf->d_itf == NULL) {
        *data = &rbuf->raw;
        return TT_SUCCESS;
    }
    d_itf = rbuf->d_itf;
    d_param = rbuf->d_param;

    raw = &rbuf->raw;
    dec = &rbuf->dec;
    has_dec = TT_FALSE;
    while (!tt_buf_empty(raw)) {
        tt_u32_t rp, wp, len = 0, d_rp, d_wp;
        tt_result_t result;

        tt_buf_backup_rwp(raw, &rp, &wp);
        result = d_itf->prepare(raw, &len, d_param);
        tt_buf_restore_rwp(raw, &rp, &wp);
        if (result == TT_BUFFER_INCOMPLETE) {
            // can not return, as there may be data already decoded
            break;
        } else if (!TT_OK(result)) {
            // return fail even if there are decoded data, as it even
            // does not know how to do next
            return TT_FAIL;
        }
        TT_ASSERT((len != 0) && (len <= TT_BUF_RLEN(raw)));
        // now len indicates how many bytes in raw would be decoded

        tt_buf_backup_rwp(dec, &d_rp, &d_wp);
        result = d_itf->decode(raw, len, dec, d_param);
        if (!TT_OK(result)) {
            // ignore data in raw
            tt_buf_restore_rwp(raw, &rp, &wp);
            tt_buf_inc_rp(raw, len);

            tt_buf_restore_rwp(dec, &d_rp, &d_wp);

            if (result != TT_PROCEEDING) {
                return TT_FAIL;
            }
        }
        has_dec = TT_TRUE;
    }

    if (has_dec) {
        tt_buf_try_refine(raw, rbuf->refine_threshold);
        *data = &rbuf->dec;
        return TT_SUCCESS;
    } else {
        return TT_BUFFER_INCOMPLETE;
    }
}

tt_result_t __rbuf_parse(IN tt_rbuf_t *rbuf, IN tt_buf_t *data)
{
    tt_rbuf_parse_itf_t *p_itf = rbuf->p_itf;
    void *p_param = rbuf->p_param;
    tt_bool_t has_parse = TT_FALSE;

    while (!tt_buf_empty(data)) {
        tt_u32_t rp, wp, len = 0;
        tt_result_t result;
        void *parse_ret = NULL;

        tt_buf_backup_rwp(data, &rp, &wp);
        result = p_itf->prepare(data, &len, p_param);
        tt_buf_restore_rwp(data, &rp, &wp);
        if (result == TT_BUFFER_INCOMPLETE) {
            // can not return, as there may be data already parsed
            break;
        } else if (!TT_OK(result)) {
            // return fail even if there are parsed data, as it even
            // does not know how to do next
            return TT_FAIL;
        }
        TT_ASSERT((len != 0) && (len <= TT_BUF_RLEN(data)));
        // now len indicates how many bytes in raw would be decoded

        result = p_itf->parse(data, len, &parse_ret, p_param);
        if (TT_OK(result)) {
            p_itf->done(parse_ret, p_param);
        } else {
            // ignore len bytes in data
            tt_buf_restore_rwp(data, &rp, &wp);
            tt_buf_inc_rp(data, len);

            if (result != TT_PROCEEDING) {
                return TT_FAIL;
            }
        }
        has_parse = TT_TRUE;
    }

    if (has_parse) {
        tt_buf_try_refine(data, rbuf->refine_threshold);
        return TT_SUCCESS;
    } else {
        return TT_BUFFER_INCOMPLETE;
    }
}
