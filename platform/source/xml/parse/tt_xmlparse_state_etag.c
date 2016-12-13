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

#include <xml/parse/tt_xmlparse_state_etag.h>

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

static void __xpsst_etag_enter(IN struct tt_xmlparser_s *parser);

static tt_result_t __xpsst_etag_update(IN struct tt_xmlparser_s *parser,
                                       IN OUT tt_u8_t **data,
                                       IN OUT tt_u32_t *data_len,
                                       OUT tt_xmlparse_state_t *next_state);

tt_xmlpsst_itf_t tt_g_xmlpsst_etag_itf = {
    __xpsst_etag_enter, NULL, __xpsst_etag_update,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void __xpsst_etag_enter(IN struct tt_xmlparser_s *parser)
{
    tt_xmlpsst_etag_t *etag = &parser->state_u.etag;

    etag->ending = TT_FALSE;

    tt_buf_reset_rwp(&parser->common_buf);
}

tt_result_t __xpsst_etag_update(IN struct tt_xmlparser_s *parser,
                                IN OUT tt_u8_t **data,
                                IN OUT tt_u32_t *data_len,
                                OUT tt_xmlparse_state_t *next_state)
{
    tt_u8_t *start, *end, *pos;

    tt_bool_t etag_complete = TT_FALSE;
    tt_buf_t *cbuf = &parser->common_buf;
    tt_xmlpsst_etag_t *etag = &parser->state_u.etag;

    start = *data;
    end = start + *data_len;
    pos = start;
    while (pos < end) {
        tt_u8_t b = *pos;

        if (tt_isspace(b)) {
            if (!etag->ending) {
                etag->ending = TT_TRUE;
            }

            ++pos;
            continue;
        } else if (b == '>') {
            etag_complete = TT_TRUE;
            *next_state = TT_XMLPARSE_STATE_TEXT;
            break;
        } else if (!etag->ending) {
            ++pos;
            continue;
        } else {
            // </name xx> is invalid
            TT_ERROR("end tag can only have name");
            return TT_FAIL;
        }
    }
    TT_DO(tt_buf_put(cbuf, start, (tt_u32_t)(pos - start)));

    if (etag_complete != 0) {
        tt_u8_t *name;
        tt_u32_t name_len = 0;
        tt_xp_on_value_t on_end_tag;

        // skip the char that terminates name
        ++pos;

        // notify
        name = TT_BUF_RPOS(cbuf);
        name_len = TT_BUF_RLEN(cbuf);
        while ((name_len > 0) && (name[name_len - 1] == ' ')) {
            --name_len;
        }
        if (name_len == 0) {
            TT_ERROR("empty xml node name");
            return TT_FAIL;
        }

        on_end_tag = parser->cb.on_end_tag;
        if ((on_end_tag != NULL) &&
            !TT_OK(on_end_tag(parser, name, name_len))) {
            return TT_FAIL;
        }
    }

    *data = pos;
    *data_len = (tt_u32_t)(end - pos);
    return TT_SUCCESS;
}
