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
@file tt_xml_node_element.h
@brief xml node: element

this file defines xml node: element
 */

#ifndef __TT_XML_NODE_ELEMENT__
#define __TT_XML_NODE_ELEMENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <xml/tt_xml_node.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_xnode_elmt_s
{
    tt_list_t child;
    tt_list_t attr;
} tt_xnode_elmt_t;

typedef tt_xnode_elmt_t tt_xnode_doc_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern tt_xnode_itf_t tt_g_xnode_elmt_itf;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_xnode_t *tt_xnode_elmt_create(IN struct tt_xmlmem_s *xm,
                                        IN OPT TO tt_char_t *name);

extern tt_xnode_t *tt_xnode_doc_create(IN struct tt_xmlmem_s *xm);

#endif /* __TT_XML_NODE_ELEMENT__ */
