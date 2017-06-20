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
@file tt_xml_node.h
@brief xml node

this file specifies xml node APIs
*/

#ifndef __TT_XML_NODE__
#define __TT_XML_NODE__

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

struct tt_xdoc_s;

typedef tt_ptr_t tt_xnode_t;

typedef enum {
    TT_XNODE_NULL,
    TT_XNODE_DOCUMENT,
    TT_XNODE_ELEMENT,
    TT_XNODE_TEXT,
    TT_XNODE_CDATA,
    TT_XNODE_COMMENT,
    TT_XNODE_PI,
    TT_XNODE_DECLARATION,
    TT_XNODE_DOCTYPE,

    TT_XNODE_NUM
} tt_xnode_type_t;
#define TT_XNODE_VALID(v) ((v) < TT_XNODE_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_xnode_component_register();

extern tt_xnode_t tt_xdoc_root(IN struct tt_xdoc_s *xd);

extern tt_bool_t tt_xnode_null(IN tt_xnode_t xn);

extern tt_xnode_type_t tt_xnode_type(IN tt_xnode_t xn);

extern const tt_char_t *tt_xnode_name(IN tt_xnode_t xn);

extern const tt_char_t *tt_xnode_value_cstr(IN tt_xnode_t xn);

extern tt_xnode_t tt_xnode_first_child(IN tt_xnode_t xn);

extern tt_xnode_t tt_xnode_last_child(IN tt_xnode_t xn);

extern tt_xnode_t tt_xnode_next_sibling(IN tt_xnode_t xn);

extern tt_xnode_t tt_xnode_prev_sibling(IN tt_xnode_t xn);

extern tt_xnode_t tt_xnode_parent(IN tt_xnode_t xn);

extern tt_xnode_t tt_xnode_root(IN tt_xnode_t xn);

#endif /* __TT_XML_NODE__ */
