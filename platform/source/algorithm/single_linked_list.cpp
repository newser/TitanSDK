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

#include <tt/algorithm/single_linked_list.h>

#include <misc/tt_assert.h>

#include <utility>

namespace tt {

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

size_t slist::count() const
{
    size_t count = 0;
    snode *n = head_;
    while (n != nullptr) {
        ++count;
        n = n->next_;
    }
    return count;
}

bool slist::contain(IN const snode &n) const
{
    snode *p = head_;
    while (p != nullptr) {
        if (p == &n) { return true; }
        p = p->next_;
    }
    return false;
}

snode *slist::tail() const
{
    snode *n = head_;
    if (n != nullptr) {
        while (n->next_ != nullptr) { n = n->next_; }
    }
    return n;
}

snode *slist::pop_tail()
{
    snode *n = head_;
    if (n == nullptr) { return nullptr; }

    if (n->next_ == nullptr) {
        head_ = nullptr;
        return n;
    }

    snode *prev;
    do {
        prev = n;
        n = n->next_;
    } while (n->next_ != nullptr);
    prev->next_ = nullptr;

    n->clear();
    return n;
}

void slist::clear()
{
    snode *n = head_;
    while (n != nullptr) {
        snode *next = n->next_;
        n->next_ = nullptr;
        n = next;
    }
    head_ = nullptr;
}

snode *slist::remove(IN snode &n)
{
    snode *p = head_;
    snode *next = n.next_;
    if (p == &n) {
        head_ = next;
    } else {
        while ((p != nullptr) && (p->next_ != &n)) { p = p->next_; }
        if (p != nullptr) { p->next_ = next; }
    }
    n.clear();
    return next;
}

snode *slist::remove(IN snode *prev, IN snode &n)
{
    snode *next = n.next_;
    if (prev != nullptr) {
        TT_ASSERT(prev->next_ == &n);
        prev->next_ = next;
    } else {
        TT_ASSERT(head_ == &n);
        head_ = next;
    }
    n.clear();
    return next;
}

slist &slist::move(IN slist &src)
{
    snode *t = tail();
    if (t != nullptr) {
        t->next_ = src.head_;
    } else {
        head_ = src.head_;
    }
    src.head_ = nullptr;

    return *this;
}

// returns how many nodes are moved to dst
size_t slist::move(IN slist &src, IN size_t count)
{
    if (this == &src) { return 0; }

    snode *prev = nullptr;
    snode *p = src.head_;
    size_t n = 0;
    while ((p != nullptr) && (n < count)) {
        prev = p;
        p = p->next_;
        ++n;
    }

    if (prev != nullptr) {
        if (head_ != nullptr) {
            tail()->next_ = src.head_;
        } else {
            head_ = src.head_;
        }
        src.head_ = prev->next_;
        prev->next_ = nullptr;
    }

    return n;
}

slist &slist::swap(IN slist &l)
{
    std::swap(head_, l.head_);

    return *this;
}

}
