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

extern "C" {
#include <xml/tt_xml_node.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <misc/tt_util.h>
#include <xml/tt_xml_document.h>
#include <xml/tt_xml_path.h>
#include <xml/tt_xml_util.h>
}

#include <pugixml.hpp>

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

static tt_xnode_type_t __type_p2t[pugi::node_doctype + 1] = {
    TT_XNODE_NULL,
    TT_XNODE_DOCUMENT,
    TT_XNODE_ELEMENT,
    TT_XNODE_TEXT,
    TT_XNODE_CDATA,
    TT_XNODE_COMMENT,
    TT_XNODE_PI,
    TT_XNODE_DECLARATION,
    TT_XNODE_DOCTYPE,
};

static pugi::xml_node_type __type_t2p[TT_XNODE_TYPE_NUM] = {
    pugi::node_null,
    pugi::node_document,
    pugi::node_element,
    pugi::node_pcdata,
    pugi::node_cdata,
    pugi::node_comment,
    pugi::node_pi,
    pugi::node_declaration,
    pugi::node_doctype,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __xnode_component_init(IN tt_component_t *comp,
                                          IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_xnode_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __xnode_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_XML_NODE, "XML Node", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_xnode_t tt_xdoc_root(IN tt_xdoc_t *xd)
{
    pugi::xml_document *pd = static_cast<pugi::xml_document *>(xd->p);
    TT_ASSERT(pd != NULL);
    return *reinterpret_cast<tt_xnode_t *>(static_cast<pugi::xml_node *>(pd));
}

tt_xnode_type_t tt_xnode_type(IN tt_xnode_t xn)
{
    return __type_p2t[PN(xn).type()];
}

const tt_char_t *tt_xnode_get_name(IN tt_xnode_t xn)
{
    return PN(xn).name();
}

tt_result_t tt_xnode_set_name(IN tt_xnode_t xn, IN const tt_char_t *name)
{
    if (PN(xn).set_name(name)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_xnode_t tt_xnode_first_child(IN tt_xnode_t xn)
{
    pugi::xml_node pn = PN(xn).first_child();
    return TN(pn);
}

tt_xnode_t tt_xnode_last_child(IN tt_xnode_t xn)
{
    pugi::xml_node pn = PN(xn).last_child();
    return TN(pn);
}

tt_xnode_t tt_xnode_child_byname(IN tt_xnode_t xn, IN const tt_char_t *name)
{
    pugi::xml_node pn = PN(xn).child(name);
    return TN(pn);
}

tt_xnode_t tt_xnode_append_child(IN tt_xnode_t xn, IN tt_xnode_type_t type)
{
    pugi::xml_node pn = PN(xn).append_child(__type_t2p[type]);
    return TN(pn);
}

tt_xnode_t tt_xnode_prepend_child(IN tt_xnode_t xn, IN tt_xnode_type_t type)
{
    pugi::xml_node pn = PN(xn).prepend_child(__type_t2p[type]);
    return TN(pn);
}

tt_xnode_t tt_xnode_insert_child_after(IN tt_xnode_t xn,
                                       IN tt_xnode_t c,
                                       IN tt_xnode_type_t type)
{
    pugi::xml_node pn = PN(xn).insert_child_after(__type_t2p[type], PN(c));
    return TN(pn);
}

tt_xnode_t tt_xnode_insert_child_before(IN tt_xnode_t xn,
                                        IN tt_xnode_t c,
                                        IN tt_xnode_type_t type)
{
    pugi::xml_node pn = PN(xn).insert_child_before(__type_t2p[type], PN(c));
    return TN(pn);
}

tt_result_t tt_xnode_remove_child(IN tt_xnode_t xn, IN tt_xnode_t c)
{
    if (PN(xn).remove_child(PN(c))) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_xnode_remove_child_byname(IN tt_xnode_t xn,
                                         IN const tt_char_t *name)
{
    if (PN(xn).remove_child(name)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_xnode_t tt_xnode_next_sibling(IN tt_xnode_t xn)
{
    pugi::xml_node pn = PN(xn).next_sibling();
    return TN(pn);
}

tt_xnode_t tt_xnode_prev_sibling(IN tt_xnode_t xn)
{
    pugi::xml_node pn = PN(xn).previous_sibling();
    return TN(pn);
}

tt_xnode_t tt_xnode_parent(IN tt_xnode_t xn)
{
    pugi::xml_node pn = PN(xn).parent();
    return TN(pn);
}

tt_xnode_t tt_xnode_root(IN tt_xnode_t xn)
{
    pugi::xml_node pn = PN(xn).root();
    return TN(pn);
}

// ========================================
// xml attribute
// ========================================

tt_xattr_t tt_xnode_first_attr(IN tt_xnode_t xn)
{
    pugi::xml_attribute xa = PN(xn).first_attribute();
    return TA(xa);
}

tt_xattr_t tt_xnode_last_attr(IN tt_xnode_t xn)
{
    pugi::xml_attribute xa = PN(xn).last_attribute();
    return TA(xa);
}

tt_xattr_t tt_xnode_attr_byname(IN tt_xnode_t xn, IN const tt_char_t *name)
{
    pugi::xml_attribute xa = PN(xn).attribute(name);
    return TA(xa);
}

tt_xattr_t tt_xnode_append_attr(IN tt_xnode_t xn, IN const tt_char_t *name)
{
    pugi::xml_attribute xa = PN(xn).append_attribute(name);
    return TA(xa);
}

tt_xattr_t tt_xnode_prepend_attr(IN tt_xnode_t xn, IN const tt_char_t *name)
{
    pugi::xml_attribute xa = PN(xn).prepend_attribute(name);
    return TA(xa);
}

tt_xattr_t tt_xnode_insert_attr_after(IN tt_xnode_t xn,
                                      IN tt_xattr_t xa,
                                      IN const tt_char_t *name)
{
    pugi::xml_attribute __xa = PN(xn).insert_attribute_after(name, PA(xa));
    return TA(__xa);
}

tt_xattr_t tt_xnode_insert_attr_before(IN tt_xnode_t xn,
                                       IN tt_xattr_t xa,
                                       IN const tt_char_t *name)
{
    pugi::xml_attribute __xa = PN(xn).insert_attribute_before(name, PA(xa));
    return TA(__xa);
}

tt_result_t tt_xnode_remove_attr(IN tt_xnode_t xn, IN tt_xattr_t xa)
{
    if (PN(xn).remove_attribute(PA(xa))) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_xnode_remove_attr_byname(IN tt_xnode_t xn,
                                        IN const tt_char_t *name)
{
    if (PN(xn).remove_attribute(name)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

// ========================================
// xml value
// ========================================

const tt_char_t *tt_xnode_get_value(IN tt_xnode_t xn, IN const tt_char_t *def)
{
    pugi::xml_text text = PN(xn).text();
    return text.as_string(def);
}

tt_result_t tt_xnode_set_value(IN tt_xnode_t xn, IN const tt_char_t *value)
{
    if (PN(xn).set_value(value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_bool_t tt_xnode_get_bool(IN tt_xnode_t xn, IN tt_bool_t def)
{
    if (PN(xn).text().as_bool(def)) {
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t tt_xnode_set_bool(IN tt_xnode_t xn, IN tt_bool_t value)
{
    if (PN(xn).text().set((bool)value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_s32_t tt_xnode_get_s32(IN tt_xnode_t xn, IN tt_s32_t def)
{
    return PN(xn).text().as_int(def);
}

tt_result_t tt_xnode_set_s32(IN tt_xnode_t xn, IN tt_s32_t value)
{
    if (PN(xn).text().set((int)value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_u32_t tt_xnode_get_u32(IN tt_xnode_t xn, IN tt_u32_t def)
{
    return PN(xn).text().as_uint(def);
}

tt_result_t tt_xnode_set_u32(IN tt_xnode_t xn, IN tt_u32_t value)
{
    if (PN(xn).text().set((unsigned int)value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_s64_t tt_xnode_get_s64(IN tt_xnode_t xn, IN tt_s64_t def)
{
    return PN(xn).text().as_llong(def);
}

tt_result_t tt_xnode_set_s64(IN tt_xnode_t xn, IN tt_s64_t value)
{
    if (PN(xn).text().set((long long)value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_u64_t tt_xnode_get_u64(IN tt_xnode_t xn, IN tt_u64_t def)
{
    return PN(xn).text().as_ullong(def);
}

tt_result_t tt_xnode_set_u64(IN tt_xnode_t xn, IN tt_u64_t value)
{
    if (PN(xn).text().set((unsigned long long)value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_float_t tt_xnode_get_float(IN tt_xnode_t xn, IN tt_float_t def)
{
    return PN(xn).text().as_float(def);
}

tt_result_t tt_xnode_set_float(IN tt_xnode_t xn, IN tt_float_t value)
{
    if (PN(xn).text().set(value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_double_t tt_xnode_get_double(IN tt_xnode_t xn, IN tt_double_t def)
{
    return PN(xn).text().as_double(def);
}

tt_result_t tt_xnode_set_double(IN tt_xnode_t xn, IN tt_double_t value)
{
    if (PN(xn).text().set(value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

// ========================================
// xml path
// ========================================

void tt_xnode_select(IN tt_xnode_t xn,
                     IN struct tt_xpath_s *xp,
                     OUT tt_xnode_t *o_xn,
                     OUT tt_xattr_t *o_xa)
{
    pugi::xpath_node p = PN(xn).select_node(*P_XP(xp));

    pugi::xml_node pn = p.node();
    *o_xn = TN(pn);

    pugi::xml_attribute pa = p.attribute();
    *o_xa = TA(pa);
}

void tt_xnode_select_all(IN tt_xnode_t xn,
                         IN tt_xpath_t *xp,
                         OUT tt_xpnodes_t *xpns)
{
    *P_XPNS(xpns) = PN(xn).select_nodes(*P_XP(xp));
}

tt_result_t __xnode_component_init(IN tt_component_t *comp,
                                   IN tt_profile_t *profile)
{
    if (sizeof(tt_xnode_t) < sizeof(class pugi::xml_node)) {
        TT_ERROR("sizeof(tt_xnode_t)[%d] < sizeof(class xml_node)[%d]",
                 sizeof(tt_xnode_t),
                 sizeof(class pugi::xml_node));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
