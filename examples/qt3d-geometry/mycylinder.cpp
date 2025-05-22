/*
  mycylinder.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "mycylinder.h"

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QGeometry>
namespace Qt3DGeometry = Qt3DCore;

#include <QVector3D>
#include <qmath.h>

class MyCylinderGeometry : public Qt3DGeometry::QGeometry
{
    Q_OBJECT
public:
    explicit MyCylinderGeometry(QNode *parent = nullptr);
    ~MyCylinderGeometry();

private:
    void createVertexData();
    void createSidesVertices(float *&verticesPtr) const;
    void createDiscVertices(float *&verticesPtr, float yPosition) const;
    void createIndexData();
    void createSidesIndices(quint16 *&indicesPtr) const;
    void createDiscIndices(quint16 *&indicesPtr, int discCenterIndex, float yPosition) const;

    Qt3DGeometry::QAttribute *m_positionAttribute;
    Qt3DGeometry::QAttribute *m_normalAttribute;
    Qt3DGeometry::QAttribute *m_indexAttribute;

    Qt3DGeometry::QBuffer *m_vertexBuffer;
    Qt3DGeometry::QBuffer *m_indexBuffer;

    int m_rings = 7;
    int m_slices = 16;
    float m_radius = 1.0f;
    float m_length = 4.0f;
};

MyCylinderGeometry::MyCylinderGeometry(QNode *parent)
    : QGeometry(parent)
{
    m_positionAttribute = new Qt3DGeometry::QAttribute(this);
    m_normalAttribute = new Qt3DGeometry::QAttribute(this);
    m_indexAttribute = new Qt3DGeometry::QAttribute(this);

    m_vertexBuffer = new Qt3DGeometry::QBuffer(this);
    m_indexBuffer = new Qt3DGeometry::QBuffer(this);

    // vec3 pos, vec3 normal
    const quint32 elementSize = 3 + 3;
    const quint32 stride = elementSize * sizeof(float);
    const int nVerts = (m_slices + 1) * m_rings + 2 * (m_slices + 1) + 2;
    const int faces = (m_slices * 2) * (m_rings - 1) + (m_slices * 2);

    m_positionAttribute->setName(Qt3DGeometry::QAttribute::defaultPositionAttributeName());
    m_positionAttribute->setVertexBaseType(Qt3DGeometry::QAttribute::Float);
    m_positionAttribute->setVertexSize(3);
    m_positionAttribute->setAttributeType(Qt3DGeometry::QAttribute::VertexAttribute);
    m_positionAttribute->setBuffer(m_vertexBuffer);
    m_positionAttribute->setByteStride(stride);
    m_positionAttribute->setCount(nVerts);

    m_normalAttribute->setName(Qt3DGeometry::QAttribute::defaultNormalAttributeName());
    m_normalAttribute->setVertexBaseType(Qt3DGeometry::QAttribute::Float);
    m_normalAttribute->setVertexSize(3);
    m_normalAttribute->setAttributeType(Qt3DGeometry::QAttribute::VertexAttribute);
    m_normalAttribute->setBuffer(m_vertexBuffer);
    m_normalAttribute->setByteStride(stride);
    m_normalAttribute->setByteOffset(3 * sizeof(float));
    m_normalAttribute->setCount(nVerts);

    m_indexAttribute->setAttributeType(Qt3DGeometry::QAttribute::IndexAttribute);
    m_indexAttribute->setVertexBaseType(Qt3DGeometry::QAttribute::UnsignedShort);
    m_indexAttribute->setBuffer(m_indexBuffer);

    m_indexAttribute->setCount(faces * 3);

    createVertexData();
    createIndexData();

    addAttribute(m_positionAttribute);
    addAttribute(m_normalAttribute);
    addAttribute(m_indexAttribute);
}

MyCylinderGeometry::~MyCylinderGeometry()
{
}

void MyCylinderGeometry::createVertexData()
{
    const int verticesCount = (m_slices + 1) * m_rings + 2 * (m_slices + 1) + 2;
    // vec3 pos, vec3 normal
    const quint32 vertexSize = (3 + 3) * sizeof(float);

    QByteArray verticesData;
    verticesData.resize(vertexSize * verticesCount);
    float *verticesPtr = reinterpret_cast<float *>(verticesData.data());

    createSidesVertices(verticesPtr);
    createDiscVertices(verticesPtr, -m_length * 0.5f);
    createDiscVertices(verticesPtr, m_length * 0.5f);

    m_vertexBuffer->setData(verticesData);
}

void MyCylinderGeometry::createSidesVertices(float *&verticesPtr) const
{
    //! [Wrong side normals]
    const float dY = m_length / static_cast<float>(m_rings - 1);
    const float dTheta = (M_PI * 2) / static_cast<float>(m_slices);

    for (int ring = 0; ring < m_rings; ++ring) {
        const float y = -m_length / 2.0f + static_cast<float>(ring) * dY;
        for (int slice = 0; slice <= m_slices; ++slice) {
            const float theta = static_cast<float>(slice) * dTheta;
            *verticesPtr++ = m_radius * qCos(theta);
            ;
            *verticesPtr++ = y;
            *verticesPtr++ = m_radius * qSin(theta);

            QVector3D n(qCos(theta), -y * 0.5f, qSin(theta));
            n.normalize();
            *verticesPtr++ = n.x();
            *verticesPtr++ = n.y();
            *verticesPtr++ = n.z();
        }
    }
    //! [Wrong side normals]
}

void MyCylinderGeometry::createDiscVertices(float *&verticesPtr, float yPosition) const
{
    const float dTheta = (M_PI * 2) / static_cast<float>(m_slices);
    const float yNormal = (yPosition < 0.0f) ? -1.0f : 1.0f;

    *verticesPtr++ = 0.0f;
    *verticesPtr++ = yPosition;
    *verticesPtr++ = 0.0f;

    *verticesPtr++ = 0.0f;
    *verticesPtr++ = yNormal;
    *verticesPtr++ = 0.0f;

    for (int slice = 0; slice <= m_slices; ++slice) {
        const float theta = static_cast<float>(slice) * dTheta;
        *verticesPtr++ = m_radius * qCos(theta);
        *verticesPtr++ = yPosition;
        *verticesPtr++ = m_radius * qSin(theta);

        *verticesPtr++ = 0.0f;
        *verticesPtr++ = yNormal;
        *verticesPtr++ = 0.0f;
    }
}

void MyCylinderGeometry::createIndexData()
{
    const int facesCount = (m_slices * 2) * (m_rings - 1) // two tris per side, for each pair of adjacent rings
        + m_slices * 2; // two caps
    const int indicesCount = facesCount * 3;
    const int indexSize = sizeof(quint16);
    Q_ASSERT(indicesCount < 65536);

    QByteArray indicesBytes;
    indicesBytes.resize(indicesCount * indexSize);
    quint16 *indicesPtr = reinterpret_cast<quint16 *>(indicesBytes.data());

    createSidesIndices(indicesPtr);
    createDiscIndices(indicesPtr, m_rings * (m_slices + 1), -m_length * 0.5);
    createDiscIndices(indicesPtr, m_rings * (m_slices + 1) + m_slices + 2, m_length * 0.5);

    m_indexBuffer->setData(indicesBytes);
}

void MyCylinderGeometry::createSidesIndices(quint16 *&indicesPtr) const
{
    for (auto ring = 0; ring < m_rings - 1; ++ring) {
        const auto ringIndexStart = ring * (m_slices + 1);
        const auto nextRingIndexStart = (ring + 1) * (m_slices + 1);

        for (auto slice = 0; slice < m_slices; ++slice) {
            const auto nextSlice = slice + 1;
            *indicesPtr++ = (ringIndexStart + slice);
            *indicesPtr++ = (nextRingIndexStart + slice);
            *indicesPtr++ = (ringIndexStart + nextSlice);
            *indicesPtr++ = (ringIndexStart + nextSlice);
            *indicesPtr++ = (nextRingIndexStart + slice);
            *indicesPtr++ = (nextRingIndexStart + nextSlice);
        }
    }
}

void MyCylinderGeometry::createDiscIndices(quint16 *&indicesPtr, int discCenterIndex, float /*yPosition*/) const
{
    //! [Wrong bottom disc indexes]
    // const auto yNormal = (yPosition < 0.0f) ? -1.0f : 1.0f;
    for (auto slice = 0; slice < m_slices; ++slice) {
        const auto nextSlice = slice + 1;
        *indicesPtr++ = discCenterIndex;
        *indicesPtr++ = (discCenterIndex + 1 + nextSlice);
        *indicesPtr++ = (discCenterIndex + 1 + slice);

        // if (yNormal < 0.0f)
        //     qSwap(*(indicesPtr -1), *(indicesPtr - 2));
    }
    //! [Wrong bottom disc indexes]
}

MyCylinder::MyCylinder(Qt3DCore::QNode *parent)
    : QGeometryRenderer(parent)
{
    setGeometry(new MyCylinderGeometry(this));
}

MyCylinder::~MyCylinder()
{
}

#include "mycylinder.moc"
