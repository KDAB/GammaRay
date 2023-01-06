/*
  statemachineviewerutil.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWERUTIL_H
#define GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWERUTIL_H

#include <QQueue>

template<class T>
class RingBuffer
{
public:
    RingBuffer()
        : m_size(5)
    {
    }

    void resize(int size)
    {
        Q_ASSERT(size > 0);
        m_size = size;
        cleanup();
    }

    void enqueue(T t)
    {
        m_entries.enqueue(t);
        cleanup();
    }

    void clear()
    {
        m_entries.clear();
    }

    int size() const
    {
        return m_entries.size();
    }

    T tail() const
    {
        return m_entries.last();
    }

    QList<T> entries() const
    {
        return m_entries;
    }

private:
    void cleanup()
    {
        while (m_entries.size() > m_size)
            m_entries.dequeue();
    }

    QQueue<T> m_entries;
    int m_size;
};

#endif
