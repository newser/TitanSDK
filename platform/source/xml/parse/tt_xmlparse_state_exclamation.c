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

#include <xml/parse/tt_xmlparse_state_exclamation.h>

#include <algorithm/tt_buffer_format.h>
#include <misc/tt_assert.h>
#include <xml/parse/tt_xml_parser.h>
#include <xml/tt_xml_def.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __EXCLM_COMMENT "--"
#define __EXCLM_CDATA "[CDATA["
#define __EXCLM_DOCTYPE "DOCTYPE"

#define __EXCLM_MAX_LEN (sizeof(__EXCLM_CDATA) - 1)

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __xpsst_exclm_enter(IN struct tt_xmlparser_s *parser);

static tt_result_t __xpsst_exclm_update(IN struct tt_xmlparser_s *parser,
                                        IN OUT tt_u8_t **data,
                                        IN OUT tt_u32_t *data_len,
                                        OUT tt_xmlparse_state_t *next_state);

tt_xmlpsst_itf_t tt_g_xmlpsst_exclm_itf = {
    __xpsst_exclm_enter, NULL, __xpsst_exclm_update,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void __xpsst_exclm_enter(IN struct tt_xmlparser_s *parser)
{
    tt_xmlpsst_exclm_t *exclm = &parser->state_u.exclm;

    exclm->not_cdata = TT_FALSE;
    exclm->not_comment = TT_FALSE;
    exclm->not_doctype = TT_FALSE;

    tt_buf_reset_rwp(&parser->common_buf);
}

tt_result_t __xpsst_exclm_update(IN struct tt_xmlparser_s *parser,
                                 IN OUT tt_u8_t **data,
                                 IN OUT tt_u32_t *data_len,
                                 OUT tt_xmlparse_state_t *next_state)
{
    tt_u8_t *start, *end, *pos, *pos_end;

    tt_bool_t exclm_complete = TT_FALSE;
    tt_buf_t *cbuf = &parser->common_buf;
    tt_xmlpsst_exclm_t *exclm = &parser->state_u.exclm;

    start = *data;
    end = start + *data_len;
    pos = start;
    TT_ASSERT(TT_BUF_RLEN(cbuf) < __EXCLM_MAX_LEN);
    pos_end = pos + (__EXCLM_MAX_LEN - TT_BUF_RLEN(cbuf));
    if (pos_end > end) {
        pos_end = end;
    }
    while (pos < pos_end) {
        tt_u32_t cbuf_len;

        TT_DO(tt_buf_put_u8(cbuf, *pos));
        ++pos;

        cbuf_len = TT_BUF_RLEN(cbuf);
        if ((cbuf_len >= (sizeof(__EXCLM_COMMENT) - 1)) &&
            !exclm->not_comment) {
            if (tt_strncmp((tt_char_t *)TT_BUF_RPOS(cbuf),
                           __EXCLM_COMMENT,
                           sizeof(__EXCLM_COMMENT) - 1) == 0) {
                exclm_complete = TT_TRUE;
                *next_state = TT_XMLPARSE_STATE_COMMENT;
                break;
            }
            exclm->not_comment = TT_TRUE;
        }
        if ((cbuf_len >= (sizeof(__EXCLM_CDATA) - 1)) && !exclm->not_cdata) {
            if (tt_strncmp((tt_char_t *)TT_BUF_RPOS(cbuf),
                           __EXCLM_CDATA,
                           sizeof(__EXCLM_CDATA) - 1) == 0) {
                exclm_complete = TT_TRUE;
                *next_state = TT_XMLPARSE_STATE_CDATA;
                break;
            }
            exclm->not_cdata = TT_TRUE;
        }
        if ((cbuf_len >= (sizeof(__EXCLM_DOCTYPE) - 1)) &&
            !exclm->not_doctype) {
            if (tt_strncmp((tt_char_t *)TT_BUF_RPOS(cbuf),
                           __EXCLM_DOCTYPE,
                           sizeof(__EXCLM_DOCTYPE) - 1) == 0) {
                exclm_complete = TT_TRUE;
                *next_state = TT_XMLPARSE_STATE_DOCTYPE;
                break;
            }
            exclm->not_doctype = TT_TRUE;
        }
    }
    if ((TT_BUF_RLEN(cbuf) >= __EXCLM_MAX_LEN) && !exclm_complete) {
        TT_ERROR("invalid <!..");
        return TT_FAIL;
    }

    *data = pos;
    *data_len = (tt_u32_t)(end - pos);
    return TT_SUCCESS;
}
