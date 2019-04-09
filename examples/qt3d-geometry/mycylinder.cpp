/*
  mycylinder.cpp

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

#include "mycylinder.h"

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>

#include <QVector3D>
#include <qmath.h>

class MyCylinderGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT
public:
    explicit MyCylinderGeometry(QNode *parent = nullptr);
    ~MyCylinderGeometry();

private:
    void createVertexData();
    void createSidesVertices(float *&verticesPtr);
    void createDiscVertices(float *&verticesPtr, float yPosition);
    void createIndexData();
    void createSidesIndices(quint16 *&indicesPtr);
    void createDiscIndices(quint16 *&indicesPtr, int discCenterIndex, float yPosition);

    Qt3DRender::QAttribute *m_positionAttribute;
    Qt3DRender::QAttribute *m_normalAttribute;
    Qt3DRender::QAttribute *m_indexAttribute;

    Qt3DRender::QBuffer *m_vertexBuffer;
    Qt3DRender::QBuffer *m_indexBuffer;

    int m_rings = 7;
    int m_slices = 16;
    float m_radius = 1.0f;
    float m_length = 4.0f;
};

MyCylinderGeometry::MyCylinderGeometry(QNode* parent)
    : QGeometry(parent)
{
    m_positionAttribute = new Qt3DRender::QAttribute(this);
    m_normalAttribute = new Qt3DRender::QAttribute(this);
    m_indexAttribute = new Qt3DRender::QAttribute(this);

    m_vertexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this);
    m_indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, this);

    // vec3 pos, vec3 normal
    const quint32 elementSize = 3 + 3;
    const quint32 stride = elementSize * sizeof(float);
    const int nVerts = (m_slices + 1) * m_rings + 2 * (m_slices + 1) + 2;
    const int faces = (m_slices * 2) * (m_rings - 1) + (m_slices * 2);

    m_positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    m_positionAttribute->setDataType(Qt3DRender::QAttribute::Float);
    m_positionAttribute->setDataSize(3);
    m_positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_positionAttribute->setBuffer(m_vertexBuffer);
    m_positionAttribute->setByteStride(stride);
    m_positionAttribute->setCount(nVerts);

    m_normalAttribute->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());
    m_normalAttribute->setDataType(Qt3DRender::QAttribute::Float);
    m_normalAttribute->setDataSize(3);
    m_normalAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_normalAttribute->setBuffer(m_vertexBuffer);
    m_normalAttribute->setByteStride(stride);
    m_normalAttribute->setByteOffset(3 * sizeof(float));
    m_normalAttribute->setCount(nVerts);

    m_indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    m_indexAttribute->setDataType(Qt3DRender::QAttribute::UnsignedShort);
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
    const int verticesCount  = (m_slices + 1) * m_rings + 2 * (m_slices + 1) + 2;
    // vec3 pos, vec3 normal
    const quint32 vertexSize = (3 + 3) * sizeof(float);

    QByteArray verticesData;
    verticesData.resize(vertexSize * verticesCount);
    float *verticesPtr = reinterpret_cast<float*>(verticesData.data());

    createSidesVertices(verticesPtr);
    createDiscVertices(verticesPtr, -m_length * 0.5f);
    createDiscVertices(verticesPtr, m_length * 0.5f);

    m_vertexBuffer->setData(verticesData);
}

void MyCylinderGeometry::createSidesVertices(float*& verticesPtr)
{
    //! [Wrong side normals]
    const float dY = m_length / static_cast<float>(m_rings - 1);
    const float dTheta = (M_PI * 2) / static_cast<float>(m_slices);

    for (int ring = 0; ring < m_rings; ++ring) {
        const float y = -m_length / 2.0f + static_cast<float>(ring) * dY;
        for (int slice = 0; slice <= m_slices; ++slice) {
            const float theta = static_cast<float>(slice) * dTheta;
            *verticesPtr++ = m_radius * qCos(theta);;
            *verticesPtr++ = y;
            *verticesPtr++ = m_radius * qSin(theta);

            QVector3D n(qCos(theta), -y *0.5f, qSin(theta));
            n.normalize();
            *verticesPtr++ = n.x();
            *verticesPtr++ = n.y();
            *verticesPtr++ = n.z();
        }
    }
    //! [Wrong side normals]
}

void MyCylinderGeometry::createDiscVertices(float *&verticesPtr, float yPosition)
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
    quint16 *indicesPtr = reinterpret_cast<quint16*>(indicesBytes.data());

    createSidesIndices(indicesPtr);
    createDiscIndices(indicesPtr, m_rings * (m_slices + 1), -m_length * 0.5);
    createDiscIndices(indicesPtr, m_rings * (m_slices + 1) + m_slices + 2, m_length * 0.5);

    m_indexBuffer->setData(indicesBytes);
}

void MyCylinderGeometry::createSidesIndices(quint16 *&indicesPtr)
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

void MyCylinderGeometry::createDiscIndices(quint16 *&indicesPtr, int discCenterIndex, float /*yPosition*/)
{
    //! [Wrong bottom disc indexes]
    //const auto yNormal = (yPosition < 0.0f) ? -1.0f : 1.0f;
    for (auto slice = 0; slice < m_slices; ++slice) {
        const auto nextSlice = slice + 1;
        *indicesPtr++ = discCenterIndex;
        *indicesPtr++ = (discCenterIndex + 1 + nextSlice);
        *indicesPtr++ = (discCenterIndex + 1 + slice);

        //if (yNormal < 0.0f)
        //    qSwap(*(indicesPtr -1), *(indicesPtr - 2));
    }
    //! [Wrong bottom disc indexes]
}

MyCylinder::MyCylinder(Qt3DCore::QNode* parent)
    : QGeometryRenderer(parent)
{
    setGeometry(new MyCylinderGeometry(this));
}

MyCylinder::~MyCylinder()
{
}

#include "mycylinder.moc"
