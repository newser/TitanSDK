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

#include <tt/algorithm/double_linked_list.h>

#include <misc/tt_assert.h>

#include <utility>

namespace tt {

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define DL_ASSERT TT_ASSERT

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

size_t dlist::count() const
{
    size_t count = 0;
    dnode *n = head_;
    while (n != NULL) {
        ++count;
        n = n->next_;
    }
    return count;
}

bool dlist::contain(const dnode &n) const
{
    dnode *pos = head_;
    while (pos != NULL) {
        if (pos == &n) { return true; }
        pos = pos->next_;
    }
    return false;
}

dlist &dlist::push_head(dnode &n)
{
    TT_ASSERT(!n.in_list());
    if (head_ != NULL) {
        head_->prev_ = &n;
        n.next_ = head_;
    } else {
        tail_ = &n;
    }
    head_ = &n;
    return *this;
}

dlist &dlist::push_tail(dnode &n)
{
    TT_ASSERT(!n.in_list());
    if (tail_ != NULL) {
        tail_->next_ = &n;
        n.prev_ = tail_;
    } else {
        head_ = &n;
    }
    tail_ = &n;
    return *this;
}

dnode *dlist::pop_head()
{
    dnode *n = head_;
    if (n != NULL) {
        head_ = head_->next_;
        if (n->next_ != NULL) {
            n->next_->prev_ = NULL;
        } else {
            tail_ = NULL;
        }
        n->clear();
    }
    return n;
}

dnode *dlist::pop_tail()
{
    dnode *n = tail_;
    if (n != NULL) {
        if (n->prev_ != NULL) {
            n->prev_->next_ = NULL;
        } else {
            head_ = NULL;
        }
        tail_ = tail_->prev_;
        n->clear();
    }
    return n;
}

dlist &dlist::insert_front(dnode &pos, dnode &n)
{
    DL_ASSERT(contain(pos));
    TT_ASSERT(!n.in_list());

    if (pos.prev_ != NULL) {
        pos.prev_->next_ = &n;
    } else {
        head_ = &n;
    }
    n.prev_ = pos.prev_;

    n.next_ = &pos;
    pos.prev_ = &n;

    return *this;
}

dlist &dlist::insert_back(dnode &pos, dnode &n)
{
    DL_ASSERT(contain(pos));
    TT_ASSERT(!n.in_list());

    if (pos.next_ != NULL) {
        pos.next_->prev_ = &n;
    } else {
        tail_ = &n;
    }
    n.next_ = pos.next_;

    n.prev_ = &pos;
    pos.next_ = &n;

    return *this;
}

dnode *dlist::remove(dnode &n)
{
    if (!n.in_list()) { return nullptr; }
    DL_ASSERT(contain(n));

    dnode *next = n.next_;
    if (next != NULL) {
        next->prev_ = n.prev_;
    } else {
        tail_ = n.prev_;
    }

    if (n.prev_ != NULL) {
        n.prev_->next_ = next;
    } else {
        head_ = next;
    }

    n.clear();

    return next;
}

dlist &dlist::move(dlist &src)
{
    if (src.empty()) { return *this; }

    if (tail_ != NULL) {
        tail_->next_ = src.head_;
        src.head_->prev_ = tail_;
    } else {
        head_ = src.head_;
    }
    tail_ = src.tail_;

    src.head_ = nullptr;
    src.tail_ = nullptr;

    return *this;
}

dlist &dlist::swap(dlist &l)
{
    std::swap(head_, l.head_);
    std::swap(tail_, l.tail_);

    return *this;
}

}
