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

#include <xml/parse/tt_xmlparse_state_tag.h>

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

static tt_result_t __xpsst_tag_update(IN struct tt_xmlparser_s *parser,
                                      IN OUT tt_u8_t **data,
                                      IN OUT tt_u32_t *data_len,
                                      OUT tt_xmlparse_state_t *next_state);

tt_xmlpsst_itf_t tt_g_xmlpsst_tag_itf = {
    NULL, NULL, __xpsst_tag_update,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t __xpsst_tag_update(IN struct tt_xmlparser_s *parser,
                               IN OUT tt_u8_t **data,
                               IN OUT tt_u32_t *data_len,
                               OUT tt_xmlparse_state_t *next_state)
{
    tt_u8_t *start, *end, *pos;

    start = *data;
    end = start + *data_len;
    pos = start;
    while (pos < end) {
        tt_u8_t b = *pos;

        if (b == '?') {
            ++pos;
            *next_state = TT_XMLPARSE_STATE_PI;
            break;
        } else if (b == '!') {
            ++pos;
            *next_state = TT_XMLPARSE_STATE_EXCLAMATION;
            break;
        } else if (b == '/') {
            ++pos;
            *next_state = TT_XMLPARSE_STATE_ETAG;
            break;
        } else {
            // pos is not changed
            *next_state = TT_XMLPARSE_STATE_STAG;
            break;
        }
    }

    *data = pos;
    *data_len = (tt_u32_t)(end - pos);
    return TT_SUCCESS;
}
