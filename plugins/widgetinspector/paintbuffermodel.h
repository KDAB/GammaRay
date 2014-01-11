/*
  paintbuffermodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PAINTBUFFERMODEL_H
#define GAMMARAY_PAINTBUFFERMODEL_H

#include "config-gammaray.h"

#ifdef HAVE_PRIVATE_QT_HEADERS
#include <QAbstractItemModel>

#include <private/qpaintbuffer_p.h> //krazy:exclude=camelcase

class QPaintBuffer;

namespace GammaRay {

/**
 * Model that shows commands stored in a QPaintBuffer.
 */
class PaintBufferModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    explicit PaintBufferModel(QObject *parent = 0);

    void setPaintBuffer(const QPaintBuffer &buffer);
    QPaintBuffer buffer() const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

  private:
    QPaintBuffer m_buffer;
    QPaintBufferPrivate *m_privateBuffer;
};

}

#endif

#endif // GAMMARAY_PAINTBUFFERMODEL_H
