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

#include <xml/parse/tt_xmlparse_state_waitattr.h>

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

static tt_result_t __xpsst_waitattr_update(IN struct tt_xmlparser_s *parser,
                                           IN OUT tt_u8_t **data,
                                           IN OUT tt_u32_t *data_len,
                                           OUT tt_xmlparse_state_t *next_state);

tt_xmlpsst_itf_t tt_g_xmlpsst_waitattr_itf = {
    NULL, NULL, __xpsst_waitattr_update,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t __xpsst_waitattr_update(IN struct tt_xmlparser_s *parser,
                                    IN OUT tt_u8_t **data,
                                    IN OUT tt_u32_t *data_len,
                                    OUT tt_xmlparse_state_t *next_state)
{
    tt_u8_t *start, *end, *pos;
    tt_bool_t complete_cb = TT_FALSE;

    start = *data;
    end = start + *data_len;
    pos = start;
    while (pos < end) {
        tt_u8_t b = *pos;

        if (tt_isspace(b)) {
            ++pos;
            continue;
        } else if (b == '>') {
            ++pos;
            complete_cb = TT_TRUE;
            *next_state = TT_XMLPARSE_STATE_TEXT;
            break;
        } else if (b == '/') {
            ++pos;
            *next_state = TT_XMLPARSE_STATE_STAGEND;
            break;
        } else {
            // do not move pos
            *next_state = TT_XMLPARSE_STATE_ATTR;
            break;
        }
    }

    if (complete_cb && (parser->cb.on_start_tag_complete != NULL) &&
        !TT_OK(parser->cb.on_start_tag_complete(parser))) {
        return TT_FAIL;
    }

    *data = pos;
    *data_len = (tt_u32_t)(end - pos);
    return TT_SUCCESS;
}
