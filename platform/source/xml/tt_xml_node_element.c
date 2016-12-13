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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either exmress or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <xml/tt_xml_node_element.h>

#include <xml/tt_xml_memory.h>

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

static tt_result_t __xnelmt_create(IN struct tt_xnode_s *xn);

static void __xnelmt_destroy(IN struct tt_xnode_s *xn);

tt_xnode_itf_t tt_g_xnode_elmt_itf = {
    __xnelmt_create, __xnelmt_destroy,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_xnode_t *tt_xnode_elmt_create(IN tt_xmlmem_t *xm, IN OPT TO tt_char_t *name)
{
    return tt_xnode_create(xm,
                           sizeof(tt_xnode_elmt_t),
                           TT_XNODE_TYPE_ELEMENT,
                           name,
                           NULL);
}

tt_result_t __xnelmt_create(IN struct tt_xnode_s *xn)
{
    tt_xnode_elmt_t *xnelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    tt_list_init(&xnelmt->child);
    tt_list_init(&xnelmt->attr);

    return TT_SUCCESS;
}

void __xnelmt_destroy(IN struct tt_xnode_s *xn)
{
    tt_xnode_elmt_t *xnelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);
    tt_list_t *list;
    tt_lnode_t *node;

    list = &xnelmt->child;
    while ((node = tt_list_pophead(list)) != NULL) {
        tt_xnode_destroy(TT_CONTAINER(node, tt_xnode_t, node));
    }

    list = &xnelmt->attr;
    while ((node = tt_list_pophead(list)) != NULL) {
        tt_xnode_destroy(TT_CONTAINER(node, tt_xnode_t, node));
    }
}
