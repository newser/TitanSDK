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

#define __HAS_CHILD(type)                                                      \
    (((type) == TT_XNODE_TYPE_DOC) || ((type) == TT_XNODE_TYPE_ELEMENT))

#define __HAS_ATTR(type) ((type) == TT_XNODE_TYPE_ELEMENT)

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

static tt_xnode_t *__xn_clone_generic(IN tt_xnode_t *xn);

static tt_xnode_itf_t tt_g_xnode_generic_itf = {
    NULL, NULL, __xn_clone_generic,
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

tt_xnode_t *tt_xnode_create(IN struct tt_xmlmem_s *xm,
                            IN tt_u32_t size,
                            IN tt_xnode_type_t type,
                            IN OPT tt_char_t *name,
                            IN OPT tt_char_t *value)
{
    tt_char_t *new_name = NULL, *new_value = NULL;
    tt_xnode_t *xn;

    if (name != NULL) {
        new_name = tt_xm_copycstr(xm, name);
        if (new_name == NULL) {
            TT_ERROR("no mem for xnode name");
            goto cfail;
        }
    }

    if (value != NULL) {
        new_value = tt_xm_copycstr(xm, value);
        if (new_value == NULL) {
            TT_ERROR("no mem for xnode value");
            goto cfail;
        }
    }

    xn = tt_xnode_create_nocopy(xm, size, type, new_name, new_value);
    if (xn == NULL) {
        goto cfail;
    }

    return xn;

cfail:

    if (new_name != NULL) {
        tt_xm_free(new_name);
    }

    if (new_value != NULL) {
        tt_xm_free(new_value);
    }

    return NULL;
}

tt_xnode_t *tt_xnode_create_nocopy(IN tt_xmlmem_t *xm,
                                   IN tt_u32_t size,
                                   IN tt_xnode_type_t type,
                                   IN OPT TO tt_char_t *name,
                                   IN OPT TO tt_char_t *value)
{
    tt_xnode_t *xn;
    tt_xnode_itf_t *itf;

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
        return NULL;
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

tt_result_t tt_xnode_set_name(IN tt_xnode_t *xn, IN tt_char_t *name)
{
    tt_xnode_type_t t = xn->type;
    tt_char_t *new_name;

    if (!__HAS_NAME(t)) {
        TT_WARN("node[%d] can not have name", t);
        return TT_FAIL;
    }

    new_name = tt_xm_copycstr(tt_xm_xmlmem(xn), name);
    if (new_name == NULL) {
        TT_ERROR("no mem to copy xnode name");
        return TT_FAIL;
    }

    if (xn->name != NULL) {
        tt_xm_free(xn->name);
    }
    xn->name = new_name;

    return TT_SUCCESS;
}

tt_result_t tt_xnode_set_value(IN tt_xnode_t *xn, IN tt_char_t *value)
{
    tt_xnode_type_t t = xn->type;
    tt_char_t *new_val;

    if (!__HAS_VALUE(t)) {
        TT_WARN("node[%d] can not have value", t);
        return TT_FAIL;
    }

    new_val = tt_xm_copycstr(tt_xm_xmlmem(xn), value);
    if (new_val == NULL) {
        TT_ERROR("no mem to copy xnode value");
        return TT_FAIL;
    }

    if (xn->value != NULL) {
        tt_xm_free(xn->value);
    }
    xn->value = new_val;

    return TT_SUCCESS;
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
    TT_ASSERT(__HAS_CHILD(TT_XNODE_OF(parent)->type));
    return TT_XNODE_OF(parent);
}

tt_xnode_t *tt_xnode_first_child(IN tt_xnode_t *xn)
{
    tt_xnode_elmt_t *xelmt;
    tt_lnode_t *lnode;

    if (!__HAS_CHILD(xn->type)) {
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

    if (!__HAS_CHILD(xn->type)) {
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

tt_u32_t tt_xnode_child_num(IN tt_xnode_t *xn)
{
    tt_xnode_elmt_t *xelmt;

    if (!__HAS_CHILD(xn->type)) {
        return 0;
    }
    xelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    return tt_list_count(&xelmt->child);
}

tt_xnode_t *tt_xnode_first_attr(IN tt_xnode_t *xn)
{
    tt_xnode_elmt_t *xelmt;
    tt_lnode_t *lnode;

    if (!__HAS_ATTR(xn->type)) {
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

    if (!__HAS_ATTR(xn->type)) {
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

tt_u32_t tt_xnode_attr_num(IN tt_xnode_t *xn)
{
    tt_xnode_elmt_t *xelmt;

    if (!__HAS_ATTR(xn->type)) {
        return 0;
    }
    xelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    return tt_list_count(&xelmt->attr);
}

tt_result_t tt_xnode_addhead_child(IN tt_xnode_t *xn, IN tt_xnode_t *child)
{
    tt_xnode_elmt_t *xelmt;

    if (!__HAS_CHILD(xn->type)) {
        TT_ERROR("node[%d] does not have child", xn->type);
        return TT_FAIL;
    }
    xelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    tt_list_push_head(&xelmt->child, &child->node);
    return TT_SUCCESS;
}

tt_result_t tt_xnode_addtail_child(IN tt_xnode_t *xn, IN tt_xnode_t *child)
{
    tt_xnode_elmt_t *xelmt;

    if (!__HAS_CHILD(xn->type)) {
        TT_ERROR("node[%d] does not have child", xn->type);
        return TT_FAIL;
    }
    xelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    tt_list_push_tail(&xelmt->child, &child->node);
    return TT_SUCCESS;
}

tt_result_t tt_xnode_addhead_attr(IN tt_xnode_t *xn, IN tt_xnode_t *attr)
{
    tt_xnode_elmt_t *xelmt;

    if (!__HAS_ATTR(xn->type)) {
        TT_ERROR("node[%d] does not have attr", xn->type);
        return TT_FAIL;
    }
    xelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    tt_list_push_head(&xelmt->attr, &attr->node);
    return TT_SUCCESS;
}

tt_result_t tt_xnode_addtail_attr(IN tt_xnode_t *xn, IN tt_xnode_t *attr)
{
    tt_xnode_elmt_t *xelmt;

    if (!__HAS_ATTR(xn->type)) {
        TT_ERROR("node[%d] does not have attr", xn->type);
        return TT_FAIL;
    }
    xelmt = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    tt_list_push_tail(&xelmt->attr, &attr->node);
    return TT_SUCCESS;
}

tt_xnode_t *tt_xnode_child_byname(IN tt_xnode_t *xn, IN const tt_char_t *name)
{
    tt_xnode_t *c = tt_xnode_first_child(xn);
    while (c != NULL) {
        if (tt_strcmp(c->name, name) == 0) {
            return c;
        }
        c = tt_xnode_next(c);
    }
    return NULL;
}

tt_xnode_t *tt_xnode_attr_byname(IN tt_xnode_t *xn, IN const tt_char_t *name)
{
    tt_xnode_t *a = tt_xnode_first_attr(xn);
    while (a != NULL) {
        if (tt_strcmp(a->name, name) == 0) {
            return a;
        }
        a = tt_xnode_next(a);
    }
    return NULL;
}

tt_xnode_t *tt_xnode_next_byname(IN tt_xnode_t *xn, IN const tt_char_t *name)
{
    tt_xnode_t *c = tt_xnode_next(xn);
    while (c != NULL) {
        if (tt_strcmp(c->name, name) == 0) {
            return c;
        }
        c = tt_xnode_next(c);
    }
    return NULL;
}

tt_char_t *tt_xnode_get_text(IN tt_xnode_t *xn)
{
    if (xn->type == TT_XNODE_TYPE_ELEMENT) {
        tt_xnode_t *c = tt_xnode_first_child(xn);
        while (c != NULL) {
            if (c->type == TT_XNODE_TYPE_TEXT) {
                return c->value;
            }
            c = tt_xnode_next(c);
        }
        return NULL;
    } else if (xn->type == TT_XNODE_TYPE_TEXT) {
        return xn->value;
    } else {
        return NULL;
    }
}

tt_result_t tt_xnode_set_text(IN tt_xnode_t *xn, IN tt_char_t *text)
{
    if (xn->type == TT_XNODE_TYPE_ELEMENT) {
        tt_xnode_t *c = tt_xnode_first_child(xn);
        while (c != NULL) {
            if (c->type == TT_XNODE_TYPE_TEXT) {
                break;
            }
            c = tt_xnode_next(c);
        }
        return TT_COND(c != NULL,
                       tt_xnode_set_value(c, text),
                       tt_xnode_add_text(xn, text));
    } else if (xn->type == TT_XNODE_TYPE_ELEMENT) {
        return tt_xnode_set_value(xn, text);
    } else {
        TT_WARN("node[%d] can not have text", xn->type);
        return TT_FAIL;
    }
}

tt_result_t tt_xnode_add_text(IN tt_xnode_t *xn, IN tt_char_t *text)
{
    tt_xmlmem_t *xm = tt_xm_xmlmem(xn);
    tt_char_t *new_text;
    tt_xnode_t *c;

    new_text = tt_xm_copycstr(xm, text);
    if (new_text == NULL) {
        TT_ERROR("no mem for new text");
        return TT_FAIL;
    }

    c = tt_xnode_text_create_nocopy(xm, new_text);
    if (c == NULL) {
        tt_xm_free(new_text);
        return TT_FAIL;
    }
    tt_xnode_addtail_child(xn, c);

    return TT_SUCCESS;
}

const tt_char_t *tt_xnode_get_attrval(IN tt_xnode_t *xn,
                                      IN const tt_char_t *attr_name)
{
    tt_xnode_t *a = tt_xnode_first_attr(xn);
    while (a != NULL) {
        if (tt_strcmp(a->name, attr_name) == 0) {
            return a->value;
        }
        a = tt_xnode_next(a);
    }
    return NULL;
}

tt_result_t tt_xnode_set_attrval(IN tt_xnode_t *xn,
                                 IN const tt_char_t *attr_name,
                                 IN tt_char_t *attr_value)
{
    tt_xnode_t *a = tt_xnode_attr_byname(xn, attr_name);
    if (a != NULL) {
        return tt_xnode_set_value(a, attr_value);
    } else {
        tt_xmlmem_t *xm = tt_xm_xmlmem(xn);
        tt_char_t *new_name, *new_val;

        new_name = tt_xm_copycstr(xm, attr_name);
        if (new_name == NULL) {
            TT_ERROR("no mem for new name");
            return TT_FAIL;
        }

        new_val = tt_xm_copycstr(xm, attr_value);
        if (new_name == NULL) {
            TT_ERROR("no mem for new value");
            tt_xm_free(new_name);
            return TT_FAIL;
        }

        a = tt_xnode_attr_create_nocopy(xm, new_name, new_val);
        if (a == NULL) {
            tt_xm_free(new_name);
            tt_xm_free(new_val);
            return TT_FAIL;
        }
        tt_xnode_addtail_attr(xn, a);

        return TT_SUCCESS;
    }
}

tt_xnode_t *tt_xnode_remove_child(IN tt_xnode_t *xn,
                                  IN const tt_char_t *child_name)
{
    tt_xnode_t *c = tt_xnode_child_byname(xn, child_name);
    if (c != NULL) {
        tt_xnode_remove(c);
    }
    return c;
}

tt_xnode_t *tt_xnode_remove_attr(IN tt_xnode_t *xn,
                                 IN const tt_char_t *attr_name)
{
    tt_xnode_t *a = tt_xnode_attr_byname(xn, attr_name);
    if (a != NULL) {
        tt_xnode_remove(a);
    }
    return a;
}

void tt_xnode_replace(IN tt_xnode_t *xn, IN tt_xnode_t *with_xn)
{
    tt_list_insert_after(&xn->node, &with_xn->node);
    tt_list_remove(&xn->node);
}

tt_xnode_t *tt_xnode_clone(IN tt_xnode_t *xn)
{
    tt_xnode_clone_t xn_clone;

    xn_clone = tt_s_xnode_itf[xn->type]->clone;
    if (xn_clone == NULL) {
        TT_ERROR("can not clone node[%d]", xn->type);
        return NULL;
    }

    return xn_clone(xn);
}

tt_xnode_t *__xn_clone_generic(IN tt_xnode_t *xn)
{
    tt_xmlmem_t *xm = tt_xm_xmlmem(xn);
    tt_char_t *new_name = NULL, *new_val = NULL;
    tt_xnode_t *new_xn = NULL;

    if (xn->name != NULL) {
        new_name = tt_xm_copycstr(xm, xn->name);
        if (new_name == NULL) {
            TT_ERROR("no mem for new name");
            goto cfail;
        }
    }

    if (xn->value != NULL) {
        new_val = tt_xm_copycstr(xm, xn->value);
        if (new_val == NULL) {
            TT_ERROR("no mem for new value");
            goto cfail;
        }
    }

    new_xn = tt_xnode_create_nocopy(xm, 0, xn->type, new_name, new_val);
    if (new_xn == NULL) {
        goto cfail;
    }

    return new_xn;

cfail:

    if (new_xn != NULL) {
        tt_xnode_destroy(new_xn);
    } else {
        if (new_name != NULL) {
            tt_xm_free(new_name);
        }
        if (new_val != NULL) {
            tt_xm_free(new_val);
        }
    }

    return NULL;
}
