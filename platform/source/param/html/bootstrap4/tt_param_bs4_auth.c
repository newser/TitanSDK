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
 * Unless required by anavlicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <param/html/bootstrap4/tt_param_bs4_auth.h>

////////////////////////////////////////////////////////////
// internal macro
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

tt_param_bs4_display_t tt_param_bs4_display(IN tt_param_t *p,
                                            IN tt_param_bs4_level_t l)
{
    static tt_param_bs4_level_t map[TT_PARAM_BS4_LV_NUM][TT_PARAM_LV_NUM] = {
        // TT_PARAM_BS4_LV_USER
        {
            TT_PARAM_BS4_DISP_RD,
            TT_PARAM_BS4_DISP_WR,
            TT_PARAM_BS4_DISP_WR,
            TT_PARAM_BS4_DISP_RD,
            TT_PARAM_BS4_DISP_HIDE,
            TT_PARAM_BS4_DISP_HIDE,
        },

        // TT_PARAM_BS4_LV_ADMIN
        {
            TT_PARAM_BS4_DISP_RD,
            TT_PARAM_BS4_DISP_WR,
            TT_PARAM_BS4_DISP_WR,
            TT_PARAM_BS4_DISP_WR,
            TT_PARAM_BS4_DISP_RD,
            TT_PARAM_BS4_DISP_WR,
        },
    };

    return map[l][p->level];
}
