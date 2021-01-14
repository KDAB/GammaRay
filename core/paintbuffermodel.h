/*
  paintbuffermodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

    QModelIndex index(int row, int column, const QModelIndex & parent) const override;
    QModelIndex parent(const QModelIndex & child) const override;

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
