/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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
#include <xml/tt_xml_attribute.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>
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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __xattr_component_init(IN tt_component_t *comp,
                                          IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_xattr_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __xattr_component_init,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_XML_ATTR,
                      "XML Attribute",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

const tt_char_t *tt_xattr_get_name(IN tt_xattr_t xa)
{
    return PA(xa).name();
}

tt_result_t tt_xattr_set_name(IN tt_xattr_t xa, IN const tt_char_t *name)
{
    if (PA(xa).set_name(name)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_xattr_t tt_xattr_next(IN tt_xattr_t xa)
{
    pugi::xml_attribute pa = PA(xa).next_attribute();
    return TA(pa);
}

tt_xattr_t tt_xattr_prev(IN tt_xattr_t xa)
{
    pugi::xml_attribute pa = PA(xa).previous_attribute();
    return TA(pa);
}

const tt_char_t *tt_xattr_get_value(IN tt_xattr_t xa, IN const tt_char_t *def)
{
    return PA(xa).as_string(def);
}

tt_result_t tt_xattr_set_value(IN tt_xattr_t xa, IN const tt_char_t *value)
{
    if (PA(xa).set_value(value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_bool_t tt_xattr_get_bool(IN tt_xattr_t xa, IN tt_bool_t def)
{
    if (PA(xa).as_bool(def == TT_TRUE)) {
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t tt_xattr_set_bool(IN tt_xattr_t xa, IN tt_bool_t value)
{
    if (PA(xa).set_value(value == TT_TRUE)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_s32_t tt_xattr_get_s32(IN tt_xattr_t xa, IN tt_s32_t def)
{
    return PA(xa).as_int(def);
}

tt_result_t tt_xattr_set_s32(IN tt_xattr_t xa, IN tt_s32_t value)
{
    if (PA(xa).set_value((int)value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_u32_t tt_xattr_get_u32(IN tt_xattr_t xa, IN tt_u32_t def)
{
    return PA(xa).as_uint(def);
}

tt_result_t tt_xattr_set_u32(IN tt_xattr_t xa, IN tt_u32_t value)
{
    if (PA(xa).set_value((unsigned int)value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_s64_t tt_xattr_get_s64(IN tt_xattr_t xa, IN tt_s64_t def)
{
    return PA(xa).as_llong(def);
}

tt_result_t tt_xattr_set_s64(IN tt_xattr_t xa, IN tt_s64_t value)
{
    if (PA(xa).set_value((long long)value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_u64_t tt_xattr_get_u64(IN tt_xattr_t xa, IN tt_u64_t def)
{
    return PA(xa).as_ullong(def);
}

tt_result_t tt_xattr_set_u64(IN tt_xattr_t xa, IN tt_u64_t value)
{
    if (PA(xa).set_value((unsigned long long)value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_float_t tt_xattr_get_float(IN tt_xattr_t xa, IN tt_float_t def)
{
    return PA(xa).as_float(def);
}

tt_result_t tt_xattr_set_float(IN tt_xattr_t xa, IN tt_float_t value)
{
    if (PA(xa).set_value(value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_double_t tt_xattr_get_double(IN tt_xattr_t xa, IN tt_double_t def)
{
    return PA(xa).as_double(def);
}

tt_result_t tt_xattr_set_double(IN tt_xattr_t xa, IN tt_double_t value)
{
    if (PA(xa).set_value(value)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t __xattr_component_init(IN tt_component_t *comp,
                                   IN tt_profile_t *profile)
{
    if (sizeof(tt_xattr_t) < sizeof(class pugi::xml_attribute)) {
        TT_ERROR("sizeof(tt_xattr_t)[%d] < sizeof(class xml_attribute)[%d]",
                 sizeof(tt_xattr_t),
                 sizeof(class pugi::xml_attribute));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
