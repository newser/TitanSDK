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

/**
@file tt_component.h
@brief platform component

this file defines ts platform component
*/

#ifndef __TT_COMPONENT__
#define __TT_COMPONENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_component_s;
struct tt_profile_s;

typedef enum {
    // never change the order of component id, which
    // determines components dependencies

    // log
    TT_COMPONENT_LOG,

    // config
    TT_COMPONENT_CONFIG,

    // version
    TT_COMPONENT_VERSION_INFO,

    // algorithm
    TT_COMPONENT_RAND,

    // os
    TT_COMPONENT_ATOMIC,
    TT_COMPONENT_STAMP_PTR,
    TT_COMPONENT_SPINLOCK,
    TT_COMPONENT_NUMA,
    TT_COMPONENT_PROCESS,

    // memory
    TT_COMPONENT_PAGE,
    TT_COMPONENT_MEMORY_POOL,
    TT_COMPONENT_SLAB,

    // thread
    TT_COMPONENT_THREAD,

    // time
    TT_COMPONENT_TIME_REF,
    TT_COMPONENT_TIMER_MGR,
    TT_COMPONENT_DATE,

    // event
    TT_COMPONENT_EVENT_CENTER,
    TT_COMPONENT_EVENT_POLLER,

    // io
    TT_COMPONENT_FILE_SYSTEM,
    TT_COMPONENT_IO_NETWORK,
    TT_COMPONENT_NETWORK_INTERFACE,
    TT_COMPONENT_SOCKET,
    TT_COMPONENT_IPC,
    TT_COMPONENT_CONSOLE,
    TT_COMPONENT_IO_POLLER,
    TT_COMPONENT_IO_WORKER_GROUP,

    // xml
    TT_COMPONENT_XML_NODE,
    TT_COMPONENT_XML_ATTR,
    TT_COMPONENT_XML_PATH,

    // crypto
    TT_COMPONENT_CRYPTO,

    // network
    TT_COMPONENT_SSL,

    // dns
    TT_COMPONENT_MODULE_ADNS,
    TT_COMPONENT_DNS,
    TT_COMPONENT_DNS_CACHE,

    // log manager
    TT_COMPONENT_LOG_MANAGER,
    TT_COMPONENT_LOGMGR_CONFIG,
    TT_COMPONENT_SSL_LOG,

    TT_COMPONENT_NUM
} tt_component_id_t;
#define TT_COMPONENT_ID_VALID(cid) ((cid) < TT_COMPONENT_NUM)

typedef tt_result_t (*tt_component_init_t)(IN struct tt_component_s *comp,
                                           IN struct tt_profile_s *profile);

typedef struct
{
    tt_component_init_t init;
} tt_component_itf_t;

typedef struct tt_component_s
{
    tt_component_id_t cid;
    const tt_char_t *name;
    void *opaque;

    tt_component_itf_t itf;
} tt_component_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// params of component may be expanded
extern void tt_component_init(IN tt_component_t *comp,
                              IN tt_component_id_t cid,
                              IN const tt_char_t *name,
                              IN void *opaque,
                              IN tt_component_itf_t *itf);

extern void tt_component_register(IN tt_component_t *comp);

/**
@note
- this function return TT_FAIL, if any component initialization failed.
  and those components which have been initialized wound not be
  uninitialized.
*/
extern tt_result_t tt_component_start(IN struct tt_profile_s *profile);

extern tt_component_t *tt_component_find_id(IN tt_component_id_t cid);

extern tt_component_t *tt_component_find_name(IN const tt_char_t *name);

extern tt_bool_t tt_component_is_started(IN tt_component_id_t cid);

#endif /* __TT_COMPONENT__ */
