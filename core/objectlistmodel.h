/*
  objectlistmodel.h

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

#ifndef GAMMARAY_OBJECTLISTMODEL_H
#define GAMMARAY_OBJECTLISTMODEL_H

#include "objectmodelbase.h"

#include <QMutex>
#include <QVector>
#include <QSet>

namespace GammaRay {
class Probe;

/**
 * NOTE: Making the model itself threadsafe works in theory,
 * but as soon as we put a proxymodel on top everything breaks.
 * Esp. the {begin,end}{InsertRemove}Rows() calls trigger
 * signals which apparently must be delivered directly to the proxy,
 * otherwise it's internal state may be messed up and assertions
 * start flying around...
 * So the solution: only call these methods in the main thread
 * and on remove. when called from a background thread, invalidate
 * the data first.
 */
class ObjectListModel : public ObjectModelBase<QAbstractTableModel>
{
    Q_OBJECT
public:
    explicit ObjectListModel(Probe *probe);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE QPair<int, QVariant> defaultSelectedItem() const;

    /*!
     * Returns a list of all objects.
     *
     * FIXME: This is a dirty hack. Instead of offering a getter to the internal data
     * here, we should move it out and only give the model a view of the data.
     */
    const QVector<QObject*> &objects() const;

private slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);

private:
    void removeObject(QObject *obj);

    // sorted vector for stable iterators/indexes, esp. for the model methods
    QVector<QObject *> m_objects;
};
}

#endif // GAMMARAY_OBJECTLISTMODEL_H
