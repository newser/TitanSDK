/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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
#include <misc/tt_util.h>

#include <tt_sys_error.h>

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
        p = VirtualAllocExNuma(GetCurrentProcess(),
                               NULL,
                               size,
                               MEM_COMMIT | MEM_RESERVE,
                               PAGE_READWRITE,
                               tt_g_numa_node_id_memory);
    } else {
        p = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
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
        if (!VirtualFreeEx(GetCurrentProcess(), page_begin, 0, MEM_RELEASE)) {
            TT_ERROR_NTV("fail to free pages at %p", page_begin);
        }
    } else {
        if (!VirtualFree(page_begin, 0, MEM_RELEASE)) {
            TT_ERROR_NTV("fail to free pages at %p", page_begin);
        }
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
    void *reserve_addr = NULL;
    tt_u32_t reserve_size = 1 << (size_order + 1);
    // each time reserve twice virtual address as required, then there must
    // be an interval begin with some address aligned as required

    void *commit_addr = NULL;
    tt_u32_t commit_size = 1 << size_order;

    void *ret = NULL;

    // reserve

    if (tt_g_numa_node_id_memory != TT_NUMA_NODE_ID_UNSPECIFIED) {
        reserve_addr = VirtualAllocExNuma(GetCurrentProcess(),
                                          NULL,
                                          reserve_size,
                                          MEM_RESERVE,
                                          PAGE_NOACCESS,
                                          tt_g_numa_node_id_memory);
    } else {
        reserve_addr =
            VirtualAlloc(NULL, reserve_size, MEM_RESERVE, PAGE_NOACCESS);
    }
    if (reserve_addr == NULL) {
        TT_ERROR_NTV("fail to reserve pages");
        return NULL;
    }

    // commit

    commit_addr = reserve_addr;
    TT_PTR_ALIGN_INC(commit_addr, size_order);

    if (tt_g_numa_node_id_memory != TT_NUMA_NODE_ID_UNSPECIFIED) {
        ret = VirtualAllocExNuma(GetCurrentProcess(),
                                 commit_addr,
                                 commit_size,
                                 MEM_COMMIT,
                                 PAGE_READWRITE,
                                 tt_g_numa_node_id_memory);
    } else {
        ret =
            VirtualAlloc(commit_addr, commit_size, MEM_COMMIT, PAGE_READWRITE);
    }
    if ((ret == NULL) || (ret != commit_addr)) {
        if (tt_g_numa_node_id_memory != TT_NUMA_NODE_ID_UNSPECIFIED) {
            VirtualFreeEx(GetCurrentProcess(), reserve_addr, 0, MEM_RELEASE);
        } else {
            VirtualFree(reserve_addr, 0, MEM_RELEASE);
        }

        TT_ERROR_NTV("fail to commit pages");
        return NULL;
    }

    // all done
    *handle = (tt_uintptr_t)reserve_addr;
    return ret;
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
    if (tt_g_numa_node_id_memory != TT_NUMA_NODE_ID_UNSPECIFIED) {
        VirtualFreeEx(GetCurrentProcess(), (void *)handle, 0, MEM_RELEASE);
    } else {
        VirtualFree((void *)handle, 0, MEM_RELEASE);
    }
}

#endif // __TT_PAGE_OS_NATIVE__
