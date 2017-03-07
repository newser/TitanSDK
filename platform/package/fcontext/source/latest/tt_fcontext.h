
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __TT_FCONTEXT__
#define __TT_FCONTEXT__

#include <stddef.h>

typedef void *tt_fcontext_t;

typedef struct
{
    tt_fcontext_t fctx;
    void *data;
} tt_transfer_t;

tt_transfer_t tt_jump_fcontext(tt_fcontext_t const to, void *vp);

tt_fcontext_t tt_make_fcontext(void *sp,
                               size_t size,
                               void (*fn)(tt_transfer_t));

#endif // __TT_FCONTEXT__
