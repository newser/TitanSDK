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

/**
@file tt_xmlparse_state.h
@brief xml parse state

this file defines xml parser state
*/

#ifndef __TT_XMLPARSE_STATE__
#define __TT_XMLPARSE_STATE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_xmlparser_s;

typedef enum {
    // guess encoding
    TT_XMLPARSE_STATE_INIT,

    // found content
    TT_XMLPARSE_STATE_TEXT,
    // found "<"
    TT_XMLPARSE_STATE_TAG,
    // found start tag
    TT_XMLPARSE_STATE_STAG,
    // found "/>"
    TT_XMLPARSE_STATE_STAGEND,
    // found end tag: "</"
    TT_XMLPARSE_STATE_ETAG,
    // wait for attribute
    TT_XMLPARSE_STATE_WAITATTR,
    // found attribute
    TT_XMLPARSE_STATE_ATTR,
    // found "<?"
    TT_XMLPARSE_STATE_PI,
    // found "<!"
    TT_XMLPARSE_STATE_EXCLAMATION,
    // found "<![CDATA["
    TT_XMLPARSE_STATE_CDATA,
    // found "<!--"
    TT_XMLPARSE_STATE_COMMENT,
    // found "<!DOCTYPE"
    TT_XMLPARSE_STATE_DOCTYPE,

    TT_XMLPARSE_STATE_COMPLETE,
    TT_XMLPARSE_STATE_ERROR,

    TT_XMLPARSE_STATE_NUM
} tt_xmlparse_state_t;
#define TT_XMLPARSE_STATE_VALID(s) ((s) < TT_XMLPARSE_STATE_NUM)

typedef void (*tt_xmlpsst_enter_t)(IN struct tt_xmlparser_s *parser);

typedef void (*tt_xmlpsst_exit_t)(IN struct tt_xmlparser_s *parser);

// this api should check the inputed @ref data as well as the data stored
// in parser->common_buf, if data are enough to switch to new state, do
// corresponding operations and update out parameter, otherwise, @ref
// data should be stored in parser->common_buf and then update out params.
// note parser->common_buf is managed by state itself, the state may cache
// data in the buf and the state must guarantee the buf in a consistent
// status when switching to new state
typedef tt_result_t (*tt_xmlpsst_update_t)(IN struct tt_xmlparser_s *parser,
                                           IN OUT tt_u8_t **data,
                                           IN OUT tt_u32_t *data_len,
                                           OUT tt_xmlparse_state_t *next_state);

typedef struct
{
    tt_xmlpsst_enter_t enter;
    tt_xmlpsst_exit_t exit;
    tt_xmlpsst_update_t update;
} tt_xmlpsst_itf_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern tt_xmlpsst_itf_t *tt_g_xmlpsst_itf[TT_XMLPARSE_STATE_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_XMLPARSE_STATE__ */
