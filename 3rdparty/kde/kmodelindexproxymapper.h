/*
    Copyright (C) 2010 Klarälvdalens Datakonsult AB,
        a KDAB Group company, info@kdab.net,
        author Stephen Kelly <stephen@kdab.com>
    Copyright (c) 2016 Ableton AG <info@ableton.com>
        Author Stephen Kelly <stephen.kelly@ableton.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef KMODELINDEXPROXYMAPPER_H
#define KMODELINDEXPROXYMAPPER_H

#include <QObject>

#include "kitemmodels_export.h"

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QModelIndex;
class QItemSelection;
QT_END_NAMESPACE
class KModelIndexProxyMapperPrivate;

/**
 * @class KModelIndexProxyMapper kmodelindexproxymapper.h KModelIndexProxyMapper
 *
 * @brief This class facilitates easy mapping of indexes and selections through proxy models.
 *
 * In a complex system of proxy models there can be a need to map indexes and selections between them,
 * and sometimes to do so without knowledge of the path from one model to another.
 *
 * For example,
 *
 * @verbatim
 *     Root model
 *         |
 *       /    \
 *   Proxy 1   Proxy 3
 *      |       |
 *   Proxy 2   Proxy 4
 * @endverbatim
 *
 * If there is a need to map indexes between proxy 2 and proxy 4, a KModelIndexProxyMapper can be created
 * to facilitate mapping of indexes between them.
 *
 * @code
 *   m_indexMapper = new KModelIndexProxyMapper(proxy2, proxy4, this);
 *
 *  ...
 *
 *   const QModelIndex proxy4Index = m_mapLeftToRight(proxy2->index(0, 0));
 *   Q_ASSERT(proxy4Index.model() == proxy4);
 * @endcode
 *
 * Note that the aim is to achieve black box connections so that there is no need for application code to
 * know the structure of proxy models in the path between left and right and attempt to manually map them.
 *
 * @verbatim
 *     Root model
 *         |
 *   ---------------
 *   |  Black Box  |
 *   ---------------
 *      |       |
 *   Proxy 2   Proxy 4
 * @endverbatim
 *
 * The isConnected property indicates whether there is a
 * path from the left side to the right side.
 *
 * @author Stephen Kelly <steveire@gmail.com>
 *
 */
class KITEMMODELS_EXPORT KModelIndexProxyMapper : public QObject
{
    Q_OBJECT

    /**
     * Indicates whether there is a chain that can be followed from leftModel to rightModel.
     *
     * This value can change if the sourceModel of an intermediate proxy is changed.
     */
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
public:
    /**
     * Constructor
     */
    KModelIndexProxyMapper(const QAbstractItemModel *leftModel, const QAbstractItemModel *rightModel, QObject *parent = nullptr);

    ~KModelIndexProxyMapper();

    /**
     * Maps the @p index from the left model to the right model.
     */
    QModelIndex mapLeftToRight(const QModelIndex &index) const;

    /**
     * Maps the @p index from the right model to the left model.
     */
    QModelIndex mapRightToLeft(const QModelIndex &index) const;

    /**
     * Maps the @p selection from the left model to the right model.
     */
    QItemSelection mapSelectionLeftToRight(const QItemSelection &selection) const;

    /**
     * Maps the @p selection from the right model to the left model.
     */
    QItemSelection mapSelectionRightToLeft(const QItemSelection &selection) const;

    bool isConnected() const;

Q_SIGNALS:
    void isConnectedChanged();

private:
    //@cond PRIVATE
    Q_DECLARE_PRIVATE(KModelIndexProxyMapper)
    KModelIndexProxyMapperPrivate *const d_ptr;
    //@endcond
};

#endif
