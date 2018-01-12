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

#include <tt_process_native.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <os/tt_process.h>

#include <tt_sys_error.h>
#include <tt_wchar.h>

#include <psapi.h>

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

static tt_char_t tt_s_process_name[256];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_process_create_ntv(IN tt_process_ntv_t *sys_proc,
                                  IN const tt_char_t *file,
                                  IN OPT tt_char_t *const argv[],
                                  IN OPT struct tt_process_attr_s *attr)
{
    wchar_t *wc_file = NULL;
    wchar_t *lpCommandLine = NULL;
    tt_result_t result = TT_FAIL;

    DWORD dwCreationFlags = 0;
    STARTUPINFOW si;

    wc_file = tt_wchar_create(file, 0, NULL);
    if (wc_file == NULL) {
        TT_ERROR("no mem for converting file name");
        goto __pc_out;
    }

    do {
        tt_u32_t i, len, utf8_len;
        tt_char_t *arg;
        tt_char_t *utf8_cli;

        if (argv == NULL) {
            break;
        }

        // utf8 length
        i = 0;
        len = 0;
        while ((arg = argv[i++]) != NULL) {
            len += (tt_u32_t)strlen(arg) + 1; // seperating space
        }
        if (len == 0) {
            break;
        }

        // utf8 string
        utf8_len = len + 1;
        utf8_cli = (tt_char_t *)tt_malloc(utf8_len);
        if (utf8_cli == NULL) {
            TT_ERROR("no mem for utf8 cmd line");
            goto __pc_out;
        }
        i = 0;
        len = 0;
        while ((arg = argv[i++]) != NULL) {
            tt_u32_t l = (tt_u32_t)strlen(arg);
            memcpy(utf8_cli + len, arg, l);
            len += l;
            utf8_cli[len++] = ' '; // seperating space
        }
        TT_ASSERT(len < utf8_len);
        utf8_cli[len++] = 0; // terminating null

        // to wchar string
        lpCommandLine = tt_wchar_create(utf8_cli, 0, NULL);
        tt_free(utf8_cli);
        if (lpCommandLine == NULL) {
            TT_ERROR("no mem for process cmd line");
            goto __pc_out;
        }
    } while (0);

    tt_memset(&si, 0, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    // debug
    dwCreationFlags |= CREATE_NEW_CONSOLE;

    if (!CreateProcessW(wc_file,
                        lpCommandLine,
                        NULL,
                        NULL,
                        FALSE, // no inherit handles
                        dwCreationFlags,
                        NULL,
                        NULL,
                        &si,
                        &sys_proc->proc_info)) {
        TT_ERROR_NTV("create process failed");
        goto __pc_out;
    }

    result = TT_SUCCESS;

__pc_out:

    if (lpCommandLine != NULL) {
        tt_wchar_destroy(lpCommandLine);
    }

    if (wc_file != NULL) {
        tt_wchar_destroy(wc_file);
    }

    return result;
}

tt_result_t tt_process_wait_ntv(IN tt_process_ntv_t *sys_proc,
                                IN tt_bool_t block,
                                IN OPT tt_u8_t *exit_code)
{
    DWORD ret = WaitForSingleObject(sys_proc->proc_info.hProcess,
                                    TT_COND(block, INFINITE, 0));
    if (ret == WAIT_OBJECT_0) {
        DWORD ExitCode = 0;
        if (GetExitCodeProcess(sys_proc->proc_info.hProcess, &ExitCode)) {
            TT_SAFE_ASSIGN(exit_code, (tt_u8_t)ExitCode);
        } else {
            TT_SAFE_ASSIGN(exit_code, TT_PROCESS_EXIT_UNKNOWN);
        }

        CloseHandle(sys_proc->proc_info.hProcess);
        CloseHandle(sys_proc->proc_info.hThread);
        return TT_SUCCESS;
    } else if (ret == WAIT_TIMEOUT) {
        return TT_E_TIMEOUT;
    } else {
        TT_ERROR_NTV("wait process failed");
        return TT_FAIL;
    }
}

void tt_process_exit_ntv(IN tt_u8_t exit_code)
{
    ExitProcess((UINT)exit_code);
}

tt_char_t *tt_process_path_ntv(IN OPT tt_process_ntv_t *sys_proc)
{
    HANDLE hProcess;
    tt_u32_t nc;
    wchar_t wc_path[1024] = {0};
    tt_char_t *path;

    if (sys_proc != NULL) {
        hProcess = sys_proc->proc_info.hProcess;
    } else {
        hProcess = GetCurrentProcess();
    }

    nc = (tt_u32_t)sizeof(wc_path) / sizeof(wchar_t);
    if (!QueryFullProcessImageNameW(hProcess, 0, wc_path, &nc)) {
        TT_ERROR_NTV("fail to get process name");
        return NULL;
    }
    // wc_path returned is null-terminated

    nc = WideCharToMultiByte(CP_UTF8, 0, wc_path, -1, NULL, 0, NULL, NULL);
    if (nc == 0) {
        TT_ERROR_NTV("fail to convert to utf8 path");
        return NULL;
    }
    // nc includes the terminating 0 if using -1

    path = tt_malloc(nc);
    if (path == NULL) {
        TT_ERROR_NTV("no mem for process path");
        return NULL;
    }

    WideCharToMultiByte(CP_UTF8, 0, wc_path, -1, path, nc, NULL, NULL);
    // path is null-terminated when using -1
    return path;
}

tt_char_t *tt_current_path_ntv(IN tt_bool_t end_slash)
{
    DWORD len;
    char *d;

    len = GetCurrentDirectoryA(0, NULL);
    if (len == 0) {
        TT_ERROR_NTV("fail to get current directory length");
        return NULL;
    }

    d = tt_malloc(len + 2);
    if (d == NULL) {
        TT_ERROR("no mem for current directory");
        return NULL;
    }

    len = GetCurrentDirectoryA(len + 2, d);
    if (len == 0) {
        TT_ERROR_NTV("fail to get current directory");
        tt_free(d);
        return NULL;
    }
    if (end_slash && (len > 0) && (d[len - 1] != '\\')) {
        d[len] = '\\';
        d[len + 1] = 0;
    }

    return d;
}

tt_result_t tt_process_name_ntv(IN tt_char_t *name, IN tt_u32_t len)
{
    wchar_t wpath[MAX_PATH + 1] = {0};
    char *upath, *p;
    tt_u32_t n;

    if (GetProcessImageFileNameW(GetCurrentProcess(), wpath, MAX_PATH) == 0) {
        TT_ERROR_NTV("fail to get process name");
        return TT_FAIL;
    }

    upath = tt_utf8_create(wpath, 0, NULL);
    if (upath == NULL) {
        return TT_FAIL;
    }

    p = tt_strrchr(upath, '\\');
    if (p != NULL) {
        p += 1;
    } else {
        p = upath;
    }

    n = (tt_u32_t)tt_strlen(p);
    if (n >= len) {
        TT_WARN("process name may be truncated");
        n = len - 1;
    }

    tt_memcpy(name, p, n);
    name[n] = 0;

    tt_free(upath);
    return TT_SUCCESS;
}
