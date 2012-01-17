/*
  readorwritelocker.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_READORWRITELOCKER_H
#define GAMMARAY_READORWRITELOCKER_H

#include <QReadWriteLock>

namespace GammaRay {

/**
 * Lock that can/should be used instead of QReadLocker
 * when we want to read or write lock and can have
 * nested functions that first writelock and then readlock.
 *
 * Esp. required for the threadsafe models.
 */
class ReadOrWriteLocker
{
  public:
    ReadOrWriteLocker(QReadWriteLock *lock)
      : m_lock(lock),
        m_locked(false)
    {
      this->lock();
    }

    void lock()
    {
      if (!m_lock->tryLockForWrite()) {
        m_lock->lockForRead();
      }
      m_locked = true;
    }

    void unlock()
    {
      m_lock->unlock();
      m_locked = false;
    }

    ~ReadOrWriteLocker()
    {
      if (m_locked) {
        unlock();
      }
    }

  private:
    QReadWriteLock *m_lock;
    bool m_locked;
};

}

#endif // GAMMARAY_READORWRITELOCKER_H
