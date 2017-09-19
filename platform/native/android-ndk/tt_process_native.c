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
#include <misc/tt_util.h>
#include <os/tt_process.h>

#include <tt_sys_error.h>

#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

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
                                  IN const tt_char_t *path,
                                  IN OPT tt_char_t *const arg[],
                                  IN OPT struct tt_process_attr_s *attr)
{
    pid_t pid = fork();

    if (pid < 0) {
        TT_ERROR_NTV("fork failed");
        return TT_FAIL;
    }

    if (pid > 0) {
        // parent process
        sys_proc->pid = pid;
        return TT_SUCCESS;
    }

    // child process
    execv(path, arg);

    // exec returned means error occured
    TT_ERROR_NTV("exec[%s] failed", path);
    _exit(EXIT_FAILURE);
    return TT_FAIL;
}

tt_result_t tt_process_wait_ntv(IN tt_process_ntv_t *sys_proc,
                                IN tt_bool_t block,
                                IN OPT tt_u8_t *exit_code)
{
    int ret;
    int status = 0;
    int options = 0;

    if (!block) {
        options |= WNOHANG;
    }

__wait_ag:
    ret = waitpid(sys_proc->pid, &status, options);
    if (ret > 0) {
        if (WIFEXITED(status)) {
            TT_SAFE_ASSIGN(exit_code, (tt_u8_t)WEXITSTATUS(status));
        } else {
            TT_SAFE_ASSIGN(exit_code, TT_PROCESS_EXIT_UNKNOWN);
        }
        return TT_SUCCESS;
    } else if (ret == 0) {
        return TT_TIME_OUT;
    }

    // failed
    if (errno == EINTR) {
        goto __wait_ag;
    }
    TT_ERROR_NTV("wait process failed");
    return TT_FAIL;
}

void tt_process_exit_ntv(IN tt_u8_t exit_code)
{
    exit((int)exit_code);
}

tt_char_t *tt_process_path_ntv(IN OPT tt_process_ntv_t *sys_proc)
{
    pid_t pid;
    char link_path[32];
    char link[1024];
    ssize_t len;
    tt_char_t *path;

    pid = getpid();
    sprintf(link_path, "/proc/%d/exe", pid);
    len = readlink(link_path, link, sizeof(link) - 1);
    if (len <= 0) {
        TT_ERROR_NTV("fail to read %s", link_path);
        return NULL;
    }

    path = tt_malloc(len + 1);
    if (path == NULL) {
        TT_ERROR("no mem for path");
        return NULL;
    }
    tt_memcpy(path, link, len);
    path[len] = 0;

    return path;
}
