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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_console_init_ntv()
{
    return TT_SUCCESS;
}

tt_result_t tt_console_enter_ntv()
{
    return TT_SUCCESS;
}

void tt_console_exit_ntv()
{
}

tt_result_t tt_console_config_ntv(IN struct tt_console_attr_s *attr)
{
    return TT_SUCCESS;
}

tt_result_t tt_console_recv_ntv(OUT tt_cons_ev_t *ev,
                                OUT tt_cons_ev_data_t *ev_data)
{
    return TT_SUCCESS;
}

tt_result_t tt_console_send_ntv(IN tt_cons_ev_t ev,
                                IN tt_cons_ev_data_t *ev_data)
{
    return TT_SUCCESS;
}
