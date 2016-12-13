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

#include <xml/parse/tt_xmlparse_state.h>

#include <xml/parse/tt_xmlparse_state_attr.h>
#include <xml/parse/tt_xmlparse_state_cdata.h>
#include <xml/parse/tt_xmlparse_state_comment.h>
#include <xml/parse/tt_xmlparse_state_doctype.h>
#include <xml/parse/tt_xmlparse_state_etag.h>
#include <xml/parse/tt_xmlparse_state_exclamation.h>
#include <xml/parse/tt_xmlparse_state_pi.h>
#include <xml/parse/tt_xmlparse_state_stag.h>
#include <xml/parse/tt_xmlparse_state_stagend.h>
#include <xml/parse/tt_xmlparse_state_tag.h>
#include <xml/parse/tt_xmlparse_state_text.h>
#include <xml/parse/tt_xmlparse_state_waitattr.h>

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

static tt_xmlpsst_itf_t tt_g_xmlpsst_init_itf = {
    NULL, NULL, NULL,
};

tt_xmlpsst_itf_t *tt_g_xmlpsst_itf[TT_XMLPARSE_STATE_NUM] = {
    &tt_g_xmlpsst_init_itf,
    &tt_g_xmlpsst_text_itf,
    &tt_g_xmlpsst_tag_itf,
    &tt_g_xmlpsst_stag_itf,
    &tt_g_xmlpsst_stagend_itf,
    &tt_g_xmlpsst_etag_itf,
    &tt_g_xmlpsst_waitattr_itf,
    &tt_g_xmlpsst_attr_itf,
    &tt_g_xmlpsst_pi_itf,
    &tt_g_xmlpsst_exclm_itf,
    &tt_g_xmlpsst_cdata_itf,
    &tt_g_xmlpsst_comment_itf,
    &tt_g_xmlpsst_doctype_itf,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////
