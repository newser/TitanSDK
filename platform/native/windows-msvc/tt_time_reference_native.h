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
@file tt_time_reference_native.h
@brief system time reference APIs

this file specifies system interfaces for time reference
*/

#ifndef __TT_TIME_REFERENCE_NATIVE__
#define __TT_TIME_REFERENCE_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// ========================================
// configuration
// ========================================

// these configurations would not be put to tt_platform_config.h

/**
@def TT_TIME_REF_NTV_TYPE_CS
spin lock is implemented as a windows critical section
*/
#define TT_TIME_REF_NTV_TYPE_QPC 1
/**
@def TT_TIME_REF_NTV_TYPE_UL
spin lock is implemented by pure spinning at user level
*/
#define TT_TIME_REF_NTV_TYPE_MMT 2

/**
@def TT_TIME_REF_NTV_TYPE
this macro determines the spin lock implementation, currently it could be:
- TT_TIME_REF_NTV_TYPE_QPC
- TT_TIME_REF_NTV_TYPE_MMT

@note
"mmt" behavors stable on more platforms(especially on virtual machines),
though it's of lower performance than "qpc"
*/
#define TT_TIME_REF_NTV_TYPE TT_TIME_REF_NTV_TYPE_MMT

// ========================================
// macro wrappers
// ========================================

#if (TT_TIME_REF_NTV_TYPE == TT_TIME_REF_NTV_TYPE_QPC)

#include <tt_time_reference_native_qpc.h>

#define tt_time_ref2ms_ntv tt_time_ref2ms_ntv_qpc
#define tt_time_ms2ref_ntv tt_time_ms2ref_ntv_qpc

#define tt_time_ref_component_init_ntv tt_time_ref_component_init_ntv_qpc

#define tt_time_ref_ntv tt_time_ref_ntv_qpc

#elif (TT_TIME_REF_NTV_TYPE == TT_TIME_REF_NTV_TYPE_MMT)

#include <tt_time_reference_native_mmt.h>

#define tt_time_ref2ms_ntv tt_time_ref2ms_ntv_mmt
#define tt_time_ms2ref_ntv tt_time_ms2ref_ntv_mmt

#define tt_time_ref_component_init_ntv tt_time_ref_component_init_ntv_mmt

#define tt_time_ref_ntv tt_time_ref_ntv_mmt

#else

#error "unknown time reference implementation"

#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_TIME_REFERENCE_NATIVE__ */
