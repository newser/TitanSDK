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
@file tt_param_bs4_util.h
@brief parameter render a html page

this file includes parameter render definition
*/

#ifndef __TT_PARAM_BS4_INTERNAL_UTIL__
#define __TT_PARAM_BS4_INTERNAL_UTIL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#ifndef TT_PARAM_BS4_INTERNAL_USAGE
#error this file is for param bootstrap4 module internal usage
#endif

#include <param/tt_param.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define __NULL_EMPTY(s) TT_COND(s != NULL, s, "")

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline const tt_char_t *__param_display(IN tt_param_t *p)
{
    const tt_char_t *disp;

    disp = tt_param_display(p);
    if (disp[0] != 0) {
        return disp;
    }

    disp = tt_param_name(p);
    if (disp[0] != 0) {
        return disp;
    }

    return "?";
}

#endif /* __TT_PARAM_BS4_INTERNAL_UTIL__ */
