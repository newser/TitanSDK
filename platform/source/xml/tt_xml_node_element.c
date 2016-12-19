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

static struct tt_xnode_s *__xnelmt_clone(IN struct tt_xnode_s *xn);

static void __xnelmt_destroy(IN struct tt_xnode_s *xn);

tt_xnode_itf_t tt_g_xnode_elmt_itf = {
    __xnelmt_create, __xnelmt_destroy, __xnelmt_clone,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_xnode_t *tt_xnode_elmt_create(IN struct tt_xmlmem_s *xm,
                                 IN OPT tt_char_t *name)
{
    tt_xnode_t *xn;

    if (name != NULL) {
        name = tt_xm_copycstr(xm, name);
        if (name == NULL) {
            TT_ERROR("no mem for xnode elmt name");
            return NULL;
        }
    }

    xn = tt_xnode_elmt_create_nocopy(xm, name);
    if ((xn == NULL) && (name != NULL)) {
        tt_xm_free(name);
        return NULL;
    }

    return xn;
}

tt_xnode_t *tt_xnode_elmt_create_nocopy(IN tt_xmlmem_t *xm,
                                        IN OPT TO tt_char_t *name)
{
    return tt_xnode_create_nocopy(xm,
                                  sizeof(tt_xnode_elmt_t),
                                  TT_XNODE_TYPE_ELEMENT,
                                  name,
                                  NULL);
}

tt_xnode_t *tt_xnode_doc_create(IN struct tt_xmlmem_s *xm)
{
    return tt_xnode_create_nocopy(xm,
                                  sizeof(tt_xnode_elmt_t),
                                  TT_XNODE_TYPE_DOC,
                                  NULL,
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

struct tt_xnode_s *__xnelmt_clone(IN struct tt_xnode_s *xn)
{
    tt_xmlmem_t *xm;
    tt_char_t *name = NULL;
    tt_xnode_t *new_xn, *tmp, *cloned;

    xm = tt_xm_xmlmem(xn);

    // doc node has null name
    if (xn->name != NULL) {
        name = tt_xm_copycstr(xm, xn->name);
        if (name == NULL) {
            TT_ERROR("fail to clone name");
            return NULL;
        }
    }

    new_xn = tt_xnode_elmt_create_nocopy(xm, xn->name);
    if (new_xn == NULL) {
        TT_ERROR("fail to clone element node");
        if (name != NULL) {
            tt_xm_free(name);
        }
        return NULL;
    }

    // this func would also be used by doc node, but doc
    // node does not have attribute nodes
    if (xn->type != TT_XNODE_TYPE_DOC) {
        tmp = tt_xnode_first_attr(xn);
        while (tmp != NULL) {
            cloned = tt_xnode_clone(tmp);
            if (cloned == NULL) {
                TT_ERROR("fail to clone attr");
                tt_xnode_destroy(new_xn);
                return NULL;
            }
            tt_xnode_addtail_attr(new_xn, cloned);

            tmp = tt_xnode_next(tmp);
        }
    }

    tmp = tt_xnode_first_child(xn);
    while (tmp != NULL) {
        cloned = tt_xnode_clone(tmp);
        if (cloned == NULL) {
            TT_ERROR("fail to clone child");
            tt_xnode_destroy(new_xn);
            return NULL;
        }
        tt_xnode_addtail_child(new_xn, cloned);

        tmp = tt_xnode_next(tmp);
    }

    return new_xn;
}
