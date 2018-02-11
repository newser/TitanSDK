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

/**
 @file tt_console_native.h
 @brief console io native

 this file specifies console native APIs
 */

#ifndef __TT_CONSOLE_NATIVE__
#define __TT_CONSOLE_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_console.h>
#include <io/tt_console_event.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

struct tt_console_attr_s;

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_console_component_init_ntv();

extern tt_result_t tt_console_enter_ntv();

extern void tt_console_exit_ntv();

extern tt_result_t tt_console_config_ntv(IN struct tt_console_attr_s *attr);

extern tt_result_t tt_console_recv_ntv(OUT tt_cons_ev_t *ev,
                                       OUT tt_cons_ev_data_t *ev_data);

extern tt_result_t tt_console_send_ntv(IN tt_cons_ev_t ev,
                                       IN tt_cons_ev_data_t *ev_data);

tt_inline void tt_console_set_color_ntv(IN tt_console_color_t foreground,
                                        IN tt_console_color_t background)
{
}

#endif // __TT_CONSOLE_NATIVE__
