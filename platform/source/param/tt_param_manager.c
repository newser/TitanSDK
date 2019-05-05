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

#include <param/tt_param_manager.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <os/tt_fiber_event.h>
#include <os/tt_task.h>
#include <param/tt_param_bool.h>
#include <param/tt_param_path.h>
#include <param/tt_param_s32.h>
#include <param/tt_param_string.h>
#include <param/tt_param_u32.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    GET_BOOL,
    SET_BOOL,

    GET_U32,
    SET_U32,

    GET_S32,
    SET_S32,

    GET_STRING,
    SET_STRING,

    PMMSG_NUM
};

typedef struct
{
    tt_fiber_ev_t fev;
    const tt_char_t *path;
    tt_u32_t len;
    tt_result_t result;
    union
    {
        tt_bool_t val_bool;
        tt_u32_t val_u32;
        tt_s32_t val_s32;
        tt_string_t *val_str;
        struct
        {
            const tt_char_t *p;
            tt_u32_t len;
        } val_cstr;
    };
} __gs_param_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_param_mgr_t tt_g_param_mgr;

static tt_task_t tt_s_param_mgr_task;

static tt_u32_t tt_s_param_mgr_comp_status = 0;
#define __PMC_MGR (1 << 0)
#define __PMC_TASK (1 << 1)

static void __get_bool(IN tt_fiber_ev_t *fev);

static void __set_bool(IN tt_fiber_ev_t *fev);

static void __get_u32(IN tt_fiber_ev_t *fev);

static void __set_u32(IN tt_fiber_ev_t *fev);

static void __get_s32(IN tt_fiber_ev_t *fev);

static void __set_s32(IN tt_fiber_ev_t *fev);

static void __get_str(IN tt_fiber_ev_t *fev);

static void __set_str(IN tt_fiber_ev_t *fev);

static void (*__handle_pmmsg[PMMSG_NUM])(IN tt_fiber_ev_t *fev) = {
    __get_bool, __set_bool, __get_u32, __set_u32,
    __get_s32,  __set_s32,  __get_str, __set_str,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __param_mgr_component_init(IN tt_component_t *comp,
                                              IN tt_profile_t *profile);

static void __param_mgr_component_exit(IN tt_component_t *comp);

static void __gs_param_init(IN __gs_param_t *gsp, IN tt_u32_t ev,
                            IN const tt_char_t *path, IN tt_u32_t len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_param_mgr_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __param_mgr_component_init,
        __param_mgr_component_exit,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_PARAM_MANAGER, "Parameter Manager",
                      NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t tt_param_mgr_create(IN tt_param_mgr_t *pm, IN tt_param_t *root,
                                IN OPT tt_param_mgr_attr_t *attr)
{
    tt_param_mgr_attr_t __attr;

    if (attr == NULL) {
        tt_param_mgr_attr_default(&__attr);
        attr = &__attr;
    }

    pm->root = root;
    pm->fb = NULL;

    return TT_SUCCESS;
}

void tt_param_mgr_destroy(IN tt_param_mgr_t *pm)
{
    TT_ASSERT(pm != NULL);
}

void tt_param_mgr_attr_default(IN tt_param_mgr_attr_t *attr)
{
    TT_ASSERT(attr != NULL);
}

tt_param_t *tt_param_mgr_find_n(IN tt_param_mgr_t *pm, IN const tt_char_t *path,
                                IN tt_u32_t len)
{
    return tt_param_path_p2n(pm->root, pm->root, path, len);
}

tt_result_t tt_param_mgr_fiber_routine(IN void *param)
{
    tt_param_mgr_t *pm;
    tt_fiber_t *fb;
    tt_fiber_ev_t *fev;

    pm = (tt_param_mgr_t *)param;

    fb = tt_current_fiber();
    TT_ASSERT(fb != NULL);

    TT_ASSERT(pm->fb == NULL);
    pm->fb = fb;

    while ((fev = tt_fiber_recv_ev(fb, TT_TRUE)) != NULL) {
        TT_ASSERT(fev->ev < PMMSG_NUM);
        __handle_pmmsg[fev->ev](fev);
        tt_fiber_finish(fev);
    }

    return TT_SUCCESS;
}

tt_result_t tt_param_mgr_get_bool(IN tt_param_mgr_t *pm,
                                  IN const tt_char_t *path, IN tt_u32_t len,
                                  OUT tt_bool_t *val)
{
    tt_param_t *p = tt_param_mgr_find_n(pm, path, len);
    if (p == NULL) {
        return TT_E_NOEXIST;
    } else if (p->type != TT_PARAM_BOOL) {
        return TT_E_BADARG;
    } else {
        *val = tt_param_get_bool(p);
        return TT_SUCCESS;
    }
}

tt_result_t tt_param_mgr_set_bool(IN tt_param_mgr_t *pm,
                                  IN const tt_char_t *path, IN tt_u32_t len,
                                  IN tt_bool_t val)
{
    tt_param_t *p = tt_param_mgr_find_n(pm, path, len);
    if (p == NULL) {
        return TT_E_NOEXIST;
    } else if (p->type != TT_PARAM_BOOL) {
        return TT_E_BADARG;
    } else {
        tt_param_set_bool(p, val);
        return TT_SUCCESS;
    }
}

tt_result_t tt_param_mgr_get_u32(IN tt_param_mgr_t *pm,
                                 IN const tt_char_t *path, IN tt_u32_t len,
                                 OUT tt_u32_t *val)
{
    tt_param_t *p = tt_param_mgr_find_n(pm, path, len);
    if (p == NULL) {
        return TT_E_NOEXIST;
    } else if (p->type != TT_PARAM_U32) {
        return TT_E_BADARG;
    } else {
        *val = tt_param_get_u32(p);
        return TT_SUCCESS;
    }
}

tt_result_t tt_param_mgr_set_u32(IN tt_param_mgr_t *pm,
                                 IN const tt_char_t *path, IN tt_u32_t len,
                                 IN tt_u32_t val)
{
    tt_param_t *p = tt_param_mgr_find_n(pm, path, len);
    if (p == NULL) {
        return TT_E_NOEXIST;
    } else if (p->type != TT_PARAM_U32) {
        return TT_E_BADARG;
    } else {
        tt_param_set_u32(p, val);
        return TT_SUCCESS;
    }
}

tt_result_t tt_param_mgr_get_s32(IN tt_param_mgr_t *pm,
                                 IN const tt_char_t *path, IN tt_u32_t len,
                                 OUT tt_s32_t *val)
{
    tt_param_t *p = tt_param_mgr_find_n(pm, path, len);
    if (p == NULL) {
        return TT_E_NOEXIST;
    } else if (p->type != TT_PARAM_S32) {
        return TT_E_BADARG;
    } else {
        *val = tt_param_get_s32(p);
        return TT_SUCCESS;
    }
}

tt_result_t tt_param_mgr_set_s32(IN tt_param_mgr_t *pm,
                                 IN const tt_char_t *path, IN tt_u32_t len,
                                 IN tt_s32_t val)
{
    tt_param_t *p = tt_param_mgr_find_n(pm, path, len);
    if (p == NULL) {
        return TT_E_NOEXIST;
    } else if (p->type != TT_PARAM_S32) {
        return TT_E_BADARG;
    } else {
        tt_param_set_s32(p, val);
        return TT_SUCCESS;
    }
}

tt_result_t tt_param_mgr_get_str(IN tt_param_mgr_t *pm,
                                 IN const tt_char_t *path, IN tt_u32_t len,
                                 OUT tt_string_t *val)
{
    tt_param_t *p = tt_param_mgr_find_n(pm, path, len);
    if (p == NULL) {
        return TT_E_NOEXIST;
    } else if (p->type != TT_PARAM_STRING) {
        return TT_E_BADARG;
    } else {
        return TT_COND(tt_param_get_str(p, val) != NULL, TT_SUCCESS,
                       TT_E_NOMEM);
    }
}

tt_result_t tt_param_mgr_set_str(IN tt_param_mgr_t *pm,
                                 IN const tt_char_t *path, IN tt_u32_t len,
                                 IN const tt_char_t *val, IN tt_u32_t val_len)
{
    tt_param_t *p = tt_param_mgr_find_n(pm, path, len);
    if (p == NULL) {
        return TT_E_NOEXIST;
    } else if (p->type != TT_PARAM_STRING) {
        return TT_E_BADARG;
    } else {
        return tt_param_set_str_n(p, val, val_len);
    }
}

tt_result_t tt_get_param_bool(IN const tt_char_t *path, IN tt_u32_t len,
                              OUT tt_bool_t *val)
{
    __gs_param_t gsp;

    __gs_param_init(&gsp, GET_BOOL, path, len);

    tt_fiber_send_ev(tt_g_param_mgr.fb, &gsp.fev, TT_TRUE);
    *val = gsp.val_bool;
    return gsp.result;
}

tt_result_t tt_set_param_bool(IN const tt_char_t *path, IN tt_u32_t len,
                              IN tt_bool_t val)
{
    __gs_param_t gsp;

    __gs_param_init(&gsp, SET_BOOL, path, len);
    gsp.val_bool = val;

    tt_fiber_send_ev(tt_g_param_mgr.fb, &gsp.fev, TT_TRUE);
    return gsp.result;
}

tt_result_t tt_get_param_u32(IN const tt_char_t *path, IN tt_u32_t len,
                             OUT tt_u32_t *val)
{
    __gs_param_t gsp;

    __gs_param_init(&gsp, GET_U32, path, len);

    tt_fiber_send_ev(tt_g_param_mgr.fb, &gsp.fev, TT_TRUE);
    *val = gsp.val_u32;
    return gsp.result;
}

tt_result_t tt_set_param_u32(IN const tt_char_t *path, IN tt_u32_t len,
                             IN tt_u32_t val)
{
    __gs_param_t gsp;

    __gs_param_init(&gsp, SET_U32, path, len);
    gsp.val_u32 = val;

    tt_fiber_send_ev(tt_g_param_mgr.fb, &gsp.fev, TT_TRUE);
    return gsp.result;
}

tt_result_t tt_get_param_s32(IN const tt_char_t *path, IN tt_u32_t len,
                             OUT tt_s32_t *val)
{
    __gs_param_t gsp;

    __gs_param_init(&gsp, GET_S32, path, len);

    tt_fiber_send_ev(tt_g_param_mgr.fb, &gsp.fev, TT_TRUE);
    *val = gsp.val_s32;
    return gsp.result;
}

tt_result_t tt_set_param_s32(IN const tt_char_t *path, IN tt_u32_t len,
                             IN tt_s32_t val)
{
    __gs_param_t gsp;

    __gs_param_init(&gsp, SET_S32, path, len);
    gsp.val_s32 = val;

    tt_fiber_send_ev(tt_g_param_mgr.fb, &gsp.fev, TT_TRUE);
    return gsp.result;
}

tt_result_t tt_get_param_str(IN const tt_char_t *path, IN tt_u32_t len,
                             OUT tt_string_t *val)
{
    __gs_param_t gsp;

    __gs_param_init(&gsp, GET_STRING, path, len);
    gsp.val_str = val;

    tt_fiber_send_ev(tt_g_param_mgr.fb, &gsp.fev, TT_TRUE);
    return gsp.result;
}

tt_result_t tt_set_param_str(IN const tt_char_t *path, IN tt_u32_t len,
                             IN const tt_char_t *val, IN tt_u32_t val_len)
{
    __gs_param_t gsp;

    __gs_param_init(&gsp, SET_STRING, path, len);
    gsp.val_cstr.p = val;
    gsp.val_cstr.len = val_len;

    tt_fiber_send_ev(tt_g_param_mgr.fb, &gsp.fev, TT_TRUE);
    return gsp.result;
}

tt_result_t __param_mgr_component_init(IN tt_component_t *comp,
                                       IN tt_profile_t *profile)
{
    tt_param_mgr_attr_t pm_attr;
    tt_task_attr_t tsk_attr;
    tt_fiber_attr_t fb_attr;
    tt_fiber_t *fb;

    // ========================================
    // create param manager
    // ========================================

    tt_param_mgr_attr_default(&pm_attr);

    if (!TT_OK(
            tt_param_mgr_create(&tt_g_param_mgr, tt_g_param_root, &pm_attr))) {
        return TT_FAIL;
    }
    tt_s_param_mgr_comp_status |= __PMC_MGR;

    // ========================================
    // create param manager task
    // ========================================

    // create task
    tt_task_attr_default(&tsk_attr);

    if (!TT_OK(tt_task_create(&tt_s_param_mgr_task, &tsk_attr))) {
        return TT_FAIL;
    }
    tt_s_param_mgr_comp_status |= __PMC_TASK;

    // create fiber
    tt_fiber_attr_default(&fb_attr);

    if (!TT_OK(tt_task_add_fiber(&tt_s_param_mgr_task, "Parameter Manager",
                                 tt_param_mgr_fiber_routine, &tt_g_param_mgr,
                                 &fb_attr)) ||
        !TT_OK(tt_task_run(&tt_s_param_mgr_task))) {
        return TT_FAIL;
    }

    {
        volatile tt_param_mgr_t *vpm;
        vpm = &tt_g_param_mgr;
        while (vpm->fb == NULL)
            ;
    }

    return TT_SUCCESS;
}

void __param_mgr_component_exit(IN tt_component_t *comp)
{
    if (tt_s_param_mgr_comp_status & __PMC_TASK) {
        tt_task_exit(&tt_s_param_mgr_task);
        tt_task_wait(&tt_s_param_mgr_task);
    }

    if (tt_s_param_mgr_comp_status & __PMC_MGR) {
        tt_param_mgr_destroy(&tt_g_param_mgr);
    }
}

void __gs_param_init(IN __gs_param_t *gsp, IN tt_u32_t ev,
                     IN const tt_char_t *path, IN tt_u32_t len)
{
    tt_fiber_ev_init(&gsp->fev, ev);
    gsp->path = path;
    gsp->len = len;
    gsp->result = TT_FAIL;
}

void __get_bool(IN tt_fiber_ev_t *fev)
{
    __gs_param_t *gsp = TT_CONTAINER(fev, __gs_param_t, fev);

    gsp->result = tt_param_mgr_get_bool(&tt_g_param_mgr, gsp->path, gsp->len,
                                        &gsp->val_bool);
}

void __set_bool(IN tt_fiber_ev_t *fev)
{
    __gs_param_t *gsp = TT_CONTAINER(fev, __gs_param_t, fev);

    gsp->result = tt_param_mgr_set_bool(&tt_g_param_mgr, gsp->path, gsp->len,
                                        gsp->val_bool);
}

void __get_u32(IN tt_fiber_ev_t *fev)
{
    __gs_param_t *gsp = TT_CONTAINER(fev, __gs_param_t, fev);

    gsp->result = tt_param_mgr_get_u32(&tt_g_param_mgr, gsp->path, gsp->len,
                                       &gsp->val_u32);
}

void __set_u32(IN tt_fiber_ev_t *fev)
{
    __gs_param_t *gsp = TT_CONTAINER(fev, __gs_param_t, fev);

    gsp->result = tt_param_mgr_set_u32(&tt_g_param_mgr, gsp->path, gsp->len,
                                       gsp->val_u32);
}

void __get_s32(IN tt_fiber_ev_t *fev)
{
    __gs_param_t *gsp = TT_CONTAINER(fev, __gs_param_t, fev);

    gsp->result = tt_param_mgr_get_s32(&tt_g_param_mgr, gsp->path, gsp->len,
                                       &gsp->val_s32);
}

void __set_s32(IN tt_fiber_ev_t *fev)
{
    __gs_param_t *gsp = TT_CONTAINER(fev, __gs_param_t, fev);

    gsp->result = tt_param_mgr_set_s32(&tt_g_param_mgr, gsp->path, gsp->len,
                                       gsp->val_s32);
}

void __get_str(IN tt_fiber_ev_t *fev)
{
    __gs_param_t *gsp = TT_CONTAINER(fev, __gs_param_t, fev);

    gsp->result = tt_param_mgr_get_str(&tt_g_param_mgr, gsp->path, gsp->len,
                                       gsp->val_str);
}

void __set_str(IN tt_fiber_ev_t *fev)
{
    __gs_param_t *gsp = TT_CONTAINER(fev, __gs_param_t, fev);

    gsp->result = tt_param_mgr_set_str(&tt_g_param_mgr, gsp->path, gsp->len,
                                       gsp->val_cstr.p, gsp->val_cstr.len);
}
