/*
  messagemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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
#ifndef GAMMARAY_MESSAGEHANDLER_MESSAGEMODEL_H
#define GAMMARAY_MESSAGEHANDLER_MESSAGEMODEL_H

#include "backtrace.h"

#include <QAbstractTableModel>
#include <QTime>
#include <QVector>

namespace GammaRay {

struct DebugMessage {
  QtMsgType type;
  QString message;
  QTime time;
  Backtrace backtrace;
};

}

Q_DECLARE_METATYPE(GammaRay::DebugMessage)
Q_DECLARE_TYPEINFO(GammaRay::DebugMessage, Q_MOVABLE_TYPE);

namespace GammaRay {

class MessageModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    explicit MessageModel(QObject *parent = 0);
    ~MessageModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    enum Columns {
      TypeColumn,
      TimeColumn,
      MessageColumn,
      COLUMN_COUNT
    };

  public slots:
    void addMessage(const GammaRay::DebugMessage &message);

  private:
    QVector<DebugMessage> m_messages;
};

}

#endif // MESSAGEMODEL_H
