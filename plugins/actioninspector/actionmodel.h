/*
  actionmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    enum Column
    {
        AddressColumn,
        NameColumn,
        CheckablePropColumn,
        CheckedPropColumn,
        PriorityPropColumn,
        ShortcutsPropColumn,
        /** Mark column count */
        ColumnCount
    };

    enum Role
    {
        ObjectIdRole = UserRole + 1,
        ObjectRole,
        ShortcutConflictRole
    };

    explicit ActionModel(QObject *parent = nullptr);
    ~ActionModel() override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

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
