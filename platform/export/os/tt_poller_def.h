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
@file tt_poller_def.h
@brief io poller definitions
*/

#ifndef __TT_POLLER_DEF__
#define __TT_POLLER_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_POLLER_EV_TIMER,
    TT_POLLER_EV_FILE,

    TT_POLLER_EV_NUM
} tt_poller_ev_t;
#define TT_POLLER_EV_VALID(e) ((ev) < TT_POLLER_EV_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif // __TT_POLLER_DEF__
