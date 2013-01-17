/*
  objectpropertymodel.h

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

#ifndef GAMMARAY_OBJECTPROPERTYMODEL_H
#define GAMMARAY_OBJECTPROPERTYMODEL_H

#include <QAbstractItemModel>
#include <QPointer>

class QTimer;

namespace GammaRay {

class ObjectPropertyModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    explicit ObjectPropertyModel(QObject *parent = 0);
    void setObject(QObject *object);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  protected:
    QPointer<QObject> m_obj;

  private slots:
    void slotReset() { reset(); }
    void updateAll();
    void doEmitChanged();

  private:
    QTimer *m_updateTimer;
};

}

#endif // GAMMARAY_OBJECTPROPERTYMODEL_H
