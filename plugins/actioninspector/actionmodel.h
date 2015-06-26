/*
  actionmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef GAMMARAY_ACTIONINSPECTOR_ACTIONMODEL_H
#define GAMMARAY_ACTIONINSPECTOR_ACTIONMODEL_H

#include <QAbstractTableModel>
#include <QVector>

class QAction;

namespace GammaRay {

class ActionValidator;

class ActionModel : public QAbstractTableModel
{
  Q_OBJECT

  public:
    enum Column {
      AddressColumn,
      NameColumn,
      CheckablePropColumn,
      CheckedPropColumn,
      PriorityPropColumn,
      ShortcutsPropColumn,
      /** Mark column count */
      ColumnCount
    };

    explicit ActionModel(QObject *parent = 0);
    ~ActionModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

  public slots:
    void objectAdded(QObject *object);
    void objectRemoved(QObject *object);

  private:
    // sorted vector of QActions
    QVector<QAction*> m_actions;

    ActionValidator *m_duplicateFinder;
};

}

#endif // GAMMARAY_ACTIONMODEL_H
