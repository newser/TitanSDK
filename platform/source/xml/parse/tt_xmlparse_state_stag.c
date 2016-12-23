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

#include <xml/parse/tt_xmlparse_state_stag.h>

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

static void __xpsst_stag_enter(IN struct tt_xmlparser_s *parser);

static tt_result_t __xpsst_stag_update(IN struct tt_xmlparser_s *parser,
                                       IN OUT tt_u8_t **data,
                                       IN OUT tt_u32_t *data_len,
                                       OUT tt_xmlparse_state_t *next_state);

tt_xmlpsst_itf_t tt_g_xmlpsst_stag_itf = {
    __xpsst_stag_enter, NULL, __xpsst_stag_update,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void __xpsst_stag_enter(IN struct tt_xmlparser_s *parser)
{
    tt_buf_reset_rwp(&parser->common_buf);
}

tt_result_t __xpsst_stag_update(IN struct tt_xmlparser_s *parser,
                                IN OUT tt_u8_t **data,
                                IN OUT tt_u32_t *data_len,
                                OUT tt_xmlparse_state_t *next_state)
{
    tt_u8_t *start, *end, *pos;

    tt_bool_t stag_complete = TT_FALSE, complete_cb = TT_FALSE;
    tt_buf_t *cbuf = &parser->common_buf;

    start = *data;
    end = start + *data_len;
    pos = start;
    while (pos < end) {
        tt_u8_t b = *pos;

        if (tt_isspace(b)) {
            stag_complete = TT_TRUE;
            *next_state = TT_XMLPARSE_STATE_WAITATTR;
            break;
        } else if (b == '>') {
            stag_complete = TT_TRUE;
            complete_cb = TT_TRUE;
            *next_state = TT_XMLPARSE_STATE_TEXT;
            break;
        } else if (b == '/') {
            stag_complete = TT_TRUE;
            *next_state = TT_XMLPARSE_STATE_STAGEND;
            break;
        }

        ++pos;
    }
    TT_DO(tt_buf_put(cbuf, start, (tt_u32_t)(pos - start)));

    if (stag_complete) {
        tt_u32_t name_len = 0;
        tt_xp_on_value_t on_start_tag;
        tt_xp_on_state_t on_start_tag_complete;

        // skip the char that terminates name
        ++pos;

        // notify
        name_len = TT_BUF_RLEN(cbuf);
        if (name_len == 0) {
            TT_ERROR("empty xml node name");
            return TT_FAIL;
        }

        on_start_tag = parser->cb.on_start_tag;
        if ((on_start_tag != NULL) &&
            !TT_OK(on_start_tag(parser, TT_BUF_RPOS(cbuf), name_len))) {
            return TT_FAIL;
        }

        on_start_tag_complete = parser->cb.on_start_tag_complete;
        if (complete_cb && (on_start_tag_complete != NULL) &&
            !TT_OK(on_start_tag_complete(parser))) {
            return TT_FAIL;
        }
    }

    *data = pos;
    *data_len = (tt_u32_t)(end - pos);
    return TT_SUCCESS;
}
