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
@file tt_page_os_native.h
@brief memory page APIs

memory page APIs
*/

#ifndef __TT_PAGE_OS_NATIVE__
#define __TT_PAGE_OS_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <init/tt_platform_info.h>
#include <log/tt_log.h>

#include <tt_sys_error.h>

#include <malloc.h>
#include <stdlib.h>
#include <sys/mman.h>

#ifdef TT_PLATFORM_NUMA_ENABLE
#include <numa.h>
#endif

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// allocate more virtual space to get an aligned address, this method
// is less likely fail but some virtual space are wasted
#define __APA_REDUNDANT 2
// use posix_memalign, does all platform support this function?
#define __APA_POSIX_MEMALIGN 3

#define __ALLOC_PAGE_ALIGN __APA_POSIX_MEMALIGN
//#define __ALLOC_PAGE_ALIGN __APA_REDUNDANT

// ========================================
// numa api switchers
// ========================================

#ifdef TT_PLATFORM_NUMA_ENABLE

#define __PAGE_ALLOC_ONNODE(size, numa_node_id_memory)                         \
    numa_alloc_onnode((size), (numa_node_id_memory))
#define __PAGE_FREE_ONNODE(page_begin, size, numa_node_id_memory)              \
    numa_free((page_begin), (size))

#define __PAGE_TONODE_MEMORY(p, size, numa_node_id_memory)                     \
    numa_tonode_memory((p), (size), (numa_node_id_memory))

#else

#define __PAGE_ALLOC_ONNODE(size, numa_node_id_memory)                         \
    mmap(NULL,                                                                 \
         (size),                                                               \
         PROT_READ | PROT_WRITE,                                               \
         MAP_PRIVATE | MAP_ANONYMOUS,                                          \
         -1,                                                                   \
         0)
#define __PAGE_FREE_ONNODE(page_begin, size, numa_node_id_memory)              \
    munmap((page_begin), (size))

#define __PAGE_TONODE_MEMORY(p, size, numa_node_id_memory)

#endif

#define __PAGE_ALLOC(size)                                                     \
    mmap(NULL, (size), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0)
#define __PAGE_FREE(page_begin, size) munmap((page_begin), (size))

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
@fn void *tt_page_alloc_ntv(IN tt_u32_t size)
allocate pages from a specific node

@param [in] size total size of required pages

@return
- beginning address of allocated pages
- NULL, otherwise

@note
- this function must handle numa binding
- size should be aligned previously or leading to unknown behavior
- this function must be thread safe, so if system call does not satify
  such condition, use an internal lock to protect it
- known API like VirtualAllocExNuma() or numa_alloc_onnode() are all
  thread safe
*/
tt_inline void *tt_page_alloc_ntv(IN tt_u32_t size)
{
    void *p = NULL;

    if (tt_g_numa_node_id_memory != TT_NUMA_NODE_ID_UNSPECIFIED) {
        p = __PAGE_ALLOC_ONNODE(size, tt_g_numa_node_id_memory);
    } else {
        p = __PAGE_ALLOC(size);
    }
    if (p == NULL) {
        TT_ERROR("fail to allocate pages");
    }

    return p;
}

/**
@fn void tt_page_free_ntv(IN void *page_begin,
                          IN tt_u32_t size)
free pages to a specific node

@param [in] page_begin begin address of pages
@param [in] size total size of pages

@note
- size should be aligned previously or leading to unknown behavior
- this function must be thread safe, so if system call does not satify
  such condition, use an internal lock to protect it
- known API like VirtualFree() or numa_free() are all thread safe
*/
tt_inline void tt_page_free_ntv(IN void *page_begin, IN tt_u32_t size)
{
    if (tt_g_numa_node_id_memory != TT_NUMA_NODE_ID_UNSPECIFIED) {
        __PAGE_FREE_ONNODE(page_begin, size, tt_g_numa_node_id_memory);
    } else {
        __PAGE_FREE(page_begin, size);
    }
}

/**
@fn void *tt_page_alloc_align_ntv(IN tt_u32_t size_order,
                                    OUT tt_uintptr_t *handle)
allocate pages from a specific node, the returned address is aligned with size

@param [in] size_order order of required size
@param [in] handle to be passed to free_aligned()

@return
- beginning address of allocated pages
- NULL, otherwise

@note
- this function must handle numa binding
- this function must be thread safe, so if system call does not satify
  such condition, use an internal lock to protect it
- known API like VirtualAllocExNuma() or mmap()+mbind() are all
  thread safe
*/
tt_inline void *tt_page_alloc_align_ntv(IN tt_u32_t size_order,
                                        OUT tt_uintptr_t *handle)
{
#if (__ALLOC_PAGE_ALIGN == __APA_REDUNDANT)

    void *reserve_addr = NULL;
    tt_u32_t reserve_size = 1 << (size_order + 1);
    // each time reserve twice virtual address as required, then there must
    // be an interval begin with some address aligned as required

    void *commit_addr = NULL;
    tt_u32_t commit_size = 1 << size_order;

    reserve_addr =
        mmap(NULL, reserve_size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (reserve_addr == NULL) {
        TT_ERROR_NTV("fail to reserve pages");
        return NULL;
    }

    commit_addr = reserve_addr;
    TT_PTR_ALIGN_INC(commit_addr, size_order);
    if (mprotect(commit_addr, commit_size, PROT_READ | PROT_WRITE) != 0) {
        TT_ERROR_NTV("fail to change pages to read/write");
        munmap(reserve_addr, reserve_size);

        return NULL;
    }

    // bind to numa node
    if (tt_g_numa_node_id_memory != TT_NUMA_NODE_ID_UNSPECIFIED) {
        __PAGE_TONODE_MEMORY(commit_addr,
                             commit_size,
                             tt_g_numa_node_id_memory);
    }

    // all done
    *handle = (tt_uintptr_t)reserve_addr;
    return commit_addr;

#elif (__ALLOC_PAGE_ALIGN == __APA_POSIX_MEMALIGN)

    void *p = NULL;
    tt_u32_t size = 1 << size_order;
    int ret = posix_memalign(&p, size, size);

    if ((ret != 0) || (p == NULL)) {
        TT_ERROR_NTV("fail to allocated aligned memory");
        return NULL;
    }

    // bind to node,
    if (tt_g_numa_node_id_memory != TT_NUMA_NODE_ID_UNSPECIFIED) {
        __PAGE_TONODE_MEMORY(p, size, tt_g_numa_node_id_memory);
    }

    // handle is not used
    *handle = (tt_uintptr_t)NULL;
    return p;

#else

#error how to allocate address aligned pages?

#endif
}

/**
@fn void tt_page_free_align_ntv(IN void *page_begin,
                                  IN tt_u32_t size_order,
                                  IN tt_uintptr_t handle)
free pages to a specific node

@param [in] page_begin begin address of pages
@param [in] size_order order of total size
@param [in] handle the handle returned when allocated

@note
- size should be aligned previously or leading to unknown behavior
- this function must be thread safe, so if system call does not satify
  such condition, use an internal lock to protect it
- known API like VirtualFree() or numa_free() are all thread safe
*/
tt_inline void tt_page_free_align_ntv(IN void *page_begin,
                                      IN tt_u32_t size_order,
                                      IN tt_uintptr_t handle)
{
#if (__ALLOC_PAGE_ALIGN == __APA_REDUNDANT)

    if (munmap((tt_ptr_t)handle, 1 << (size_order + 1)) != 0) {
        TT_ERROR_NTV("fail to release pages");
    }

#elif (__ALLOC_PAGE_ALIGN == __APA_POSIX_MEMALIGN)

    free(page_begin);

#else

#error how to free address aligned pages?

#endif
}

#endif // __TT_PAGE_OS_NATIVE__
