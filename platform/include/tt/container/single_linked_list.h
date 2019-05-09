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
@file tt_single_linked_list.h
@brief simple sl
this file defines apis of simple sl data structure.

 - push_head/push_tail/pop_head/pop_tail
 - head/tail
 - insert_front/insert_back
 - empty
 - clear
 - remove(n)
 - swap
*/

#ifndef __TT_SINGLE_LINKED_LIST_CPP__
#define __TT_SINGLE_LINKED_LIST_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/misc/macro.h>

#include <cassert>
#include <utility>

namespace tt {

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

class slist;

class snode
{
    friend class slist;

public:
    snode() = default;

    snode *next() const { return next_; };

private:
    void clear() { next_ = nullptr; }

    snode *next_{nullptr};

    TT_NON_COPYABLE(snode)
};

class slist
{
    friend class snode;

public:
    slist() = default;

    size_t count() const;
    bool empty() const { return head_ == NULL; }
    bool contain(const snode &n) const;

    snode *head() const { return head_; }
    snode *tail() const;

    void push_head(snode &n)
    {
        n.next_ = head_;
        head_ = &n;
    }
    void push_tail(snode &n)
    {
        snode *t = tail();
        if (t != NULL) {
            t->next_ = &n;
        } else {
            head_ = &n;
        }
    }

    snode *pop_head()
    {
        snode *n = head_;
        if (n != NULL) {
            head_ = head_->next_;
            n->next_ = NULL;
        }
        return n;
    }
    snode *pop_tail();

    void clear();

    void insert_back(snode &pos, snode &n)
    {
        assert(n.next_ == nullptr);
        n.next_ = pos.next_;
        pos.next_ = &n;
    }

    snode *remove(snode &n);
    snode *remove(snode *prev, snode &n);

    void move(slist &src);
    size_t move(slist &src, size_t count);
    void swap(slist &l) { std::swap(head_, l.head_); }

private:
    snode *head_{nullptr};

    TT_NON_COPYABLE(slist)
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

#endif /* __TT_SINGLE_LINKED_LIST_CPP__ */
