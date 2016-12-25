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

#include <tt_process_native.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <os/tt_process.h>

#include <tt_sys_error.h>
#include <tt_wchar.h>

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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_process_create_ntv(IN tt_process_ntv_t *sys_proc,
                                  IN const tt_char_t *file,
                                  IN OPT tt_char_t *const argv[],
                                  IN OPT struct tt_process_attr_s *attr)
{
    wchar_t *w_file = NULL;
    wchar_t *lpCommandLine = NULL;
    tt_result_t result = TT_FAIL;

    DWORD dwCreationFlags = 0;
    STARTUPINFO si;

    // convert app name
    w_file = tt_wchar_create(file, NULL);
    if (w_file == NULL) {
        TT_ERROR("no mem for converting file name");
        goto __pc_out;
    }

    // construct command line
    do {
        tt_u32_t i, len, utf8_len;
        tt_char_t *cur_arg;
        tt_char_t *utf8_cli;

        if (argv == NULL) {
            break;
        }

        // utf8 length
        i = 0;
        len = 0;
        while ((cur_arg = argv[i++]) != NULL) {
            len += (tt_u32_t)strlen(cur_arg) + 1; // seperating space
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
        while ((cur_arg = argv[i++]) != NULL) {
            tt_u32_t l = (tt_u32_t)strlen(cur_arg);
            memcpy(utf8_cli + len, cur_arg, l);
            len += l;
            utf8_cli[len++] = ' '; // seperating space
        }
        TT_ASSERT(len < utf8_len);
        utf8_cli[len++] = 0; // terminating null

        // to wchar string
        lpCommandLine = tt_wchar_create(utf8_cli, NULL);
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

    if (!CreateProcessW(w_file,
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

    if (w_file != NULL) {
        tt_wchar_destroy(w_file);
    }

    return result;
}

tt_result_t tt_process_wait_ntv(IN tt_process_ntv_t *sys_proc,
                                IN tt_bool_t block,
                                IN OPT tt_u8_t *exit_code)
{
    DWORD dwMilliseconds;
    DWORD ret;

    if (block) {
        dwMilliseconds = INFINITE;
    } else {
        dwMilliseconds = 0;
    }

    ret = WaitForSingleObject(sys_proc->proc_info.hProcess, dwMilliseconds);
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
        return TT_TIME_OUT;
    } else {
        TT_ERROR_NTV("wait process failed");
        return TT_FAIL;
    }
}

void tt_process_exit_ntv(IN tt_u8_t exit_code)
{
    ExitProcess((UINT)exit_code);
}
