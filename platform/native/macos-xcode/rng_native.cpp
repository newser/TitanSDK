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

#include <rng_native.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

namespace tt {

namespace native {

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

//#define __RAND_DEV "/dev/random"
#define __RAND_DEV "/dev/urandom"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static int __rand_fd = -1;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

err rng_start()
{
    assert(__rand_fd == -1);
    __rand_fd = open(__RAND_DEV, O_RDONLY);
    if (__rand_fd < 0) { return err::e_fail; }

    return err::e_ok;
}

void rng_stop()
{
    close(__rand_fd);
    __rand_fd = -1;
}

err rng(void *addr, size_t size)
{
    uint8_t *data = (uint8_t *)addr;
    size_t n = 0;
    ssize_t ret;

ag:
    ret = read(__rand_fd, data + n, size - n);
    if (ret > 0) {
        n += ret;
        if (n < size) {
            goto ag;
        } else {
            return err::e_ok;
        }
    } else if (errno == EINTR) {
        goto ag;
    } else {
        return err::e_fail;
    }
}

}

}
