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

#include <xml/parse/tt_xmlparse_state_pi.h>

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

static void __xpsst_pi_enter(IN struct tt_xmlparser_s *parser);

static tt_result_t __xpsst_pi_update(IN struct tt_xmlparser_s *parser,
                                     IN OUT tt_u8_t **data,
                                     IN OUT tt_u32_t *data_len,
                                     OUT tt_xmlparse_state_t *next_state);

tt_xmlpsst_itf_t tt_g_xmlpsst_pi_itf = {
    __xpsst_pi_enter, NULL, __xpsst_pi_update,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void __xpsst_pi_enter(IN struct tt_xmlparser_s *parser)
{
    tt_xmlpsst_pi_t *pi = &parser->state_u.pi;

    pi->ending = TT_FALSE;

    tt_buf_reset_rwp(&parser->common_buf);
}

tt_result_t __xpsst_pi_update(IN struct tt_xmlparser_s *parser,
                              IN OUT tt_u8_t **data,
                              IN OUT tt_u32_t *data_len,
                              OUT tt_xmlparse_state_t *next_state)
{
    tt_u8_t *start, *end, *pos;

    tt_bool_t pi_complete = TT_FALSE;
    tt_buf_t *cbuf = &parser->common_buf;
    tt_xmlpsst_pi_t *pi = &parser->state_u.pi;

    start = *data;
    end = start + *data_len;
    pos = start;
    while (pos < end) {
        tt_u8_t b = *pos++;

        if (pi->ending) {
            if (b != '>') {
                TT_ERROR("PI must be terminated with ?>");
                return TT_FAIL;
            }

            pi_complete = TT_TRUE;
            *next_state = TT_XMLPARSE_STATE_TEXT;
            break;
        } else {
            if (b == '?') {
                pi->ending = TT_TRUE;
            }
        }
    }
    TT_DO(tt_buf_put(cbuf, start, (tt_u32_t)(pos - start)));

    if (pi_complete) {
        tt_u8_t *p, *end, *name, *value;
        tt_u32_t name_len, value_len;
        tt_xp_on_name_value_t on_pi;

        // parse target name and value
        p = TT_BUF_RPOS(cbuf);
        TT_ASSERT(TT_BUF_RLEN(cbuf) >= 2);
        end = p + TT_BUF_RLEN(cbuf) - 2;
        TT_ASSERT((end[0] == '?') && (end[1] == '>'));

        // target is immeidately following <?:
        // PI ::= '<?' PITarget (S ...
        name = p;
        while (p < end) {
            tt_u8_t b = *p;
            if (tt_isspace(b)) {
                break;
            }
            ++p;
        }
        name_len = (tt_u32_t)(p - name);
        if (name_len == 0) {
            TT_ERROR("empty pi name");
            return TT_FAIL;
        }

        while (p < end) {
            tt_u8_t b = *p;
            if (!tt_isspace(b)) {
                break;
            }
            ++p;
        }
        while (end > p) {
            tt_u8_t b = *(end - 1);
            if (!tt_isspace(b)) {
                break;
            }
            --end;
        }
        if (p < end) {
            value = p;
            value_len = (tt_u32_t)(end - p);
        } else {
            value = NULL;
            value_len = 0;
        }

        // notify
        on_pi = parser->cb.on_pi;
        if ((on_pi != NULL) &&
            !TT_OK(on_pi(parser, name, name_len, value, value_len))) {
            return TT_FAIL;
        }
    }

    *data = pos;
    *data_len = (tt_u32_t)(end - pos);
    return TT_SUCCESS;
}
