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

#include <param/tt_param.h>

#include <algorithm/tt_buffer.h>
#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <param/tt_param_dir.h>

#include <tt_cstd_api.h>

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

tt_param_t *tt_g_param_root;

tt_param_t *tt_g_param_platform;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __param_component_init(IN tt_component_t *comp,
                                          IN tt_profile_t *profile);

static void __param_component_exit(IN tt_component_t *comp);

static tt_bool_t __name_ok(IN const tt_char_t *name);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_param_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __param_component_init, __param_component_exit,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_PARAM, "Parameter", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t tt_param_add2root(IN tt_param_t *co)
{
    return tt_param_dir_add(TT_PARAM_CAST(tt_g_param_root, tt_param_dir_t), co);
}

tt_result_t tt_param_add2platform(IN tt_param_t *co)
{
    return tt_param_dir_add(TT_PARAM_CAST(tt_g_param_platform, tt_param_dir_t),
                            co);
}

tt_param_t *tt_param_create(IN tt_u32_t len,
                            IN tt_param_type_t type,
                            IN const tt_char_t *name,
                            IN tt_param_itf_t *itf,
                            IN OPT void *opaque,
                            IN OPT tt_param_attr_t *attr)
{
    tt_param_t *p;
    tt_param_attr_t __attr;

    TT_ASSERT(TT_PARAM_TYPE_VALID(type));
    TT_ASSERT(name != NULL);
    TT_ASSERT(itf != NULL);

    if (!__name_ok(name)) {
        return NULL;
    }

    if (attr == NULL) {
        tt_param_attr_default(&__attr);
        attr = &__attr;
    }

    p = tt_zalloc(sizeof(tt_param_t) + len);
    if (p == NULL) {
        TT_ERROR("no mem for cfg node");
        return NULL;
    }

    p->name = name;
    p->display = attr->display;
    p->brief = attr->brief;
    p->detail = attr->detail;

    p->itf = itf;
    p->opaque = opaque;
    tt_lnode_init(&p->node);
    p->type = type;

    p->need_reboot = attr->need_reboot;
    p->can_read = attr->can_read;
    p->can_write = attr->can_write;
    p->level = attr->level;

    return p;
}

void tt_param_destroy(IN tt_param_t *p)
{
    TT_ASSERT(p != NULL);

    if (p->itf->on_destroy != NULL) {
        p->itf->on_destroy(p);
    }

    tt_free(p);
}

void tt_param_attr_default(IN tt_param_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->display = "";
    attr->brief = "";
    attr->detail = "";

    attr->need_reboot = TT_FALSE;
    attr->can_read = TT_TRUE;
    attr->can_write = TT_TRUE;
    attr->level = TT_PARAM_LV_ADMIN;
}

tt_result_t tt_param_read(IN tt_param_t *p, OUT tt_buf_t *output)
{
    TT_ASSERT(p != NULL);
    TT_ASSERT(p->itf != NULL);
    TT_ASSERT(output != NULL);

    if (!p->can_read || (p->itf->read == NULL)) {
        return TT_E_UNSUPPORT;
    }

    return p->itf->read(p, output);
}

tt_result_t tt_param_write(IN tt_param_t *p,
                           IN tt_u8_t *val,
                           IN tt_u32_t val_len)
{
    TT_ASSERT(p != NULL);
    TT_ASSERT(p->itf != NULL);
    TT_ASSERT(val != NULL);

    if (!p->can_write || (p->itf->write == NULL)) {
        return TT_E_UNSUPPORT;
    }

    return p->itf->write(p, val, val_len);
}

tt_param_t *tt_param_parent(IN tt_param_t *p)
{
    tt_param_dir_t *cdir;

    if (p->node.lst == NULL) {
        return NULL;
    }
    cdir = TT_CONTAINER(p->node.lst, tt_param_dir_t, child);

    return TT_PARAM_OF(cdir);
}

tt_result_t __param_component_init(IN tt_component_t *comp,
                                   IN tt_profile_t *profile)
{
    tt_param_attr_t attr;

    // create root config node
    tt_param_attr_default(&attr);

    tt_g_param_root = tt_param_dir_create("", &attr);
    if (tt_g_param_root == NULL) {
        TT_ERROR("fail to create config node: root");
        return TT_FAIL;
    }

    // create platform config node
    tt_param_attr_default(&attr);
    attr.brief = "platform configurations";
    attr.detail = "this directory includes all platform related configurations";

    tt_g_param_platform = tt_param_dir_create("platform", &attr);
    if (tt_g_param_platform == NULL) {
        TT_ERROR("fail to create config node: platform");
        return TT_FAIL;
    }

    if (!TT_OK(tt_param_dir_add(TT_PARAM_CAST(tt_g_param_root, tt_param_dir_t),
                                tt_g_param_platform))) {
        TT_ERROR("fail to add config node: platform");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void __param_component_exit(IN tt_component_t *comp)
{
    // destroying root would destroy all nodes
    tt_param_destroy(tt_g_param_root);
}

tt_bool_t __name_ok(IN const tt_char_t *name)
{
    tt_u32_t i, n;

    n = (tt_u32_t)tt_strlen(name);
    for (i = 0; i < n; ++i) {
        tt_char_t c = name[i];

        if (!tt_isalnum(c) && (c != '-')) {
            TT_ERROR("invalid char: %c, 0x%x", c, c);
            return TT_FALSE;
        }
    }

    return TT_TRUE;
}
