/*
  actionmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <common/modelroles.h>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

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

    enum Role {
        ObjectIdRole = UserRole + 1,
        ObjectRole,
        ShortcutConflictRole
    };

    explicit ActionModel(QObject *parent = nullptr);
    ~ActionModel() override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

public slots:
    void objectAdded(QObject *object);
    void objectRemoved(QObject *object);

private slots:
    void actionChanged();

private:
    void scanForShortcutDuplicates() const;

    // sorted vector of QActions
    QVector<QAction *> m_actions;

    ActionValidator *m_duplicateFinder;
};
}

#endif // GAMMARAY_ACTIONMODEL_H
