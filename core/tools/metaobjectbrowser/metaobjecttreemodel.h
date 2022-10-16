/*
  metaobjecttreemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_METAOBJECTTREEMODEL_H
#define GAMMARAY_METAOBJECTTREEMODEL_H

#include <QModelIndex>
#include <QSet>
#include <QVector>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
class Probe;

class MetaObjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit MetaObjectTreeModel(QObject *parent = nullptr);
    ~MetaObjectTreeModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    QModelIndexList match(const QModelIndex &start, int role, const QVariant &value, int hits,
                          Qt::MatchFlags flags) const override;

private:
    QModelIndex indexForMetaObject(const QMetaObject *metaObject) const;
    static const QMetaObject *metaObjectForIndex(const QModelIndex &index);


private slots:
    void addMetaObject(const QMetaObject *metaObject);
    void endAddMetaObject(const QMetaObject *metaObject);
    void scheduleDataChange(const QMetaObject *mo);
    void emitPendingDataChanged();

private:
    QSet<const QMetaObject *> m_pendingDataChanged;
    QTimer *m_pendingDataChangedTimer;
};
}

#endif // GAMMARAY_METAOBJECTTREEMODEL_H
