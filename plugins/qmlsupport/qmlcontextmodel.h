/*
  qmlcontextmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QMLCONTEXTMODEL_H
#define GAMMARAY_QMLCONTEXTMODEL_H

#include <QAbstractTableModel>

#include <QVector>

QT_BEGIN_NAMESPACE
class QQmlContext;
QT_END_NAMESPACE

namespace GammaRay {
class QmlContextModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit QmlContextModel(QObject *parent = nullptr);
    ~QmlContextModel() override;

    void clear();
    void setContext(QQmlContext *leafContext);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    QVector<QQmlContext *> m_contexts;
};
}

#endif // GAMMARAY_QMLCONTEXTMODEL_H
