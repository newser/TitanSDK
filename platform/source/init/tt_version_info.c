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

#include <init/tt_version_info.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_char_t tt_g_version_cstr[60];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __ver_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_ver_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __ver_component_init,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_VERSION_INFO,
                      "Version Infomation",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

tt_ver_t tt_ver_major()
{
    return ((tt_ver_t)TT_VERSION_MAJOR);
}

tt_ver_t tt_ver_minor()
{
    return ((tt_ver_t)TT_VERSION_MINOR);
}

tt_ver_t tt_ver_revision()
{
    return ((tt_ver_t)TT_VERSION_REVISION);
}

const tt_char_t *tt_version_cstr()
{
    if (tt_g_version_cstr[0] == 0) {
        tt_ver_format(tt_g_version_cstr,
                      sizeof(tt_g_version_cstr),
                      TT_VER_FORMAT_FULL);
    }
    return tt_g_version_cstr;
}

void tt_ver_format(IN tt_char_t *buf, IN tt_u32_t buf_len, IN tt_u32_t how)
{
    TT_ASSERT(buf != NULL);
    TT_ASSERT(buf_len != 0);

    tt_memset(buf, 0, buf_len);

    switch (how) {
        case TT_VER_FORMAT_BASIC: {
            tt_snprintf(buf,
                        buf_len - 1,
                        "%u.%u",
                        tt_ver_major(),
                        tt_ver_minor());
        } break;
        case TT_VER_FORMAT_FULL: {
            tt_snprintf(buf,
                        buf_len - 1,
                        "%u.%u.%u(%s)",
                        tt_ver_major(),
                        tt_ver_minor(),
                        tt_ver_revision(),
                        TT_VERSION_BUILD);
        } break;
        case TT_VER_FORMAT_STANDARD:
        default: {
            tt_snprintf(buf,
                        buf_len - 1,
                        "%u.%u.%u",
                        tt_ver_major(),
                        tt_ver_minor(),
                        tt_ver_revision());
        } break;
    }
}

tt_result_t __ver_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    // version string will be initialized by tt_version_cstr()

    TT_INFO("\n----------------------------------------");
    TT_INFO("Initializing TitanSDK");
    TT_INFO("version: %s", tt_version_cstr());
    TT_INFO("----------------------------------------\n");
    return TT_SUCCESS;
}

tt_result_t tt_ver_require_major(IN tt_ver_t major)
{
    if (major != tt_ver_major()) {
        TT_ERROR("[Error] required major number: %d but: %d\n",
                 major,
                 tt_ver_major());
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_ver_require_minor(IN tt_ver_t minor)
{
    if (minor != tt_ver_minor()) {
        TT_ERROR("[Error] required minor number: %d, but: %d\n",
                 minor,
                 tt_ver_minor());
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_ver_require(IN tt_ver_t major, IN tt_ver_t minor)
{
    if (!TT_OK(tt_ver_require_major(major))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_ver_require_minor(minor))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
