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

/**
@file tt_double_linked_list.h
@brief doulbe linked dl
this file defines apis of doulbe linked dl data structure.

 - push_tail/push_head/pop_tail/pop_head
 - head/tail
 - insert_front/insert_back
 - move
 - empty
 - clear
 - remove(n)

*/

#ifndef __TT_DOUBLE_LINKED_LIST_CPP__
#define __TT_DOUBLE_LINKED_LIST_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/misc/util.h>

#include <cassert>
#include <utility>

namespace tt {

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

class dlist;

class dnode
{
    friend class dlist;

public:
    dnode() = default;

    dnode *prev() const { return prev_; };
    dnode *next() const { return next_; };

    bool in_list() const { return (prev_ != NULL) || (next_ != NULL); }

private:
    void clear()
    {
        prev_ = nullptr;
        next_ = nullptr;
    }

    dnode *prev_{nullptr};
    dnode *next_{nullptr};

    TT_NON_COPYABLE(dnode)
};

class dlist
{
    friend class dnode;

public:
    dlist() = default;

    size_t count() const;
    bool empty() const { return head_ == NULL; }
    bool contain(const dnode &n) const;

    dnode *head() const { return head_; }
    dnode *tail() const { return tail_; }
    void push_head(dnode &n);
    void push_tail(dnode &n);
    dnode *pop_head();
    dnode *pop_tail();

    void clear()
    {
        while (pop_head() != nullptr)
            ;
    }

    void insert_front(dnode &pos, dnode &n);
    void insert_back(dnode &pos, dnode &n);
    dnode *remove(dnode &n);

    void move(dlist &src);
    void swap(dlist &l)
    {
        std::swap(head_, l.head_);
        std::swap(tail_, l.tail_);
    }

private:
    dnode *head_{nullptr};
    dnode *tail_{nullptr};

    TT_NON_COPYABLE(dlist)
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

#endif /* __TT_DOUBLE_LINKED_LIST_CPP__ */
