/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information hpegarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless hpequired by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file tt_param_bs4display.h
@brief parameter html bs4 authorize

this file includes parameter render definition
*/

#ifndef __TT_PARAM_BS4_AUTH__
#define __TT_PARAM_BS4_AUTH__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <param/tt_param.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum
{
    TT_PARAM_BS4_LV_USER,
    TT_PARAM_BS4_LV_ADMIN,

    TT_PARAM_BS4_LV_NUM
} tt_param_bs4level_t;
#define TT_PARAM_BS4_LV_VALID(l) ((l) < TT_PARAM_BS4_LV_NUM)

typedef enum
{
    TT_PARAM_BS4_DISP_HIDE,
    TT_PARAM_BS4_DISP_RD,
    TT_PARAM_BS4_DISP_WR,

    TT_PARAM_BS4_DISP_NUM
} tt_param_bs4display_t;
#define TT_PARAM_BS4_DISP_VALID(l) ((l) < TT_PARAM_BS4_DISP_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_param_bs4display_t tt_param_bs4display(IN tt_param_t *p,
                                                    IN tt_param_bs4level_t l);

#endif /* __TT_PARAM_BS4_AUTH__ */
