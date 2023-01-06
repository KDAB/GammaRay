/*
  ringbuffer.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_RINGBUFFER_H
#define GAMMARAY_RINGBUFFER_H

#include <QVector>

namespace GammaRay {

template<class T>
struct RingBuffer
{
    RingBuffer(int capacity)
        : m_head(0)
        , m_capacity(capacity)
    {
        Q_ASSERT(capacity > 0);
    }

    int count() const
    {
        return qMin(m_capacity, m_vector.count());
    }
    int capacity() const
    {
        return m_capacity;
    }
    bool isEmpty() const
    {
        return count() == 0;
    }

    const T &last() const
    {
        return at(count() - 1);
    }

    void append(const T &t)
    {
        if (m_vector.count() == m_capacity) {
            m_vector[m_head++] = t;
            if (m_head >= m_capacity)
                m_head = 0;
        } else {
            m_vector.append(t);
        }
    }

    const T &at(int i) const
    {
        int index = (i + m_head) % m_capacity;
        return m_vector.at(index);
    }

    void clear()
    {
        m_vector.clear();
        m_head = 0;
    }

    QVector<T> m_vector;
    int m_head;
    int m_capacity;
};

}

#endif
