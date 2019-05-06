/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/os/fiber.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

namespace tt {

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

void fiber::internal_yield(fiber &cur, bool suspend_cur)
{
    fiber_mgr &fb_mgr = cur.mgr();
    // fiber &cur = fb_mgr.current_fiber();

    assert(cur.can_yield());

    if (!fb_mgr.is_main(cur)) {
        cur.remove();
        fb_mgr.push_tail(!suspend_cur, cur);
    }

    fiber &next = fb_mgr.next();
    if (next != cur) {
        fb_mgr.current_fiber(next);
        native::fctx::jump(fb_mgr, cur, next);
        fb_mgr.current_fiber(cur);
    }
}

void fiber::internal_resume(fiber &cur, fiber &new_fb, bool suspend_cur)
{
    fiber_mgr &fb_mgr = cur.mgr();

    if (cur == new_fb) { return; }

    if (!fb_mgr.is_main(cur)) {
        cur.remove();
        fb_mgr.push_tail(!suspend_cur, cur);
    }

    if (!fb_mgr.is_main(new_fb)) {
        new_fb.remove();
        fb_mgr.push_head(true, new_fb);
    }

    fb_mgr.current_fiber(new_fb);
    native::fctx::jump(fb_mgr, cur, new_fb);
    fb_mgr.current_fiber(cur);
}

fiber *fiber_mgr::find(const char *name) const
{
    static_assert(std::is_standard_layout<fiber>::value);

    if (name[0] == 0) { return nullptr; }

    for (lnode *n = active_.head(); n != nullptr; n = n->next()) {
        if (fiber *fb = TT_CONTAINER(n, fiber, node);
            fb->name() != nullptr && strcmp(fb->name(), name) == 0) {
            return fb;
        }
    }

    for (lnode *n = pending_.head(); n != nullptr; n = n->next()) {
        if (fiber *fb = TT_CONTAINER(n, fiber, node);
            fb->name() != nullptr && strcmp(fb->name(), name) == 0) {
            return fb;
        }
    }

    return nullptr;
}

}
