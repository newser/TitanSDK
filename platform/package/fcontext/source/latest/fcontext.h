
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __FCONTEXT__
#define __FCONTEXT__

#include <stddef.h>

typedef void *fcontext_t;

typedef struct
{
    fcontext_t fctx;
    void *data;
} transfer_t;

transfer_t jump_fcontext(fcontext_t const to, void *vp);

fcontext_t make_fcontext(void *sp, size_t size, void (*fn)(transfer_t));

#endif // __FCONTEXT__
