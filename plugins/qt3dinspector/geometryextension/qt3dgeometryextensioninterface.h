/*
  qt3dgeometryextensioninterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QT3DGEOMETRYEXTENSIONINTERFACE_H
#define GAMMARAY_QT3DGEOMETRYEXTENSIONINTERFACE_H

#include <QtGlobal>

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
namespace Qt3DGeometry = Qt3DCore;

namespace GammaRay {
struct Qt3DGeometryAttributeData
{
    Qt3DGeometryAttributeData() = default;
    bool operator==(const Qt3DGeometryAttributeData &rhs) const;

    QString name;
    Qt3DGeometry::QAttribute::AttributeType attributeType = Qt3DGeometry::QAttribute::VertexAttribute;
    uint byteOffset = 0;
    uint byteStride = 0;
    uint count = 0;
    uint divisor = 0;
    Qt3DGeometry::QAttribute::VertexBaseType vertexBaseType = Qt3DGeometry::QAttribute::UnsignedShort;
    uint vertexSize = 0;
    uint bufferIndex = 0;
};

struct Qt3DGeometryBufferData
{
    Qt3DGeometryBufferData() = default;
    bool operator==(const Qt3DGeometryBufferData &rhs) const;

    QString name;
    QByteArray data;
};

struct Qt3DGeometryData
{
    bool operator==(const Qt3DGeometryData &rhs) const;

    QVector<Qt3DGeometryAttributeData> attributes;
    QVector<Qt3DGeometryBufferData> buffers;
};

class Qt3DGeometryExtensionInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(
        GammaRay::Qt3DGeometryData geometryData READ geometryData WRITE setGeometryData NOTIFY geometryDataChanged)
public:
    explicit Qt3DGeometryExtensionInterface(const QString &name, QObject *parent = nullptr);
    ~Qt3DGeometryExtensionInterface();

    Qt3DGeometryData geometryData() const;
    void setGeometryData(const Qt3DGeometryData &data);

signals:
    void geometryDataChanged();

private:
    Qt3DGeometryData m_data;
};
}

Q_DECLARE_METATYPE(GammaRay::Qt3DGeometryData)
QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::Qt3DGeometryExtensionInterface,
                    "com.kdab.GammaRay.Qt3DGeometryExtensionInterface/1.0")
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE
QDataStream &operator<<(QDataStream &s, const GammaRay::Qt3DGeometryData &);
QDataStream &operator>>(QDataStream &s, GammaRay::Qt3DGeometryData &);
QDataStream &operator<<(QDataStream &out, const GammaRay::Qt3DGeometryBufferData &data);
QDataStream &operator>>(QDataStream &out, GammaRay::Qt3DGeometryBufferData &data);
QDataStream &operator<<(QDataStream &out, const GammaRay::Qt3DGeometryAttributeData &data);
QDataStream &operator>>(QDataStream &in, GammaRay::Qt3DGeometryAttributeData &data);
QT_END_NAMESPACE

#endif // GAMMARAY_QT3DGEOMETRYEXTENSIONINTERFACE_H
