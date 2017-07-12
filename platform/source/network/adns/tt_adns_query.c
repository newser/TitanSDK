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

#include <network/adns/tt_adns_query.h>

#include <event/tt_event_center.h>
#include <network/adns/tt_adns_domain_manager.h>

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

tt_result_t tt_adns_query(IN const tt_char_t *domain,
                          IN tt_adns_rr_type_t type,
                          IN tt_u32_t flag,
                          OUT tt_adns_qryctx_t *qryctx)
{
    tt_evcenter_t *evc = tt_evc_current();

    if (evc == NULL) {
        TT_ERROR("not in evc");
        return TT_FAIL;
    }
    if (evc->adns_dmgr == NULL) {
        TT_ERROR("evc has not enabled adns");
        return TT_FAIL;
    }

    return tt_adns_dmgr_query(evc->adns_dmgr, domain, type, flag, qryctx);
}

tt_result_t tt_adns_query_async(IN const tt_char_t *domain,
                                IN tt_adns_rr_type_t type,
                                IN tt_u32_t flag,
                                IN tt_adns_on_query_t on_query,
                                IN OPT void *cb_param)
{
    tt_evcenter_t *evc = tt_evc_current();

    if (evc == NULL) {
        TT_ERROR("not in evc");
        return TT_FAIL;
    }
    if (evc->adns_dmgr == NULL) {
        TT_ERROR("evc has not enabled adns");
        return TT_FAIL;
    }

    return tt_adns_dmgr_query_async(evc->adns_dmgr,
                                    domain,
                                    type,
                                    flag,
                                    on_query,
                                    cb_param);
}
