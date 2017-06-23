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
#include <xml/tt_xml_path.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <xml/tt_xml_attribute.h>
#include <xml/tt_xml_node.h>
#include <xml/tt_xml_util.h>
}

#include <new>
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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __xpath_component_init(IN tt_component_t *comp,
                                          IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_xpath_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __xpath_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_XML_PATH, "XML Path", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

// ========================================
// xml path
// ========================================

tt_result_t tt_xpath_create(IN tt_xpath_t *xp,
                            IN const tt_char_t *expr,
                            IN OPT tt_xpvars_t *xpvs)
{
    pugi::xpath_query *xq = new (xp->p) pugi::xpath_query(expr, P_XPVS(xpvs));
    if (xq->result()) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to create xpath: %s", xq->result().description());
        return TT_FAIL;
    }
}

void tt_xpath_destroy(IN tt_xpath_t *xp)
{
    reinterpret_cast<pugi::xpath_query *>(xp->p)->~xpath_query();
}

// ========================================
// xml path variable set
// ========================================

void tt_xpvars_init(IN tt_xpvars_t *xpvs)
{
    new (xpvs->p) pugi::xpath_variable_set();
}

void tt_xpvars_destroy(IN tt_xpvars_t *xpvs)
{
    P_XPVS(xpvs)->~xpath_variable_set();
}

tt_result_t tt_xpvars_set_bool(IN tt_xpvars_t *xpvs,
                               IN const tt_char_t *name,
                               IN tt_bool_t val)
{
    if (P_XPVS(xpvs)->set(name, (bool)val)) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to set xpath bool: %s", name);
        return TT_FAIL;
    }
}

tt_result_t tt_xpvars_set_number(IN tt_xpvars_t *xpvs,
                                 IN const tt_char_t *name,
                                 IN tt_double_t val)
{
    if (P_XPVS(xpvs)->set(name, (double)val)) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to set xpath number: %s", name);
        return TT_FAIL;
    }
}

tt_result_t tt_xpvars_set_cstr(IN tt_xpvars_t *xpvs,
                               IN const tt_char_t *name,
                               IN const tt_char_t *val)
{
    if (P_XPVS(xpvs)->set(name, (const char *)val)) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to set xpath string: %s", name);
        return TT_FAIL;
    }
}

// ========================================
// xml path node set
// ========================================

void tt_xpnodes_init(IN tt_xpnodes_t *xpns)
{
    new (xpns->p) pugi::xpath_node_set();
}

void tt_xpnodes_destroy(IN tt_xpnodes_t *xpns)
{
    P_XPNS(xpns)->~xpath_node_set();
}

void tt_xpnodes_sort(IN tt_xpnodes_t *xpns, IN tt_bool_t reverse)
{
    P_XPNS(xpns)->sort(reverse);
}

tt_u32_t tt_xpnodes_count(IN tt_xpnodes_t *xpns)
{
    return (tt_u32_t)P_XPNS(xpns)->size();
}

void tt_xpnodes_iter(IN tt_xpnodes_t *xpns, OUT tt_xpnodes_iter_t *iter)
{
    iter->xpns = xpns;

    pugi::xpath_node_set::const_iterator i = P_XPNS(xpns)->begin();
    iter->p = T_ITER(i);
}

tt_result_t tt_xpnodes_iter_next(IN OUT tt_xpnodes_iter_t *iter,
                                 OUT tt_xnode_t *xn,
                                 OUT tt_xattr_t *xa)
{
    pugi::xpath_node_set::const_iterator i = P_ITER(iter->p);
    if (i != P_XPNS(iter->xpns)->end()) {
        pugi::xml_node pn = i->node();
        *xn = TN(pn);

        pugi::xml_attribute pa = i->attribute();
        *xa = TA(pa);

        ++i;
        iter->p = T_ITER(i);

        return TT_SUCCESS;
    } else {
        return TT_END;
    }
}

tt_result_t __xpath_component_init(IN tt_component_t *comp,
                                   IN tt_profile_t *profile)
{
    if (sizeof(tt_xpath_t) < sizeof(class pugi::xpath_query)) {
        TT_ERROR("sizeof(tt_xpath_t)[%d] < sizeof(class xpath_query)[%d]",
                 sizeof(tt_xpath_t),
                 sizeof(class pugi::xpath_query));
        return TT_FAIL;
    }

    if (sizeof(tt_xpvars_t) < sizeof(class pugi::xpath_variable_set)) {
        TT_ERROR(
            "sizeof(tt_xpvars_t)[%d] < sizeof(class xpath_variable_set)[%d]",
            sizeof(tt_xpvars_t),
            sizeof(class pugi::xpath_variable_set));
        return TT_FAIL;
    }

    if (sizeof(tt_xpnodes_t) < sizeof(class pugi::xpath_node_set)) {
        TT_ERROR("sizeof(tt_xpnodes_t)[%d] < sizeof(class xpath_node_set)[%d]",
                 sizeof(tt_xpnodes_t),
                 sizeof(class pugi::xpath_node_set));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
