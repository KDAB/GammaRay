/*
  paintbuffermodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_WIDGETINSPECTOR_PAINTBUFFERMODEL_H
#define GAMMARAY_WIDGETINSPECTOR_PAINTBUFFERMODEL_H

#include <config-gammaray.h>
#include "paintbuffer.h"

#include <common/modelroles.h>

#include <QAbstractItemModel>

QT_BEGIN_NAMESPACE
struct QPaintBufferCommand;
class QPainterPath;
QT_END_NAMESPACE

namespace GammaRay {
/**
 * Model that shows commands stored in a QPaintBuffer.
 */
class PaintBufferModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit PaintBufferModel(QObject *parent = nullptr);

    void setPaintBuffer(const PaintBuffer &buffer);
    PaintBuffer buffer() const;

    void setCosts(const QVector<double> &costs);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;

private:
    QVariant argumentAt(const QPaintBufferCommand &cmd, int index) const;
    QString argumentDisplayString(const QPaintBufferCommand &cmd) const;
    QVariant argumentDecoration(const QPaintBufferCommand &cmd) const;

    QPainterPath clipPath(int row) const;

    PaintBuffer m_buffer;
    QPaintBufferPrivate *m_privateBuffer;
    QVector<double> m_costs;
    double m_maxCost;
};
}

#endif // GAMMARAY_PAINTBUFFERMODEL_H
