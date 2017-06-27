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

#include <tt_platform.h>

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

tt_result_t tt_platform_init(IN OPT const tt_char_t *profile_path)
{
    // ========================================
    // load configuration
    // ========================================

    if (!TT_OK(tt_profile_load(profile_path))) {
        TT_PRINTF("fail to load platform profile\n");
        return TT_FAIL;
    }

    // ========================================
    // load platform inforamtion
    // ========================================

    if (!TT_OK(tt_platform_info_load(&tt_g_profile))) {
        TT_PRINTF("fail to load platform inforamtion\n");
        return TT_FAIL;
    }

    // ========================================
    // register component
    // ========================================

    // log
    tt_log_component_register();
    tt_logmgr_component_register();
    tt_logmgr_config_component_register();

    // config
    tt_config_component_register();

    // version information
    tt_ver_component_register();

    // algoirthm
    tt_rng_component_register();

    // os
    tt_atomic_component_register();
    tt_mutex_component_register();
    tt_sem_component_register();
    tt_rwlock_component_register();
    tt_spinlock_component_register();
    tt_process_component_register();

    // memory
    tt_page_component_register();
    tt_mempool_component_register();
    tt_slab_component_register();

    // thread
    tt_thread_component_register();

    // time
    tt_time_ref_component_register();
    tt_tmr_mgr_component_register();
    tt_date_component_register();

    // io
    tt_fs_component_register();
    tt_network_io_component_register();
    tt_network_interface_component_register();
    tt_skt_component_register();
    tt_ipc_component_register();
    tt_console_component_register();
    tt_io_poller_component_register();
    tt_iowg_component_register();

    // xml
    tt_xnode_component_register();
    tt_xattr_component_register();
    tt_xpath_component_register();

    // network application
    tt_ssl_component_register();
    tt_ssl_log_component_register();
    // tt_adns_component_register();
    tt_dns_component_register();

    // ========================================
    // start component
    // ========================================

    if (!TT_OK(tt_component_start(&tt_g_profile))) {
        TT_PRINTF("fail to start all components\n");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
