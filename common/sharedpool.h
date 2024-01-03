/*
  sharedpool.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SHAREDPOOL_H
#define GAMMARAY_SHAREDPOOL_H

#include <assert.h>
#include <iostream>
#include <functional>
#include <memory>
#include <stack>
#include <vector>

#define IF_DEBUG(x)

namespace GammaRay {

template<class T>
class SharedPool
{
public:
    // no `using a = b;` for MSVC2010 :(
    typedef std::unique_ptr<T, std::function<void(T *)>> PtrType;

    SharedPool(size_t prealloc = 0)
        : m_capacity(0)
    {
        while (prealloc--) {
            add(std::unique_ptr<T>(new T));
        }
    }
    ~SharedPool()
    {
        assert(m_capacity == size() && "Some objects are still acquired");
        IF_DEBUG(std::cout << "Acquired objects left in pool on destruction: " << (m_capacity - size()) << std::endl);
    }

    void add(std::unique_ptr<T> t)
    {
        m_pool.push(std::move(t));
        m_capacity++;

        IF_DEBUG(std::cout << "Adding object to pool: " << m_pool.top().get() << " - current capacity:" << m_capacity << std::endl);
    }

    PtrType acquire()
    {
        // insert more if necessary
        if (m_pool.empty()) {
            IF_DEBUG(std::cout << "Growing pool by one" << std::endl);
            add(std::unique_ptr<T>(new T));
        }

        auto ptr = m_pool.top().release();
        IF_DEBUG(std::cout << "Acquire: " << ptr << std::endl);
        PtrType tmp(ptr, [this](T *ptr) {
            IF_DEBUG(std::cout << "Release: " << ptr << std::endl);
            m_pool.push(std::unique_ptr<T>(ptr));
        });
        m_pool.pop();
        return tmp;
    }

    bool empty() const
    {
        return m_pool.empty();
    }

    size_t capacity() const
    {
        return m_capacity;
    }

    size_t size() const
    {
        return m_pool.size();
    }

private:
    size_t m_capacity;
    std::stack<std::unique_ptr<T>, std::vector<std::unique_ptr<T>>> m_pool;
};

}

#endif
