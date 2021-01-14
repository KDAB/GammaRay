/*
  objecttreemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_OBJECTTREEMODEL_H
#define GAMMARAY_OBJECTTREEMODEL_H

#include "objectmodelbase.h"

#include <QVector>

namespace GammaRay {
class Probe;

class ObjectTreeModel : public ObjectModelBase<QAbstractItemModel>
{
    Q_OBJECT
public:
    explicit ObjectTreeModel(Probe *probe);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE QPair<int, QVariant> defaultSelectedItem() const;

private slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);
    void objectReparented(QObject *obj);

private:
    QModelIndex indexForObject(QObject *object) const;

private:
    QHash<QObject *, QObject *> m_childParentMap;
    QHash<QObject *, QVector<QObject *> > m_parentChildMap;
};
}

#endif // GAMMARAY_OBJECTTREEMODEL_H
