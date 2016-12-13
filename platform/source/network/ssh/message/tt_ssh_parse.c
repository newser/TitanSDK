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

#include <network/ssh/message/tt_ssh_parse.h>

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

tt_result_t tt_ssh_boolean_parse(IN tt_buf_t *buf, OUT tt_bool_t *boolean_val)
{
    tt_u8_t val_u8;

    TT_DO(tt_buf_get_u8(buf, &val_u8));

    *boolean_val = TT_BOOL(val_u8 != 0);

    return TT_SUCCESS;
}

tt_result_t tt_ssh_string_parse(IN tt_buf_t *buf,
                                OUT tt_u8_t **string_val,
                                OUT tt_u32_t *string_len)
{
    tt_u32_t len;

    TT_DO(tt_buf_get_u32_h(buf, &len));
    TT_DO(tt_buf_getptr(buf, string_val, len));
    *string_len = len;

    return TT_SUCCESS;
}

tt_result_t tt_ssh_mpint_parse(IN tt_buf_t *buf,
                               OUT tt_u8_t **mpint,
                               OUT tt_u32_t *mpint_len)
{
    tt_u32_t len;

    TT_DO(tt_buf_get_u32_h(buf, &len));

    *mpint = TT_BUF_RPOS(buf);
    *mpint_len = len;

    TT_DO(tt_buf_inc_rp(buf, len));

    return TT_SUCCESS;
}

tt_result_t tt_ssh_namelist_parse(IN tt_buf_t *buf,
                                  IN tt_ssh_namelist_parse_cb_t cb,
                                  IN void *cb_param)
{
    tt_u32_t len;
    tt_char_t *p, *end;
    tt_char_t *name;

    TT_DO(tt_buf_get_u32_h(buf, &len));
    if (len == 0) {
        return TT_SUCCESS;
    }

    p = (tt_char_t *)TT_BUF_RPOS(buf);
    TT_DO(tt_buf_inc_rp(buf, len));
    end = TT_PTR_INC(tt_char_t, p, len);

    name = p;
    while (p < end) {
        if (*p == ',') {
            if (name == p) {
                TT_ERROR("name can not be 0 length");
                return TT_FAIL;
            }

            if (!TT_OK(cb(name, (tt_u32_t)(p - name), cb_param))) {
                return TT_FAIL;
            }

            name = p + 1;
        }

        ++p;
    }

    TT_ASSERT(name <= p);
    if (name == p) {
        TT_ERROR("namelist can not end with ,");
        return TT_FAIL;
    }
    if (!TT_OK(cb(name, (tt_u32_t)(p - name), cb_param))) {
        return TT_FAIL;
    }


    return TT_SUCCESS;
}
