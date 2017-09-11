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
@file tt_time_reference.h
@brief get system time reference

this file specifies time reference APIs.

<hr>

<b>"REFERENCE" TIME</b><br>
"REFERENCE" time is the time since some point after system start up,
the "point" is not always the time when system boots up

*/

#ifndef __TT_TIME_REFERENCE__
#define __TT_TIME_REFERENCE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_time_reference_native.h>

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

/**
@fn void tt_time_ref_component_register()
register ts time reference system
*/
tt_export void tt_time_ref_component_register();

/**
@fn tt_s64_t tt_time_ref()
get system reference time

@return
system reference time in millisecond unit

@note
- returned value should not be supposed in undefined units, use predefined
  utilities such like tt_time_ref2ms() to get exact time value
- time value returned is not supposed to be monotonical
- time wrap around issue won't happen within 50 years
- it's recommended to specified a ctx to get better performance
*/
tt_inline tt_s64_t tt_time_ref()
{
    return tt_time_ref_ntv();
}

tt_inline tt_s64_t tt_time_ref2ms(IN tt_s64_t ref)
{
    return tt_time_ref2ms_ntv(ref);
}

tt_inline tt_s64_t tt_time_ms2ref(IN tt_s64_t ms)
{
    return tt_time_ms2ref_ntv(ms);
}

#endif /* __TT_TIME_REFERENCE__ */
