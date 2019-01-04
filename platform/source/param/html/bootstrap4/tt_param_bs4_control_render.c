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

#include <param/html/bootstrap4/tt_param_bs4_control.h>

#include <algorithm/tt_buffer_format.h>
#include <param/html/bootstrap4/tt_param_bs4_content.h>

#define TT_PARAM_BS4_INTERNAL_USAGE
#include <param/html/bootstrap4/tt_param_bs4_internal_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __2ENTRY_INPUT_RD_START                                                \
    "<div class=\"form-group row col-12 col-md-6 px-0 py-1\">"                 \
    "<label class=\"col-4 text-right pr-0 col-form-label %s\">%s :</label>"    \
    "<div class=\"col-6\">"                                                    \
    "<input type=\"text\" class=\"form-control-plaintext %s\" id=\"%s\" "      \
    "readonly %s %s %s %s value=\""

#define __2ENTRY_INPUT_END "\" /></div></div>"

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

tt_result_t tt_param_bs4_ctrl_render(IN tt_param_bs4_content_t *ct,
                                     IN tt_param_t *param,
                                     IN tt_param_bs4_level_t lv,
                                     OUT tt_buf_t *buf)
{
    return TT_SUCCESS;
}

tt_result_t tt_param_bs4_ctrl_render_pair(IN tt_param_bs4_content_t *ct,
                                          IN tt_param_t *p1,
                                          IN tt_param_t *p2,
                                          IN tt_param_bs4_level_t lv,
                                          OUT tt_buf_t *buf)
{
    tt_param_bs4_display_t disp;
    /*
        disp = tt_param_bs4_display(p1, lv);
        if (disp == TT_PARAM_BS4_DISP_RD) {
            TT_DO(tt_buf_putf(buf,
                              __2ENTRY_INPUT_RD_START,
                              ct->name_class,
                              __param_display(p1),
                              ct->val_class,
                              tt_param_name(p1)));
            TT_DO(tt_param_read(p1, buf));
            __PUT_CSTR(buf, __2ENTRY_INPUT_END);
        } else {
            TT_ASSERT(disp == TT_PARAM_BS4_DISP_WR);

            TT_DO(tt_buf_putf(buf,
                              __2ENTRY_INPUT_WR_START,
                              ct->name_class,
                              __param_display(p1),
                              ct->val_class,
                              tt_param_name(p1)));
            TT_DO(tt_param_read(p1, buf));
            __PUT_CSTR(buf, __2ENTRY_INPUT_END);
        }

        disp = tt_param_bs4_display(p2, lv);
        if (disp == TT_PARAM_BS4_DISP_RD) {
            TT_DO(tt_buf_putf(buf,
                              __2ENTRY_INPUT_RD_START,
                              ct->name_class,
                              __param_display(p2),
                              ct->val_class,
                              tt_param_name(p2)));
            TT_DO(tt_param_read(p2, buf));
            __PUT_CSTR(buf, __2ENTRY_INPUT_END);
        } else {
            TT_ASSERT(disp == TT_PARAM_BS4_DISP_WR);

            TT_DO(tt_buf_putf(buf,
                              __2ENTRY_INPUT_WR_START,
                              ct->name_class,
                              __param_display(p2),
                              ct->val_class,
                              tt_param_name(p2)));
            TT_DO(tt_param_read(p2, buf));
            __PUT_CSTR(buf, __2ENTRY_INPUT_END);
        }
    */
    return TT_SUCCESS;
}
