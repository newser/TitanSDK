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

#include <algorithm/tt_io_buffer.h>

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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_iobuf_init(IN tt_iobuf_t *iob,
                   IN tt_iobuf_itf_t *itf,
                   IN OPT void *param,
                   IN OPT tt_iobuf_attr_t *attr)
{
    tt_iobuf_attr_t __attr;

    TT_ASSERT(iob != NULL);
    TT_ASSERT(itf != NULL);
    TT_ASSERT((itf->prepare != NULL) && (itf->transform != NULL));

    if (attr == NULL) {
        tt_iobuf_attr_default(&__attr);
        attr = &__attr;
    }

    iob->itf = itf;
    iob->param = param;
    tt_buf_init(&iob->ibuf, &attr->ibuf_attr);
    tt_buf_init(&iob->obuf, &attr->obuf_attr);
    iob->refine_threshold = attr->refine_threshold;
}

void tt_iobuf_destroy(IN tt_iobuf_t *iob)
{
    TT_ASSERT(iob != NULL);

    tt_buf_destroy(&iob->ibuf);
    tt_buf_destroy(&iob->obuf);
}

void tt_iobuf_attr_default(IN tt_iobuf_attr_t *attr)
{
    tt_buf_attr_default(&attr->ibuf_attr);
    tt_buf_attr_default(&attr->obuf_attr);

    attr->refine_threshold = 1024;
}

tt_result_t tt_iobuf_put(IN tt_iobuf_t *iob, IN tt_u8_t *data, IN tt_u32_t len)
{
    tt_buf_t *ibuf = &iob->ibuf;
    tt_buf_t *obuf = &iob->obuf;
    tt_iobuf_itf_t *itf = iob->itf;

    if (TT_BUF_RLEN(ibuf) != 0) {
        tt_u32_t produced, consumed;

        // if ibuf has data, we should merge them first
        TT_DO(tt_buf_put(ibuf, data, len));

        while (!tt_buf_empty(ibuf) &&
               ((produced = itf->prepare(TT_BUF_RPOS(ibuf),
                                         TT_BUF_RLEN(ibuf),
                                         iob->param)) != 0)) {
            TT_DO(tt_buf_reserve(obuf, produced));

            consumed = itf->transform(TT_BUF_RPOS(ibuf),
                                      TT_BUF_RLEN(ibuf),
                                      TT_BUF_WPOS(obuf),
                                      TT_BUF_WLEN(obuf),
                                      iob->param);
            if (consumed == 0) {
                // prepare has indicated there are enough input data, so failing
                // to transform input is an error
                return TT_FAIL;
            }
            tt_buf_inc_rp(ibuf, consumed);
            tt_buf_inc_wp(obuf, produced);
        }

        tt_buf_try_refine(ibuf, iob->refine_threshold);
        return TT_SUCCESS;
    } else {
        tt_u32_t n = 0, produced, consumed;

        while (
            (n < len) &&
            ((produced = itf->prepare(data + n, len - n, iob->param)) != 0)) {
            TT_DO(tt_buf_reserve(obuf, produced));

            consumed = itf->transform(data + n,
                                      len - n,
                                      TT_BUF_WPOS(obuf),
                                      TT_BUF_WLEN(obuf),
                                      iob->param);
            if (consumed == 0) {
                // prepare has indicated there are enough input data, so failing
                // to transform input is an error
                return TT_FAIL;
            }
            n += consumed;
            TT_ASSERT(n <= len);
            tt_buf_inc_wp(obuf, produced);
        }

        if (n < len) {
            // left data should be cahced in ibuf
            TT_DO(tt_buf_put(ibuf, data + n, len - n));
        }
        return TT_SUCCESS;
    }
}

tt_result_t tt_iobuf_get(IN tt_iobuf_t *iob, IN tt_u8_t *p, IN tt_u32_t len)
{
    tt_buf_t *obuf = &iob->obuf;

    TT_DO(tt_buf_get(obuf, p, len));

    tt_buf_try_refine(obuf, iob->refine_threshold);
    return TT_SUCCESS;
}

void tt_iobuf_clear(IN tt_iobuf_t *iob)
{
    tt_buf_clear(&iob->ibuf);
    tt_buf_clear(&iob->obuf);

    if (iob->itf->clear != NULL) {
        iob->itf->clear(iob->param);
    }
}
