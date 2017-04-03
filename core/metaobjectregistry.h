/*
  metaobjecttreemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_METAOBJECTREGISTRY_H
#define GAMMARAY_METAOBJECTREGISTRY_H

#include <QObject>
#include <QSet>
#include <QVector>

namespace GammaRay {

class MetaObjectRegistry : public QObject
{
    Q_OBJECT

public:
    enum MetaObjectData {
        ClassName,
        Valid,
        SelfCount,
        SelfAliveCount,
        InclusiveCount,
        InclusiveAliveCount,
    };

    explicit MetaObjectRegistry(QObject *parent = nullptr);
    ~MetaObjectRegistry();

    void scanMetaTypes();

    QVariant data(const QMetaObject *metaObject, MetaObjectData type) const;
    bool isValid(const QMetaObject *metaObject) const;

    const QMetaObject *parentOf(const QMetaObject *metaObject) const;
    QVector<const QMetaObject *> childrenOf(const QMetaObject *metaObject) const;

public slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);

signals:
    void beforeMetaObjectAdded(const QMetaObject *metaObject);
    void afterMetaObjectAdded(const QMetaObject *metaObject);
    void dataChanged(const QMetaObject *metaObject);

private:
    void addMetaObject(const QMetaObject *metaObject);
    void removeMetaObject(const QMetaObject *metaObject);
    bool inheritsQObject(const QMetaObject *mo) const;

    bool isKnownMetaObject(const QMetaObject *metaObject) const;

private:
    QHash<const QMetaObject *, const QMetaObject *> m_childParentMap;
    QHash<const QMetaObject *, QVector<const QMetaObject *> > m_parentChildMap;

    struct MetaObjectInfo
    {
        MetaObjectInfo()
            : invalid(false)
            , selfCount(0)
            , selfAliveCount(0)
            , inclusiveCount(0)
            , inclusiveAliveCount(0) {}

        /**
         * True if the meta object is suspected invalid. We can't know when one is destroyed,
         * so we mark this as true when all of the objects with this type are destroyed.
         */
        bool invalid;
        /// Number of objects of a particular meta object type ever created
        int selfCount;
        /// Number of instances of a meta object currently alive
        int selfAliveCount;
        /**
         * Number of objects of the exact meta object type
         * + number of objects of type that inherit from this meta type
         */
        int inclusiveCount;
        /// Inclusive instance count currently alive
        int inclusiveAliveCount;
        /// A copy of QMetaObject::className()
        QByteArray className;
    };
    QHash<const QMetaObject*, MetaObjectInfo> m_metaObjectInfoMap;
    /// meta objects at creation time, so we can correctly decrement instance counts
    /// after destruction
    QHash<QObject*, const QMetaObject*> m_metaObjectMap;
};
}

#endif // GAMMARAY_METAOBJECTTREEMODEL_H
