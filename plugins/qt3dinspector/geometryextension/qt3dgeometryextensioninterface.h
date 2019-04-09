/*
  qt3dgeometryextensioninterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_QT3DGEOMETRYEXTENSIONINTERFACE_H
#define GAMMARAY_QT3DGEOMETRYEXTENSIONINTERFACE_H

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>

namespace GammaRay {
struct Qt3DGeometryAttributeData
{
    Qt3DGeometryAttributeData() = default;
    bool operator==(const Qt3DGeometryAttributeData &rhs) const;

    QString name;
    Qt3DRender::QAttribute::AttributeType attributeType = Qt3DRender::QAttribute::VertexAttribute;
    uint byteOffset = 0;
    uint byteStride = 0;
    uint count = 0;
    uint divisor = 0;
    Qt3DRender::QAttribute::VertexBaseType vertexBaseType = Qt3DRender::QAttribute::UnsignedShort;
    uint vertexSize = 0;
    uint bufferIndex = 0;
};

struct Qt3DGeometryBufferData
{
    Qt3DGeometryBufferData() = default;
    bool operator==(const Qt3DGeometryBufferData &rhs) const;

    QString name;
    QByteArray data;
    Qt3DRender::QBuffer::BufferType type = Qt3DRender::QBuffer::VertexBuffer;
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

#endif // GAMMARAY_QT3DGEOMETRYEXTENSIONINTERFACE_H
