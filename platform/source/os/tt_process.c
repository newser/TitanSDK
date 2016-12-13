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

#include <os/tt_process.h>

#include <misc/tt_assert.h>

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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_process_component_register()
{
}

tt_result_t tt_process_create(IN tt_process_t *proc,
                              IN const tt_char_t *path,
                              IN OPT tt_char_t *const arg[],
                              IN OPT tt_process_attr_t *attr)
{
    TT_ASSERT(proc != NULL);
    TT_ASSERT(path != NULL);

    if (attr != NULL) {
        tt_memcpy(&proc->attr, attr, sizeof(tt_process_attr_t));
    } else {
        tt_process_attr_default(&proc->attr);
    }

    return tt_process_create_ntv(&proc->sys_process, path, arg, &proc->attr);
}

tt_result_t tt_process_wait(IN tt_process_t *proc,
                            IN tt_bool_t block,
                            IN OPT tt_u8_t *exit_code)
{
    TT_ASSERT(proc != NULL);

    return tt_process_wait_ntv(&proc->sys_process, block, exit_code);
}

void tt_process_attr_default(IN tt_process_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_memset(attr, 0, sizeof(tt_process_attr_t));
}

void tt_process_exit(IN tt_u8_t exit_code)
{
    tt_process_exit_ntv(exit_code);
}
