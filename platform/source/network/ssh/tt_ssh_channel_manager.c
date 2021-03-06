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

#include <network/ssh/tt_ssh_channel_manager.h>

#include <misc/tt_assert.h>

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

tt_result_t tt_sshchmgr_send(IN tt_sshchmgr_t *chmgr,
                             IN struct tt_sshmsg_s *msg)
{
    TT_ASSERT(chmgr->itf->send != NULL);

    return chmgr->itf->send(chmgr, msg);
}

tt_result_t tt_sshchmgr_extra_size(IN struct tt_sshchmgr_s *chmgr,
                                   OUT tt_u32_t *head_mac,
                                   OUT tt_u32_t *block_size)
{
    TT_ASSERT(chmgr->itf->extra_size != NULL);

    return chmgr->itf->extra_size(chmgr, head_mac, block_size);
}

void tt_sshchmgr_on_ch_destroy(IN tt_sshchmgr_t *chmgr,
                               IN struct tt_sshch_s *ch)
{
    if (chmgr->itf->on_ch_destroy != NULL) {
        chmgr->itf->on_ch_destroy(chmgr, ch);
    }
}
