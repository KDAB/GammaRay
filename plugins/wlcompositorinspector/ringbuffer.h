/*
  ringbuffer.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

  int count() const { return qMin(m_capacity, m_vector.count()); }
  int capacity() const { return m_capacity; }
  bool isEmpty() const { return count() == 0; }

  const T &last() const { return at(count() - 1); }

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
