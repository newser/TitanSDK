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

#include <xml/parse/tt_xmlparse_state_text.h>

#include <algorithm/tt_buffer_common.h>
#include <xml/parse/tt_xml_parser.h>
#include <xml/tt_xml_def.h>

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

static void __xpsst_text_enter(IN struct tt_xmlparser_s *parser);

static tt_result_t __xpsst_text_update(IN struct tt_xmlparser_s *parser,
                                       IN OUT tt_u8_t **data,
                                       IN OUT tt_u32_t *data_len,
                                       OUT tt_xmlparse_state_t *next_state);

tt_xmlpsst_itf_t tt_g_xmlpsst_text_itf = {
    __xpsst_text_enter, NULL, __xpsst_text_update,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void __xpsst_text_enter(IN struct tt_xmlparser_s *parser)
{
    tt_buf_reset_rwp(&parser->common_buf);
}

tt_result_t __xpsst_text_update(IN struct tt_xmlparser_s *parser,
                                IN OUT tt_u8_t **data,
                                IN OUT tt_u32_t *data_len,
                                OUT tt_xmlparse_state_t *next_state)
{
    tt_u8_t *start, *end, *pos;

    tt_bool_t text_complete = TT_FALSE;
    tt_buf_t *cbuf = &parser->common_buf;

    start = *data;
    end = start + *data_len;
    pos = start;
    while (pos < end) {
        if (*pos == '<') {
            text_complete = TT_TRUE;
            *next_state = TT_XMLPARSE_STATE_TAG;
            break;
        }

        ++pos;
    }
    TT_DO(tt_buf_put(cbuf, start, (tt_u32_t)(pos - start)));

    if (text_complete) {
        tt_u32_t text_len;
        tt_xp_on_value_t on_text;

        // skip the char that terminates text
        ++pos;

        // notify
        if (parser->attr.trim_text) {
            tt_buf_trim_sp(cbuf);
        }
        text_len = TT_BUF_RLEN(cbuf);

        on_text = parser->cb.on_text;
        if ((text_len != 0) && (on_text != NULL) &&
            !TT_OK(on_text(parser, TT_BUF_RPOS(cbuf), text_len))) {
            return TT_FAIL;
        }
    }

    *data = pos;
    *data_len = (tt_u32_t)(end - pos);
    return TT_SUCCESS;
}
