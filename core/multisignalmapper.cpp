/*
  multisignalmapper.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "multisignalmapper.h"

#include <QDebug>
#include <QMetaMethod>
#include <QMetaObject>

namespace GammaRay {

class MultiSignalMapperPrivate : public QObject
{
  public:
    explicit MultiSignalMapperPrivate(MultiSignalMapper* parent) : QObject(parent), q(parent) {}
    ~MultiSignalMapperPrivate() {}

    int qt_metacall(QMetaObject::Call call, int methodId, void** args)
    {
      methodId = QObject::qt_metacall(call, methodId, args);
      if (methodId < 0)
        return methodId;

      if (call == QMetaObject::InvokeMetaMethod) {
        if (methodId == 0) {
          Q_ASSERT(sender());
          Q_ASSERT(senderSignalIndex() != -1);
          emit q->signalEmitted(sender(), senderSignalIndex());
        }
        --methodId; // our method offset is 1
      }
      return methodId;
    }

  private:
    MultiSignalMapper* q;
};

}

using namespace GammaRay;

MultiSignalMapper::MultiSignalMapper(QObject *parent) : QObject(parent), d(new MultiSignalMapperPrivate(this))
{
}

MultiSignalMapper::~MultiSignalMapper()
{
}

void MultiSignalMapper::connectToSignal(QObject *sender, const QMetaMethod &signal)
{
  QMetaObject::connect(sender, signal.methodIndex(), d, QObject::metaObject()->methodCount(), Qt::AutoConnection | Qt::UniqueConnection, 0);
}
