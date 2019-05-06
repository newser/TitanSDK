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

#include <tt/native/fcontext/fcontext_boost.h>

#include <tt/os/fiber.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

namespace tt::native {

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

void fctx_boost::jump(fiber_mgr &fb_mgr, fiber &from, fiber &to)
{
    // from : current fiber
    // to   : the fiber to be switched to
    // prev : the previous fiber when tt_jump_fcontext() returns

    // an example is: f1 -> f2 -> f3 -> f1, when f1 returns
    // from tt_jump_fcontext():
    //  from : f1, as f1 ever switched to f2
    //  to   : f2, as f1 ever switched to f2
    //  prev : f3, as f1 returns from f3

    fctx &fctx_to = to.fctx();
    fctx_to.from(from);
    assert(fctx_to.boost_handle() != nullptr);
    tt_transfer_t t = tt_jump_fcontext(fctx_to.boost_handle(), &to);

    fiber &prev = *from.fctx().from();
    if (prev.end()) {
        prev.remove();
        delete &prev;
    } else {
        // save prev's boost context data
        prev.fctx().boost_handle(t.fctx);
    }
}

void fctx_boost::routine(tt_transfer_t t)
{
    fiber &fb = *((fiber *)t.data);
    fiber_mgr &fb_mgr = fb.mgr();
    fiber &fb_main = fb_mgr.main();
    fctx &fctx_main = fb_main.fctx();

    // must save from's context
    fiber &from = *fb.fctx().from();
    from.fctx().boost_handle(t.fctx);

    fb.run();

    // must not use t.fctx, see a case:
    //  f1 -> f2
    //  f2 -> f3 ; now f2's t.fctx is stored in f1
    //  f3 -> f1
    //  f1 -> main ; f1 termiantes
    //  main -> f2
    //  f2 -> ? ; f2 terminates now, but t.fctx is invalid
    //
    // the simplest solution is jumping to main fiber as it's
    // always valid

    fb.end(true);
    fctx_main.from(fb);
    tt_jump_fcontext(fctx_main.boost_handle(), &fb_main);
}

}
