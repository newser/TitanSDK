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

#include <xml/tt_xml_node.h>

#include <misc/tt_assert.h>
#include <xml/parse/tt_xml_parser.h>
#include <xml/tt_xml_memory.h>
#include <xml/tt_xml_node_element.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __HAS_NAME(type)                                                       \
    (((type) == TT_XNODE_TYPE_ELEMENT) || ((type) == TT_XNODE_TYPE_ATTR) ||    \
     ((type) == TT_XNODE_TYPE_PI))

#define __HAS_VALUE(type)                                                      \
    (((type) != TT_XNODE_TYPE_DOC) && ((type) != TT_XNODE_TYPE_ELEMENT))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_xnode_itf_t tt_g_xnode_generic_itf = {
    NULL, NULL,
};

static tt_xnode_itf_t *tt_s_xnode_itf[TT_XNODE_TYPE_NUM] = {
    &tt_g_xnode_elmt_itf, // TT_XNODE_TYPE_DOC,
    &tt_g_xnode_elmt_itf, // TT_XNODE_TYPE_ELEMENT
    &tt_g_xnode_generic_itf, // TT_XNODE_TYPE_ATTR
    &tt_g_xnode_generic_itf, // TT_XNODE_TYPE_TEXT
    &tt_g_xnode_generic_itf, // TT_XNODE_TYPE_CDATA
    &tt_g_xnode_generic_itf, // TT_XNODE_TYPE_COMMENT
    &tt_g_xnode_generic_itf, // TT_XNODE_TYPE_PI
};

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_xnode_t *tt_xnode_create(IN tt_xmlmem_t *xm,
                            IN tt_u32_t size,
                            IN tt_xnode_type_t type,
                            IN OPT TO tt_char_t *name,
                            IN OPT TO tt_char_t *value)
{
    tt_xnode_t *xn;
    tt_xnode_itf_t *itf;

    TT_ASSERT(xm != NULL);
    TT_ASSERT(TT_XNODE_TYPE_VALID(type));

    xn = (tt_xnode_t *)tt_xm_alloc(xm, sizeof(tt_xnode_t) + size);
    if (xn == NULL) {
        return NULL;
    }

    tt_lnode_init(&xn->node);

    if ((name != NULL) && !__HAS_NAME(type)) {
        TT_WARN("node[%d] can not have name", type);
        xn->name = NULL;
    } else {
        xn->name = name;
    }

    if ((value != NULL) && !__HAS_VALUE(type)) {
        TT_WARN("node[%d] can not have value", type);
        xn->value = NULL;
    } else {
        xn->value = value;
    }

    xn->type = type;
    xn->ns = NULL;

    itf = tt_s_xnode_itf[xn->type];
    if ((itf->create != NULL) && !TT_OK(itf->create(xn))) {
        // can not use tt_xnode_destroy here as creating is not done
        if (xn->name != NULL) {
            tt_xm_free(xn->name);
        }
        if (xn->value != NULL) {
            tt_xm_free(xn->value);
        }
        tt_xm_free(xn);
        return TT_FAIL;
    }

    return xn;
}

void tt_xnode_destroy(IN tt_xnode_t *xn)
{
    tt_xnode_itf_t *itf;

    TT_ASSERT(xn != NULL);

    itf = tt_s_xnode_itf[xn->type];
    if (itf->desetroy != NULL) {
        itf->desetroy(xn);
    }

    if (xn->name != NULL) {
        tt_xm_free(xn->name);
    }
    if (xn->value != NULL) {
        tt_xm_free(xn->value);
    }
    tt_xm_free(xn);
}

void tt_xnode_set_name(IN tt_xnode_t *xn, IN tt_char_t *name)
{
    tt_xnode_type_t t = xn->type;

    if (!__HAS_NAME(t)) {
        TT_WARN("node[%d] can not have name", t);
        return;
    }

    if (xn->name != NULL) {
        tt_xm_free(xn->name);
    }
    xn->name = name;
}

void tt_xnode_set_value(IN tt_xnode_t *xn, IN tt_char_t *value)
{
    tt_xnode_type_t t = xn->type;

    if (!__HAS_VALUE(t)) {
        TT_WARN("node[%d] can not have name", t);
        return;
    }

    if (xn->value != NULL) {
        tt_xm_free(xn->value);
    }
    xn->value = value;
}

tt_xnode_t *tt_xnode_parent(IN tt_xnode_t *xn)
{
    tt_xnode_elmt_t *parent;

    if (xn->node.lst == NULL) {
        return NULL;
    }

    // only element node could have child
    if (xn->type != TT_XNODE_TYPE_ATTR) {
        parent = TT_CONTAINER(xn->node.lst, tt_xnode_elmt_t, child);
    } else {
        parent = TT_CONTAINER(xn->node.lst, tt_xnode_elmt_t, attr);
    }
    TT_ASSERT(TT_XNODE_OF(parent)->type == TT_XNODE_TYPE_ELEMENT);
    return TT_XNODE_OF(parent);
}

tt_xnode_t *tt_xnode_first_child(IN tt_xnode_t *xn)
{
    tt_xnode_elmt_t *xelmt;
    tt_lnode_t *lnode;

    if (xn->type != TT_XNODE_TYPE_ELEMENT) {
        TT_ERROR("node[%d] does not have child list", xn->type);
        return NULL;
    }
    xelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    lnode = tt_list_head(&xelmt->child);
    if (lnode == NULL) {
        return NULL;
    }
    return TT_CONTAINER(lnode, tt_xnode_t, node);
}

tt_xnode_t *tt_xnode_last_child(IN tt_xnode_t *xn)
{
    tt_xnode_elmt_t *xelmt;
    tt_lnode_t *lnode;

    if (xn->type != TT_XNODE_TYPE_ELEMENT) {
        TT_ERROR("node[%d] does not have child list", xn->type);
        return NULL;
    }
    xelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    lnode = tt_list_tail(&xelmt->child);
    if (lnode == NULL) {
        return NULL;
    }
    return TT_CONTAINER(lnode, tt_xnode_t, node);
}

tt_xnode_t *tt_xnode_first_attr(IN tt_xnode_t *xn)
{
    tt_xnode_elmt_t *xelmt;
    tt_lnode_t *lnode;

    if (xn->type != TT_XNODE_TYPE_ELEMENT) {
        TT_ERROR("node[%d] does not have attr list", xn->type);
        return NULL;
    }
    xelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    lnode = tt_list_head(&xelmt->attr);
    if (lnode == NULL) {
        return NULL;
    }
    return TT_CONTAINER(lnode, tt_xnode_t, node);
}

tt_xnode_t *tt_xnode_last_attr(IN tt_xnode_t *xn)
{
    tt_xnode_elmt_t *xelmt;
    tt_lnode_t *lnode;

    if (xn->type != TT_XNODE_TYPE_ELEMENT) {
        TT_ERROR("node[%d] does not have attr list", xn->type);
        return NULL;
    }
    xelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    lnode = tt_list_tail(&xelmt->attr);
    if (lnode == NULL) {
        return NULL;
    }
    return TT_CONTAINER(lnode, tt_xnode_t, node);
}

tt_result_t tt_xnode_add_child(IN tt_xnode_t *xn, IN tt_xnode_t *child)
{
    tt_xnode_elmt_t *xelmt;

    if (xn->type != TT_XNODE_TYPE_ELEMENT) {
        TT_ERROR("node[%d] does not have child list", xn->type);
        return NULL;
    }
    xelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    tt_list_addtail(&xelmt->child, &child->node);
    return TT_SUCCESS;
}

tt_result_t tt_xnode_add_attr(IN tt_xnode_t *xn, IN tt_xnode_t *attr)
{
    tt_xnode_elmt_t *xelmt;

    if (xn->type != TT_XNODE_TYPE_ELEMENT) {
        TT_ERROR("node[%d] does not have attr list", xn->type);
        return NULL;
    }
    xelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    tt_list_addtail(&xelmt->attr, &attr->node);
    return TT_SUCCESS;
}
