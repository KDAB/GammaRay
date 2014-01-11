/*
  sharedmemorylocker.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <QSharedMemory>

namespace GammaRay
{

/** RAII helper class for locking QSharedMemory.
 * @todo this should be upstream
 */
class SharedMemoryLocker
{
public:
  explicit inline SharedMemoryLocker(QSharedMemory *shm) : m_shm(shm)
  {
    Q_ASSERT(shm);
    shm->lock();
  }

  inline ~SharedMemoryLocker()
  {
    m_shm->unlock();
  }

private:
  QSharedMemory *m_shm;
};

}
