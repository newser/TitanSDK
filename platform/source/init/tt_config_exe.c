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

#include <init/tt_config_exe.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_cfgobj_itf_t __cfgexe_itf = {
    NULL, NULL, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgobj_t *tt_cfgexe_create(IN const tt_char_t *name,
                              IN OPT tt_cfgobj_attr_t *attr,
                              IN tt_cfgexe_run_t run)
{
    tt_cfgobj_t *co;
    tt_cfgexe_t *ce;

    TT_ASSERT(run != NULL);

    co = tt_cfgobj_create(sizeof(tt_cfgexe_t),
                          TT_CFGOBJ_EXE,
                          name,
                          &__cfgexe_itf,
                          NULL,
                          attr);
    if (co == NULL) {
        return NULL;
    }

    ce = TT_CFGOBJ_CAST(co, tt_cfgexe_t);

    ce->run = run;

    return co;
}
