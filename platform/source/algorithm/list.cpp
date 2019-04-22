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

#include <tt/algorithm/list.h>

#include <misc/tt_assert.h>

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

lnode &lnode::insert_front(lnode &n)
{
    TT_ASSERT(!this->in_list() && n.in_list());

    n.next_ = this;
    n.prev_ = prev_;
    n.list_ = list_;

    // link it
    if (prev_ != nullptr) {
        prev_->next_ = &n;
    } else {
        list_->head_ = &n;
    }
    prev_ = &n;
    ++list_->count_;

    return *this;
}

lnode &lnode::insert_back(lnode &n)
{
    TT_ASSERT(!this->in_list() && n.in_list());

    n.prev_ = this;
    n.next_ = this->next_;
    n.list_ = list_;

    // link it
    if (next_ != nullptr) {
        next_->prev_ = &n;
    } else {
        list_->tail_ = &n;
    }
    next_ = &n;
    ++list_->count_;

    return *this;
}

lnode *lnode::remove()
{
    if (in_list()) { return nullptr; }

    lnode *next = next_;
    if (prev_ != nullptr) {
        prev_->next_ = next;
    } else {
        list_->head_ = next_;
    }

    if (next != nullptr) {
        next->prev_ = prev_;
    } else {
        list_->tail_ = prev_;
    }

    --list_->count_;

    clear();

    return next;
}

list &list::push_head(lnode &n)
{
    TT_ASSERT(n.in_list());

    n.next_ = head_;
    n.list_ = this;

    // link it
    if (head_ != nullptr) {
        head_->prev_ = &n;
    } else {
        tail_ = &n;
    }
    head_ = &n;
    ++count_;

    return *this;
}

list &list::push_tail(lnode &n)
{
    TT_ASSERT(n.in_list());

    n.prev_ = tail_;
    n.list_ = this;

    // link it
    if (tail_ != nullptr) {
        tail_->next_ = &n;
    } else {
        head_ = &n;
    }
    tail_ = &n;
    ++count_;

    return *this;
}

lnode *list::pop_head()
{
    lnode *n = head_;
    if (n != nullptr) {
        head_ = n->next_;
        if (n->next_ != nullptr) {
            n->next_->prev_ = nullptr;
        } else {
            tail_ = nullptr;
        }
        --count_;

        n->clear();
    }

    return n;
}

lnode *list::pop_tail()
{
    lnode *n = tail_;
    if (n != nullptr) {
        tail_ = n->prev_;
        if (n->prev_ != nullptr) {
            n->prev_->next_ = nullptr;
        } else {
            head_ = nullptr;
        }
        --count_;

        n->clear();
    }

    return n;
}

}
