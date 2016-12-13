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

#include <xml/parse/tt_xmlparse_state_attr.h>

#include <algorithm/tt_buffer_format.h>
#include <misc/tt_assert.h>
#include <xml/parse/tt_xml_parser.h>
#include <xml/tt_xml_def.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal element
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __xpsst_attr_enter(IN struct tt_xmlparser_s *parser);

static tt_result_t __xpsst_attr_update(IN struct tt_xmlparser_s *parser,
                                       IN OUT tt_u8_t **data,
                                       IN OUT tt_u32_t *data_len,
                                       OUT tt_xmlparse_state_t *next_state);

tt_xmlpsst_itf_t tt_g_xmlpsst_attr_itf = {
    __xpsst_attr_enter,
    NULL,

    __xpsst_attr_update,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void __xpsst_attr_enter(IN struct tt_xmlparser_s *parser)
{
    tt_xmlpsst_attr_t *attr = &parser->state_u.attr;

    attr->quote_num = -1;
    attr->single_quotes = TT_FALSE;
    attr->prev_equal = TT_FALSE;

    tt_buf_reset_rwp(&parser->common_buf);
}

tt_result_t __xpsst_attr_update(IN struct tt_xmlparser_s *parser,
                                IN OUT tt_u8_t **data,
                                IN OUT tt_u32_t *data_len,
                                OUT tt_xmlparse_state_t *next_state)
{
    tt_u8_t *start, *end, *pos;

    tt_bool_t attr_complete = TT_FALSE;
    tt_buf_t *cbuf = &parser->common_buf;
    tt_xmlpsst_attr_t *attr = &parser->state_u.attr;

    start = *data;
    end = start + *data_len;
    pos = start;
    while (pos < end) {
        tt_u8_t b = *pos++;

        if (attr->quote_num == -1) {
            // -1 means before '='
            if (b == '=') {
                ++attr->quote_num;
            } else if ((b == '>') || (b == '/')) {
                TT_ERROR("no attribute value");
                return TT_FAIL;
            }
        } else if (attr->quote_num == 0) {
            if (b == '\"') {
                ++attr->quote_num;
                attr->single_quotes = TT_FALSE;
            } else if (b == '\'') {
                ++attr->quote_num;
                attr->single_quotes = TT_TRUE;
            } else if (!tt_isspace(b)) {
                TT_ERROR("must be \' or \" or sp after =, but[%c]", b);
                return TT_FAIL;
            }
        } else {
            if (attr->single_quotes) {
                if (b == '\'') {
                    ++attr->quote_num;

                    attr_complete = TT_TRUE;
                    *next_state = TT_XMLPARSE_STATE_WAITATTR;
                    break;
                }
            } else {
                if (b == '\"') {
                    ++attr->quote_num;

                    attr_complete = TT_TRUE;
                    *next_state = TT_XMLPARSE_STATE_WAITATTR;
                    break;
                }
            }
        }
    }
    TT_ASSERT(attr->quote_num <= 2);
    TT_DO(tt_buf_put(cbuf, start, (tt_u32_t)(pos - start)));

    if (attr_complete) {
        tt_u8_t *p, *name, *value;
        tt_u32_t name_len, value_len;
        tt_xp_on_name_value_t on_attr;

        // parse attribute name and value
        TT_DO(tt_buf_put_u8(cbuf, 0));
        p = TT_BUF_RPOS(cbuf);

        name = p;
        p = (tt_u8_t *)tt_strchr((tt_char_t *)p, '=');
        TT_ASSERT(p != NULL);
        while (p > name) {
            tt_u8_t b = *(p - 1);
            if (!tt_isspace(b)) {
                break;
            }
            --p;
        }
        name_len = (tt_u32_t)(p - name);

        if (attr->single_quotes) {
            value = (tt_u8_t *)tt_strchr((tt_char_t *)p, '\'');
        } else {
            value = (tt_u8_t *)tt_strchr((tt_char_t *)p, '\"');
        }
        TT_ASSERT(value != NULL);
        ++value;
        value_len = (tt_u32_t)tt_strlen((tt_char_t *)value);
        TT_ASSERT(value_len > 0);
        --value_len;

        // notify
        on_attr = parser->cb.on_attr;
        if ((on_attr != NULL) &&
            !TT_OK(on_attr(parser, name, name_len, value, value_len))) {
            return TT_FAIL;
        }
    }

    *data = pos;
    *data_len = (tt_u32_t)(end - pos);
    return TT_SUCCESS;
}
