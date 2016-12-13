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

#include <xml/parse/tt_xmlparse_state_cdata.h>

#include <algorithm/tt_buffer_common.h>
#include <misc/tt_assert.h>
#include <xml/parse/tt_xml_parser.h>
#include <xml/tt_xml_def.h>

#include <tt_cstd_api.h>

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

static void __xpsst_cdata_enter(IN struct tt_xmlparser_s *parser);

static tt_result_t __xpsst_cdata_update(IN struct tt_xmlparser_s *parser,
                                        IN OUT tt_u8_t **data,
                                        IN OUT tt_u32_t *data_len,
                                        OUT tt_xmlparse_state_t *next_state);

tt_xmlpsst_itf_t tt_g_xmlpsst_cdata_itf = {
    __xpsst_cdata_enter, NULL, __xpsst_cdata_update,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void __xpsst_cdata_enter(IN struct tt_xmlparser_s *parser)
{
    tt_buf_reset_rwp(&parser->common_buf);
}

tt_result_t __xpsst_cdata_update(IN struct tt_xmlparser_s *parser,
                                 IN OUT tt_u8_t **data,
                                 IN OUT tt_u32_t *data_len,
                                 OUT tt_xmlparse_state_t *next_state)
{
    tt_u8_t *start, *end, *pos;

    tt_bool_t cdata_complete = TT_FALSE;
    tt_buf_t *cbuf = &parser->common_buf;

    start = *data;
    end = start + *data_len;
    pos = start;
    while (pos < end) {
        tt_u32_t b = *pos++;

        if (b == '>') {
            tt_u32_t cbuf_len;

            TT_DO(tt_buf_put(cbuf, start, (tt_u32_t)(pos - start)));
            start = pos;

            cbuf_len = TT_BUF_RLEN(cbuf);
            if ((cbuf_len >= 3) &&
                (tt_strncmp((tt_char_t *)TT_BUF_RPOS(cbuf) + cbuf_len - 3,
                            "]]>",
                            3) == 0)) {
                cdata_complete = TT_TRUE;
                *next_state = TT_XMLPARSE_STATE_TEXT;
                break;
            }
        }
    }
    TT_DO(tt_buf_put(cbuf, start, (tt_u32_t)(pos - start)));

    if (cdata_complete) {
        tt_u32_t cdata_len = 0;
        tt_xp_on_value_t on_cdata;

        // remove "]]>"
        TT_ASSERT(TT_BUF_RLEN(cbuf) >= 3);
        tt_buf_dec_wp(cbuf, 3);

        // notify
        if (parser->attr.trim_cdata) {
            tt_buf_trim_sp(cbuf);
        }
        cdata_len = TT_BUF_RLEN(cbuf);

        on_cdata = parser->cb.on_cdata;
        if ((cdata_len != 0) && (on_cdata != NULL) &&
            !TT_OK(on_cdata(parser, TT_BUF_RPOS(cbuf), cdata_len))) {
            return TT_FAIL;
        }
    }

    *data = pos;
    *data_len = (tt_u32_t)(end - pos);
    return TT_SUCCESS;
}
