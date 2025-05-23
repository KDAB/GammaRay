/*
  metaobjectregistry.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    enum MetaObjectData
    {
        ClassName,
        Valid,
        SelfCount,
        SelfAliveCount,
        InclusiveCount,
        InclusiveAliveCount,
    };

    explicit MetaObjectRegistry(QObject *parent = nullptr);
    ~MetaObjectRegistry() override;

    void scanMetaTypes();

    static bool isTypeIdRegistered(int typeId);

    QVariant data(const QMetaObject *metaObject, MetaObjectData type) const;
    bool isValid(const QMetaObject *metaObject) const;
    bool isStatic(const QMetaObject *metaObject) const;
    const QMetaObject *aliveInstance(const QMetaObject *metaObject) const;

    const QMetaObject *parentOf(const QMetaObject *metaObject) const;
    QVector<const QMetaObject *> childrenOf(const QMetaObject *metaObject) const;

    const QMetaObject *canonicalMetaObject(const QMetaObject *metaObject) const;

public slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);

signals:
    void beforeMetaObjectAdded(const QMetaObject *metaObject);
    void afterMetaObjectAdded(const QMetaObject *metaObject);
    void dataChanged(const QMetaObject *metaObject);

private:
    const QMetaObject *addMetaObject(const QMetaObject *metaObject, bool mergeDynamic = false);
    bool inheritsQObject(const QMetaObject *metaObject) const;

    bool isKnownMetaObject(const QMetaObject *metaObject) const;
    void addAliveInstance(QObject *obj, const QMetaObject *canonicalMO);
    void removeAliveInstance(QObject *obj, const QMetaObject *canonicalMO);

private:
    QHash<const QMetaObject *, const QMetaObject *> m_childParentMap;
    QHash<const QMetaObject *, QVector<const QMetaObject *>> m_parentChildMap;

    struct MetaObjectInfo
    {
        MetaObjectInfo() = default;

        /// @c true if this is a static meta object that can only become invalid by DLL unloading.
        bool isStatic = false;
        /// @c true if this is a merged dynamic meta object, as e.g. in use by QML
        bool isDynamic = false;
        /**
         * True if the meta object is suspected invalid. We can't know when one is destroyed,
         * so we mark this as true when all of the objects with this type are destroyed.
         */
        bool invalid = false;
        /// Number of objects of a particular meta object type ever created
        int selfCount = 0;
        /// Number of instances of a meta object currently alive
        int selfAliveCount = 0;
        /**
         * Number of objects of the exact meta object type
         * + number of objects of type that inherit from this meta type
         */
        int inclusiveCount = 0;
        /// Inclusive instance count currently alive
        int inclusiveAliveCount = 0;
        /// A copy of QMetaObject::className()
        QByteArray className;
    };
    QHash<const QMetaObject *, MetaObjectInfo> m_metaObjectInfoMap;
    /// canonical meta objects at creation time, so we can correctly decrement instance counts
    /// after destruction
    QHash<QObject *, const QMetaObject *> m_metaObjectMap;
    /// name to canonical QMO map, for merging dynamic meta objects as produced by QML
    QHash<QByteArray, const QMetaObject *> m_metaObjectNameMap;

    /// alive instances for canonical dynamic meta objects
    QHash<const QMetaObject *, QVector<const QMetaObject *>> m_aliveInstances;
    /// mapping from QObject* to its owned QMetaObject (for dynamic ones only)
    /// this is needed to clean up m_aliveInstances on deletion
    QHash<QObject *, const QMetaObject *> m_dynamicMetaObjectMap;
    /// QMO instance to canonical QMO mapping (for dynamic ones only)
    QHash<const QMetaObject *, const QMetaObject *> m_canonicalMetaObjectMap;
};
}

#endif // GAMMARAY_METAOBJECTTREEMODEL_H
