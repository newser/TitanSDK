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

#include <network/dns/tt_dns.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <misc/tt_assert.h>
#include <memory/tt_memory_alloc.h>

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

static tt_result_t __dns_component_init(IN tt_component_t *comp,
                                         IN tt_profile_t *profile);

static void *__dns_malloc(size_t s);
static void __dns_free(void *p);
static void *__dns_realloc(void *p, size_t s);

static void __dns_attr2option(IN tt_dns_attr_t *attr,
                              IN struct ares_options *options,
                              IN int *optmask);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_dns_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __dns_component_init,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_MODULE_DNS,
                      "DNS",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

tt_dns_t tt_dns_create(IN OPT tt_dns_attr_t *attr)
{
    tt_dns_attr_t __attr;
    ares_channel p;
    struct ares_options options;
    int optmask;
    int e;
    
    if (attr == NULL) {
        tt_dns_attr_default(&__attr);
        attr = &__attr;
    }
    
    __dns_attr2option(attr, &options, &optmask);
    e = ares_init_options(&p, &options, optmask);
    if (e != ARES_SUCCESS) {
        TT_ERROR("fail to create dns: %s", ares_strerror(e));
        return NULL;
    }
    
    return p;
}

void tt_dns_destroy(IN tt_dns_t d)
{
    TT_ASSERT(d != NULL);
    
    ares_destroy(d);
}

void tt_dns_attr_default(IN tt_dns_attr_t *attr)
{
    TT_ASSERT(attr != NULL);
    
    
}

tt_result_t __dns_component_init(IN tt_component_t *comp,
                                  IN tt_profile_t *profile)
{
    int flags = 0;
    int e;
    
#if TT_ENV_OS_IS_WINDOWS
    flags |= ARES_LIB_INIT_WIN32;
#endif
    e = ares_library_init_mem(flags,
                              __dns_malloc,
                              __dns_free,
                              __dns_realloc);
    if (e != ARES_SUCCESS) {
        TT_ERROR("fail to ini ares: %s", ares_strerror(e));
        return TT_FAIL;
    }
    
    return TT_SUCCESS;
}

void *__dns_malloc(size_t s)
{
    return tt_malloc(s);
}

void __dns_free(void *p)
{
    tt_free(p);
}

void *__dns_realloc(void *p, size_t s)
{
    return tt_realloc(p, s);
}

void __dns_attr2option(IN tt_dns_attr_t *attr,
                       IN struct ares_options *options,
                       IN int *optmask)
{
    
}
