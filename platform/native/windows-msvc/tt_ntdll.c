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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_ntdll.h>

#include <io/tt_file_system.h>
#include <io/tt_ipc.h>
#include <io/tt_socket.h>
#include <io/tt_socket_addr.h>

#include <tt_cstd_api.h>

#include <winternl.h>
#include <ws2ipdef.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __MAX_QSI_SIZE (1 << 27) // 128M

#ifndef NT_SUCCESS
#define NT_SUCCESS(x) ((x) >= 0)
#endif

#ifndef STATUS_INFO_LENGTH_MISMATCH
#define STATUS_INFO_LENGTH_MISMATCH 0xc0000004
#endif

#ifndef SystemHandleInformation
#define SystemHandleInformation 16
#endif

#ifndef ObjectBasicInformation
#define ObjectBasicInformation 0
#endif

#ifndef ObjectNameInformation
#define ObjectNameInformation 1
#endif

#ifndef ObjectTypeInformation
#define ObjectTypeInformation 2
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef NTSTATUS(NTAPI *pNtQSI_t)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation,
    ULONG SystemInformationLength, PULONG ReturnLength);

typedef NTSTATUS(NTAPI *pNtQO_t)(HANDLE ObjectHandle,
                                 ULONG ObjectInformationClass,
                                 PVOID ObjectInformation,
                                 ULONG ObjectInformationLength,
                                 PULONG ReturnLength);

typedef struct _SYSTEM_HANDLE
{
    ULONG ProcessId;
    BYTE ObjectTypeNumber;
    BYTE Flags;
    USHORT Handle;
    PVOID Object;
    ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG HandleCount;
    SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef enum _POOL_TYPE
{
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS
} POOL_TYPE,
    *PPOOL_TYPE;

typedef struct _OBJECT_TYPE_INFORMATION
{
    UNICODE_STRING Name;
    ULONG TotalNumberOfObjects;
    ULONG TotalNumberOfHandles;
    ULONG TotalPagedPoolUsage;
    ULONG TotalNonPagedPoolUsage;
    ULONG TotalNamePoolUsage;
    ULONG TotalHandleTableUsage;
    ULONG HighWaterNumberOfObjects;
    ULONG HighWaterNumberOfHandles;
    ULONG HighWaterPagedPoolUsage;
    ULONG HighWaterNonPagedPoolUsage;
    ULONG HighWaterNamePoolUsage;
    ULONG HighWaterHandleTableUsage;
    ULONG InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    ULONG ValidAccess;
    BOOLEAN SecurityRequired;
    BOOLEAN MaintainHandleCount;
    USHORT MaintainTypeList;
    POOL_TYPE PoolType;
    ULONG PagedPoolUsage;
    ULONG NonPagedPoolUsage;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static ULONG s_qsi_size;

static HMODULE s_mod_ntdll;

static pNtQSI_t s_pNtQSI;

static pNtQO_t s_pNtQO;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_ntdll_component_init_ntv()
{
    s_qsi_size = 1 << 16;

    s_mod_ntdll = LoadLibraryW(L"ntdll.dll");
    if (s_mod_ntdll == NULL) {
        printf("fail to load ntdll.dll\n");
        goto out;
    }

    s_pNtQSI =
        (pNtQSI_t)GetProcAddress(s_mod_ntdll, "NtQuerySystemInformation");
    if (s_pNtQSI == NULL) {
        printf("fail to get NtQuerySystemInformation()\n");
    }

    s_pNtQO = (pNtQO_t)GetProcAddress(s_mod_ntdll, "NtQueryObject");
    if (s_pNtQO == NULL) { printf("fail to get NtQueryObject()\n"); }

out:
    return TT_SUCCESS;
}

void tt_ntdll_component_exit_ntv()
{
    if (s_mod_ntdll != NULL) { FreeLibrary(s_mod_ntdll); }
}

void tt_ntdll_dump_fs(IN tt_u32_t flag)
{
    PSYSTEM_HANDLE_INFORMATION hinfo = NULL;
    ULONG size, i;
    NTSTATUS status;
    ULONG pid;

    if (s_pNtQSI == NULL) {
        printf("no NtQuerySystemInformation()\n");
        return;
    }

    do {
        s_qsi_size <<= 1;

        if (hinfo != NULL) { free(hinfo); }
        size = s_qsi_size;
        hinfo = malloc(size);
        if (hinfo == NULL) {
            printf("no memory for NtQuerySystemInformation\n");
            return;
        }

        status = s_pNtQSI(SystemHandleInformation, hinfo, size, NULL);
    } while ((status == STATUS_INFO_LENGTH_MISMATCH) &&
             (s_qsi_size <= __MAX_QSI_SIZE));
    if (!NT_SUCCESS(status)) {
        printf("NtQuerySystemInformation failed\n");
        free(hinfo);
        return;
    }

    pid = GetCurrentProcessId();
    for (i = 0; i < hinfo->HandleCount; i++) {
        PSYSTEM_HANDLE sh = &hinfo->Handles[i];
        HANDLE h = (HANDLE)(sh->Handle);
        wchar_t buf[1 << 12], *name;
        DWORD len = (sizeof(buf) / sizeof(wchar_t)) - 1;

        if (sh->ProcessId != pid) { continue; }

        if (GetFileType(h) != FILE_TYPE_DISK) { continue; }

        len = GetFinalPathNameByHandleW(h, buf, len, 0);
        if (len == 0) {
            name = L"?";
        } else if (len < ((sizeof(buf) / sizeof(wchar_t)))) {
            buf[len] = L'\0';
            name = buf;
        } else {
            name = L"?";
        }

        if (wcsncmp(name, L"\\\\?\\UNC\\", 8) == 0) {
            name += 6;
        } else if (wcsncmp(buf, L"\\\\?\\", 4) == 0) {
            name += 4;
        }

        tt_printf("%s[handle: 0x%x] [%S]\n",
                  TT_COND(flag & TT_FS_STATUS_PREFIX, "<<FS>> ", ""), h, name);
    }

    free(hinfo);
}

void tt_ntdll_dump_ipc(IN tt_u32_t flag)
{
    PSYSTEM_HANDLE_INFORMATION hinfo = NULL;
    ULONG size, i;
    NTSTATUS status;
    ULONG pid;

    if (s_pNtQSI == NULL) {
        printf("no NtQuerySystemInformation()\n");
        return;
    }

    do {
        s_qsi_size <<= 1;

        if (hinfo != NULL) { free(hinfo); }
        size = s_qsi_size;
        hinfo = malloc(size);
        if (hinfo == NULL) {
            printf("no memory for NtQuerySystemInformation\n");
            return;
        }

        status = s_pNtQSI(SystemHandleInformation, hinfo, size, NULL);
    } while ((status == STATUS_INFO_LENGTH_MISMATCH) &&
             (s_qsi_size <= __MAX_QSI_SIZE));
    if (!NT_SUCCESS(status)) {
        printf("NtQuerySystemInformation failed\n");
        free(hinfo);
        return;
    }

    pid = GetCurrentProcessId();
    for (i = 0; i < hinfo->HandleCount; i++) {
        PSYSTEM_HANDLE sh = &hinfo->Handles[i];
        HANDLE h = (HANDLE)(sh->Handle);
        tt_u8_t buf[1024];
        FILE_NAME_INFO *info = (FILE_NAME_INFO *)buf;
        wchar_t *name;

        if (sh->ProcessId != pid) { continue; }

        if ((GetFileType(h) != FILE_TYPE_PIPE) ||
            !GetNamedPipeInfo(h, NULL, NULL, NULL, NULL)) {
            // not named pipe, may be a socket
            continue;
        }

        if (GetFileInformationByHandleEx(h, FileNameInfo, info, sizeof(buf)) &&
            ((tt_u8_t *)&info->FileName[info->FileNameLength] <
             (buf + sizeof(buf) - sizeof(wchar_t)))) {
            name = info->FileName;
            name[info->FileNameLength / sizeof(wchar_t)] = L'\0';
        } else {
            name = L"?";
        }

        tt_printf("%s[handle: 0x%x] %S\n",
                  TT_COND(flag & TT_IPC_STATUS_PREFIX, "<<IPC>> ", ""), h,
                  name);
    }

    free(hinfo);
}

void tt_ntdll_dump_skt(IN tt_u32_t flag)
{
    PSYSTEM_HANDLE_INFORMATION hinfo = NULL;
    ULONG size, i;
    NTSTATUS status;
    ULONG pid;

    if (s_pNtQSI == NULL) {
        printf("no NtQuerySystemInformation()\n");
        return;
    }

    do {
        s_qsi_size <<= 1;

        if (hinfo != NULL) { free(hinfo); }
        size = s_qsi_size;
        hinfo = malloc(size);
        if (hinfo == NULL) {
            printf("no memory for NtQuerySystemInformation\n");
            return;
        }

        status = s_pNtQSI(SystemHandleInformation, hinfo, size, NULL);
    } while ((status == STATUS_INFO_LENGTH_MISMATCH) &&
             (s_qsi_size <= __MAX_QSI_SIZE));
    if (!NT_SUCCESS(status)) {
        printf("NtQuerySystemInformation failed\n");
        free(hinfo);
        return;
    }

    pid = GetCurrentProcessId();
    for (i = 0; i < hinfo->HandleCount; i++) {
        PSYSTEM_HANDLE sh = &hinfo->Handles[i];
        HANDLE h = (HANDLE)(sh->Handle);
        int val;
        int len = sizeof(int);

        if (sh->ProcessId != pid) { continue; }

        if ((GetFileType(h) != FILE_TYPE_PIPE) ||
            GetNamedPipeInfo(h, NULL, NULL, NULL, NULL)) {
            // it's a pipe
            continue;
        }

        if (getsockopt((SOCKET)h, SOL_SOCKET, SO_TYPE, (char *)&val, &len) !=
            0) {
            tt_printf("%s[handle: 0x%x] ?\n",
                      TT_COND(flag & TT_SKT_STATUS_PREFIX, "<<Socket>> ", ""),
                      h);
        } else if (val == SOCK_STREAM) {
            struct sockaddr_storage a;
            int alen = sizeof(a);
            char local[128] = {0};
            char remote[128] = {0};

            if (getsockname((SOCKET)h, (struct sockaddr *)&a, &alen) != 0) {
                tt_snprintf(local, sizeof(local) - 1, "?@?");
            } else if (a.ss_family == AF_INET) {
                struct sockaddr_in *a4 = (struct sockaddr_in *)&a;
                tt_sktaddr_ip_t ip;
                char addr[64] = {0};

                ip.a32.__u32 = a4->sin_addr.s_addr;
                tt_sktaddr_ip_n2p(TT_NET_AF_INET, &ip, addr, sizeof(addr) - 1);
                tt_snprintf(local, sizeof(local) - 1, "%s@%d", addr,
                            ntohs(a4->sin_port));
            } else if (a.ss_family == AF_INET6) {
                struct sockaddr_in6 *a6 = (struct sockaddr_in6 *)&a;
                tt_sktaddr_ip_t ip;
                char addr[64] = {0};

                tt_memcpy(ip.a128.__u8, a6->sin6_addr.s6_addr, 16);
                tt_sktaddr_ip_n2p(TT_NET_AF_INET6, &ip, addr, sizeof(addr) - 1);
                tt_snprintf(local, sizeof(local) - 1, "%s@%d", addr,
                            ntohs(a6->sin6_port));
            } else {
                tt_snprintf(local, sizeof(local) - 1, "?@?");
            }

            if (getpeername((SOCKET)h, (struct sockaddr *)&a, &alen) != 0) {
                tt_snprintf(remote, sizeof(remote) - 1, "?@?");
            } else if (a.ss_family == AF_INET) {
                struct sockaddr_in *a4 = (struct sockaddr_in *)&a;
                tt_sktaddr_ip_t ip;
                char addr[64] = {0};

                ip.a32.__u32 = a4->sin_addr.s_addr;
                tt_sktaddr_ip_n2p(TT_NET_AF_INET, &ip, addr, sizeof(addr) - 1);
                tt_snprintf(remote, sizeof(remote) - 1, "%s@%d", addr,
                            ntohs(a4->sin_port));
            } else if (a.ss_family == AF_INET6) {
                struct sockaddr_in6 *a6 = (struct sockaddr_in6 *)&a;
                tt_sktaddr_ip_t ip;
                char addr[64] = {0};

                tt_memcpy(ip.a128.__u8, a6->sin6_addr.s6_addr, 16);
                tt_sktaddr_ip_n2p(TT_NET_AF_INET6, &ip, addr, sizeof(addr) - 1);
                tt_snprintf(remote, sizeof(remote) - 1, "%s@%d", addr,
                            ntohs(a6->sin6_port));
            } else {
                tt_snprintf(remote, sizeof(local) - 1, "?@?");
            }

            tt_printf("%s[handle: 0x%x] tcp [%s --> %s]\n",
                      TT_COND(flag & TT_SKT_STATUS_PREFIX, "<<Socket>> ", ""),
                      h, local, remote);
        } else if (val == SOCK_DGRAM) {
            struct sockaddr_storage a;
            int alen = sizeof(a);
            char local[128] = {0};

            if (getsockname((SOCKET)h, (struct sockaddr *)&a, &alen) != 0) {
                tt_snprintf(local, sizeof(local) - 1, "?@?");
            } else if (a.ss_family == AF_INET) {
                struct sockaddr_in *a4 = (struct sockaddr_in *)&a;
                tt_sktaddr_ip_t ip;
                char addr[64] = {0};

                ip.a32.__u32 = a4->sin_addr.s_addr;
                tt_sktaddr_ip_n2p(TT_NET_AF_INET, &ip, addr, sizeof(addr) - 1);
                tt_snprintf(local, sizeof(local) - 1, "%s@%d", addr,
                            ntohs(a4->sin_port));
            } else if (a.ss_family == AF_INET6) {
                struct sockaddr_in6 *a6 = (struct sockaddr_in6 *)&a;
                tt_sktaddr_ip_t ip;
                char addr[64] = {0};

                tt_memcpy(ip.a128.__u8, a6->sin6_addr.s6_addr, 16);
                tt_sktaddr_ip_n2p(TT_NET_AF_INET6, &ip, addr, sizeof(addr) - 1);
                tt_snprintf(local, sizeof(local) - 1, "%s@%d", addr,
                            ntohs(a6->sin6_port));
            } else {
                tt_snprintf(local, sizeof(local) - 1, "?@?");
            }

            tt_printf("%s[handle: 0x%x] udp [%s]\n",
                      TT_COND(flag & TT_SKT_STATUS_PREFIX, "<<Socket>> ", ""),
                      h, local);
        }
    }

    free(hinfo);
}
