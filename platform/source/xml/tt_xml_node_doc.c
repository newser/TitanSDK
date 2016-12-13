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

#include <xml/tt_xml_node_doc.h>

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

static tt_result_t __xndoc_create(IN struct tt_xnode_s *xn);

static void __xndoc_destroy(IN struct tt_xnode_s *xn);

static tt_list_t *__xndoc_child_list(IN struct tt_xnode_s *xn);

tt_xnode_itf_t tt_g_xnode_doc_itf = {
    __xndoc_create, __xndoc_destroy, __xndoc_child_list, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_xnode_t *tt_xnode_doc_create(IN tt_xmlmem_t *xm)
{
    return tt_xnode_create(xm,
                           sizeof(tt_xnode_doc_t),
                           TT_XNODE_TYPE_DOC,
                           NULL,
                           NULL);
}

tt_result_t __xndoc_create(IN struct tt_xnode_s *xn)
{
    tt_xnode_doc_t *xndoc = TT_XNODE_CAST(xn, tt_xnode_doc_t);

    tt_list_init(&xndoc->child);

    return TT_SUCCESS;
}

void __xndoc_destroy(IN struct tt_xnode_s *xn)
{
    tt_xnode_doc_t *xndoc = TT_XNODE_CAST(xn, tt_xnode_doc_t);
    tt_list_t *child_list = &xndoc->child;
    tt_lnode_t *node;

    while ((node = tt_list_pophead(child_list)) != NULL) {
        tt_xnode_destroy(TT_CONTAINER(node, tt_xnode_t, node));
    }
}

tt_list_t *__xndoc_child_list(IN struct tt_xnode_s *xn)
{
    tt_xnode_doc_t *xndoc = TT_XNODE_CAST(xn, tt_xnode_doc_t);

    return &xndoc->child;
}
