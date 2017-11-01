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

#include <os/tt_process.h>

#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <init/tt_component.h>
#include <init/tt_profile.h>

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

static tt_char_t tt_s_process_name[128];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __process_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_process_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __process_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_PROCESS, "Process", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t tt_process_create(IN tt_process_t *proc,
                              IN const tt_char_t *path,
                              IN OPT tt_char_t *const arg[],
                              IN OPT tt_process_attr_t *attr)
{
    tt_process_attr_t __attr;

    TT_ASSERT(proc != NULL);
    TT_ASSERT(path != NULL);

    if (attr == NULL) {
        tt_process_attr_default(&__attr);
        attr = &__attr;
    }

    return tt_process_create_ntv(&proc->sys_proc, path, arg, attr);
}

tt_result_t tt_process_wait(IN tt_process_t *proc,
                            IN tt_bool_t block,
                            IN OPT tt_u8_t *exit_code)
{
    TT_ASSERT(proc != NULL);

    return tt_process_wait_ntv(&proc->sys_proc, block, exit_code);
}

void tt_process_attr_default(IN tt_process_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->reserved = 0;
}

void tt_process_exit(IN tt_u8_t exit_code)
{
    tt_process_exit_ntv(exit_code);
}

tt_char_t *tt_process_path(IN OPT tt_process_t *proc)
{
    return tt_process_path_ntv(TT_COND(proc != NULL, &proc->sys_proc, NULL));
}

const tt_char_t *tt_process_name()
{
    return tt_s_process_name;
}

tt_result_t __process_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    const tt_char_t *name;
    tt_u32_t len;

    name = tt_process_name_ntv();
    if (name != NULL) {
        len = (tt_u32_t)tt_strlen(name);
        if (len >= sizeof(tt_s_process_name)) {
            len = sizeof(tt_s_process_name) - 1;
        }
        tt_memcpy(tt_s_process_name, name, len);
    } else {
        tt_memcpy(tt_s_process_name, "unknown", sizeof("unknown") - 1);
    }
    
    return TT_SUCCESS;
}

