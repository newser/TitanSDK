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
@file tt_adns_init.h
@brief adns module initialization

this file defines adns module initialization apis

 1. architecture
 - client:
 adns_client --> adns_cache --> adns_domain --> adns_rr --> specfic rr
 |
 \|/
 adns_storage --> local os dns info
 /|\
 - server:           |
 adns_server --> adns_db (same role as cache, different policy)

 2. dns query
 - adns_query_context, holding status and determine what next step should
 be, such like continuing query a srv record for A record or try another
 dns server
 - adns_transaction, a simpile query (request + response), determine how
 to retransmitting or if should stop retransmittion

 */

#ifndef __TT_ADNS_INIT__
#define __TT_ADNS_INIT__

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

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_adns_component_register();

#endif /* __TT_ADNS_INIT__ */
