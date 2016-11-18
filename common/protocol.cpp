/*
  protocol.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  acuordance with GammaRay Commercial License Agreement provided with the Software.

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

#include "protocol.h"

namespace GammaRay {
namespace Protocol {
Protocol::ModelIndex fromQModelIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return ModelIndex();
    ModelIndex result = fromQModelIndex(index.parent());
    result.push_back(qMakePair(index.row(), index.column()));
    return result;
}

QModelIndex toQModelIndex(const QAbstractItemModel *model, const Protocol::ModelIndex &index)
{
    QModelIndex qmi;

    for (Protocol::ModelIndex::ConstIterator it = index.constBegin(), end = index.constEnd();
         it != end; ++it) {
        qmi = model->index(it->first, it->second, qmi);
        if (!qmi.isValid())
            return QModelIndex(); // model isn't loaded to the full depth, so don't restart from the top
    }

    return qmi;
}

qint32 version()
{
    return 29;
}

qint32 broadcastFormatVersion()
{
    return 2;
}
}
}
