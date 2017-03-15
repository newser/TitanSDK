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

#include <tt_poller_native.h>

#include <tt_sys_error.h>
#include <tt_util_native.h>

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

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

tt_result_t tt_poller_create_ntv(IN tt_poller_ntv_t *sys_poller)
{
    sys_poller->kq_fd = kqueue();
    if (sys_poller->kq_fd < 0) {
        TT_ERROR_NTV("fail to create kqueue fd");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_poller_destroy_ntv(IN tt_poller_ntv_t *sys_poller)
{
    __RETRY_IF_EINTR(close(sys_poller->kq_fd) != 0);
}

tt_result_t tt_poller_run_ntv(IN tt_poller_ntv_t *sys_poller,
                              IN tt_s64_t wait_ms,
                              OUT tt_poller_ev_t *ev,
                              OUT void **data)
{
}
