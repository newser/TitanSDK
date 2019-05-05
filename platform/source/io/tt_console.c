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

#include <io/tt_console.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <misc/tt_assert.h>
#include <os/tt_atomic.h>
#include <os/tt_thread.h>

#include <tt_console_native.h>

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

static tt_atomic_s32_t tt_s_console_running;

static tt_cons_ev_handler_t tt_s_console_ev_handler;
static void *tt_s_console_param;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __console_component_init(IN tt_component_t *comp,
                                            IN tt_profile_t *profile);

static void __console_component_exit(IN tt_component_t *comp);

static void __console_run();

static tt_result_t __console_thread(IN void *param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_console_component_register()
{
#if !TT_ENV_OS_IS_IOS && !TT_ENV_OS_IS_ANDROID
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __console_component_init,
        __console_component_exit,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_CONSOLE, "Console", NULL, &itf);

    // register component
    tt_component_register(&comp);
#endif
}

void tt_console_attr_default(IN tt_console_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->input_mode = TT_CONSOLE_IMODE_DEFAULT;
    attr->output_mode = TT_CONSOLE_OMODE_DEFAULT;

    // default subst: '.'
    attr->unprintable_substitutor = '.';
}

tt_result_t tt_console_config(IN tt_console_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    TT_ASSERT(TT_CONSOLE_IMODE_VALID(attr->input_mode));
    TT_ASSERT(TT_CONSOLE_OMODE_VALID(attr->output_mode));

    return tt_console_config_ntv(attr);
}

void tt_console_run(IN tt_cons_ev_handler_t ev_handler, IN void *param,
                    IN tt_bool_t local)
{
    TT_ASSERT(ev_handler != NULL);

    if (!TT_OK(tt_atomic_s32_cas(&tt_s_console_running, 0, 1))) {
        TT_ERROR("console is already running");
        return;
    }

    tt_s_console_ev_handler = ev_handler;
    tt_s_console_param = param;

    if (local) {
        __console_run();
    } else {
        tt_thread_attr_t attr;

        tt_thread_attr_default(&attr);
        attr.detached = TT_TRUE;

        if (tt_thread_create(__console_thread, NULL, &attr) == NULL) {
            TT_ERROR("fail to craete console thread");
        }
    }
}

tt_result_t tt_console_send(IN tt_cons_ev_t ev, IN tt_cons_ev_data_t *ev_data)
{
    TT_ASSERT(TT_CONS_EV_VALID(ev));

    return tt_console_send_ntv(ev, ev_data);
}

void tt_console_set_color(IN tt_console_color_t foreground,
                          IN tt_console_color_t background)
{
    TT_ASSERT(TT_CONSOLE_COLOR_VALID(foreground));
    TT_ASSERT(TT_CONSOLE_COLOR_VALID(background));

    tt_console_set_color_ntv(foreground, background);
}

tt_result_t __console_component_init(IN tt_component_t *comp,
                                     IN tt_profile_t *profile)
{
    if (!TT_OK(tt_console_component_init_ntv())) { return TT_FAIL; }

    tt_atomic_s32_set(&tt_s_console_running, 0);

    tt_s_console_ev_handler = NULL;
    tt_s_console_param = NULL;

    return TT_SUCCESS;
}

void __console_component_exit(IN tt_component_t *comp)
{
    tt_console_component_exit_ntv();
}

void __console_run()
{
    tt_cons_ev_t ev;
    tt_cons_ev_data_t ev_data;

    if (!TT_OK(tt_console_enter_ntv())) { return; }
    while (TT_OK(tt_console_recv_ntv(&ev, &ev_data)) &&
           TT_OK(tt_s_console_ev_handler(tt_s_console_param, ev, &ev_data)))
        ;
    tt_console_exit_ntv();

    tt_atomic_s32_set(&tt_s_console_running, 0);
}

tt_result_t __console_thread(IN void *param)
{
    __console_run();
    return TT_SUCCESS;
}
