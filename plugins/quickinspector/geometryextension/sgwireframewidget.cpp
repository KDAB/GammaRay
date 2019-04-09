/*
  sgwireframewidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "sgwireframewidget.h"
#include "sggeometrymodel.h"

#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QItemSelectionModel>

using namespace GammaRay;

SGWireframeWidget::SGWireframeWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_vertexModel(nullptr)
    , m_adjacencyModel(nullptr)
    , m_highlightModel(nullptr)
    , m_positionColumn(-1)
    , m_drawingMode(0)
    , m_geometryWidth(0)
    , m_geometryHeight(0)
    , m_zoom(1)
    , m_offset(10, 10)
{
}

SGWireframeWidget::~SGWireframeWidget() = default;

void SGWireframeWidget::paintEvent(QPaintEvent *)
{
    if (!m_vertexModel || m_vertices.isEmpty() || m_positionColumn == -1)
        return;

    // Prepare painting
    m_zoom = qMin((width() - 20) / m_geometryWidth, (height() - 20) / m_geometryHeight);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(qApp->palette().color(QPalette::WindowText));
    painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));

    // Paint
    for (int i = 0; i < m_adjacencyList.size(); i++) {
        // Calculate the index of the vertex we're supposed to draw a line from
        const int index = m_adjacencyList[i];

        if (index >= m_vertices.size())
            continue;

        // Draw highlighted faces
        if ((m_drawingMode == GL_TRIANGLES && i % 3 == 2)
            || (m_drawingMode == GL_TRIANGLE_STRIP && i >= 2)) {
            drawHighlightedFace(&painter, QVector<int>() << index << m_adjacencyList[i - 1] << m_adjacencyList[i - 2]);
        } else if (m_drawingMode == GL_TRIANGLE_FAN && i >= 2) {
            drawHighlightedFace(&painter, QVector<int>() << index << m_adjacencyList[i - 1] << m_adjacencyList.first());
        }

#ifndef QT_OPENGL_ES_2
        else if ((m_drawingMode == GL_QUADS || m_drawingMode == GL_QUAD_STRIP) && i % 4 == 3) {
            drawHighlightedFace(&painter,
                                QVector<int>() << index << m_adjacencyList[i - 1] << m_adjacencyList[i - 2] << m_adjacencyList[i - 3]);
        } else if (m_drawingMode == GL_POLYGON && i == m_adjacencyList.size() - 1) {
            QVector<int> vertices;
            vertices.reserve(m_adjacencyList.size());
            for (int j = 0; j < m_adjacencyList.size(); j++)
                vertices << j;
            drawHighlightedFace(&painter, vertices);
        }
#endif

        // Draw wires

        // Draw a connection to the previous vertex
        if (((m_drawingMode == GL_LINES && i % 2)
             || m_drawingMode == GL_LINE_LOOP
             || m_drawingMode == GL_LINE_STRIP
             || (m_drawingMode == GL_TRIANGLES && i % 3)
             || m_drawingMode == GL_TRIANGLE_STRIP
             || m_drawingMode == GL_TRIANGLE_FAN
#ifndef QT_OPENGL_ES_2
             || (m_drawingMode == GL_QUADS && i % 4 != 0)
             || (m_drawingMode == GL_QUAD_STRIP && i % 2)
             || m_drawingMode == GL_POLYGON
#endif
             ) && i > 0) {
            drawWire(&painter, index, m_adjacencyList[i - 1]);
        }

        // Draw a connection to the second previous vertex
        if ((m_drawingMode == GL_TRIANGLE_STRIP
             || (m_drawingMode == GL_TRIANGLES && i % 3 == 2)
#ifndef QT_OPENGL_ES_2
             || m_drawingMode == GL_QUAD_STRIP
#endif
             ) && i > 1) {
            drawWire(&painter, index, m_adjacencyList[i - 2]);
        }

        // draw a connection to the third previous vertex
#ifndef QT_OPENGL_ES_2
        if (m_drawingMode == GL_QUADS && i % 4 == 3) {
            drawWire(&painter, index, m_adjacencyList[i - 3]);
        }

#endif

        // Draw a connection to the very first vertex
        if ((m_drawingMode == GL_LINE_LOOP && i == m_adjacencyList.size() - 1)
#ifndef QT_OPENGL_ES_2
            || (m_drawingMode == GL_POLYGON && i == m_adjacencyList.size() - 1)
#endif
            || m_drawingMode == GL_TRIANGLE_FAN)
            drawWire(&painter, index, m_adjacencyList.first());
    }

    // Paint the vertices
    for (int i = 0; i < m_vertices.size(); ++i) {
        if (m_highlightedVertices.contains(i)) {
            painter.save();

            // Glow
            QRadialGradient radialGrad(m_vertices.at(i) * m_zoom + m_offset, 6);
            radialGrad.setColorAt(0, qApp->palette().color(QPalette::Highlight));
            radialGrad.setColorAt(1, Qt::transparent);

            painter.setBrush(QBrush(radialGrad));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(m_vertices.at(i) * m_zoom + m_offset, 12, 12);

            // Highlighted point
            painter.setBrush(QBrush(qApp->palette().color(QPalette::Highlight)));
            painter.drawEllipse(m_vertices.at(i) * m_zoom + m_offset, 3, 3);
            painter.restore();
        } else {
            // Normal unhighlighted point
            painter.drawEllipse(m_vertices.at(i) * m_zoom + m_offset, 3, 3);
        }
    }

    // Paint hint about which draw mode is used
    QString drawingMode = m_drawingMode == GL_POINTS ? QStringLiteral("GL_POINTS")
                          : m_drawingMode == GL_LINES ? QStringLiteral("GL_LINES")
                          : m_drawingMode == GL_LINE_STRIP ? QStringLiteral("GL_LINE_STRIP")
                          : m_drawingMode == GL_LINE_LOOP ? QStringLiteral("GL_LINE_LOOP") :
#ifndef QT_OPENGL_ES_2
                          m_drawingMode == GL_POLYGON ? QStringLiteral("GL_POLYGON")
                          : m_drawingMode == GL_QUADS ? QStringLiteral("GL_QUADS")
                          : m_drawingMode == GL_QUAD_STRIP ? QStringLiteral("GL_QUAD_STRIP") :
#endif
                          m_drawingMode == GL_TRIANGLES ? QStringLiteral("GL_TRIANGLES")
                          : m_drawingMode == GL_TRIANGLE_STRIP ? QStringLiteral("GL_TRIANGLE_STRIP")
                          : m_drawingMode
                          == GL_TRIANGLE_FAN ? QStringLiteral("GL_TRIANGLE_FAN") : tr("Unknown");
    QString text = tr("Drawing mode: %1").arg(drawingMode);
    painter.drawText(contentsRect().width() - painter.fontMetrics().width(text),
                     contentsRect().height() - painter.fontMetrics().height(), text);
}

void SGWireframeWidget::drawWire(QPainter *painter, int vertexIndex1, int vertexIndex2)
{
    // Draw wire
    if (m_highlightedVertices.contains(vertexIndex1)
        && m_highlightedVertices.contains(vertexIndex2)) {
        painter->save();
        painter->setPen(qApp->palette().color(QPalette::Highlight));
        painter->drawLine(m_vertices.at(vertexIndex1) * m_zoom + m_offset,
                          m_vertices.at(vertexIndex2) * m_zoom + m_offset);
        painter->restore();
    } else if (vertexIndex1 != -1 && vertexIndex2 != -1) {
        painter->drawLine(m_vertices.at(vertexIndex1) * m_zoom + m_offset,
                          m_vertices.at(vertexIndex2) * m_zoom + m_offset);
    }
}

void SGWireframeWidget::drawHighlightedFace(QPainter *painter, const QVector<int> &vertexIndices)
{
    QVector<QPointF> vertices;
    for (int index : vertexIndices) {
        if (!m_highlightedVertices.contains(index))
            return; // There is one vertex that is not highlighted. Don't highlight the face.
        vertices << m_vertices.at(index) * m_zoom + m_offset;
    }
    painter->save();
    QColor color = qApp->palette().color(QPalette::Highlight).lighter();
    color.setAlphaF(0.8);
    painter->setBrush(QBrush(color));
    painter->setPen(Qt::NoPen);
    painter->drawPolygon(QPolygonF(vertices));
    painter->restore();
}

QAbstractItemModel *SGWireframeWidget::model() const
{
    return m_vertexModel;
}

void SGWireframeWidget::setModel(QAbstractItemModel *vertexModel, QAbstractItemModel *adjacencyModel)
{
    if (m_vertexModel) {
        disconnect(m_vertexModel, nullptr, this, nullptr);
    }
    m_vertexModel = vertexModel;
    m_vertexModel->rowCount();
    connect(m_vertexModel, &QAbstractItemModel::modelReset,
            this, &SGWireframeWidget::onVertexModelReset);
    connect(m_vertexModel, &QAbstractItemModel::dataChanged,
            this, &SGWireframeWidget::onVertexModelDataChanged);
    connect(m_vertexModel, &QAbstractItemModel::rowsInserted,
            this, &SGWireframeWidget::onVertexModelRowsInserted);

    if (m_adjacencyModel) {
        disconnect(m_adjacencyModel, nullptr, this, nullptr);
    }
    m_adjacencyModel = adjacencyModel;
    m_adjacencyModel->rowCount();
    connect(m_adjacencyModel, &QAbstractItemModel::modelReset,
            this, &SGWireframeWidget::onAdjacencyModelReset);
    connect(m_adjacencyModel, &QAbstractItemModel::dataChanged,
            this, &SGWireframeWidget::onAdjacencyModelDataChanged);
    connect(m_adjacencyModel, &QAbstractItemModel::rowsInserted,
            this, &SGWireframeWidget::onAdjacencyModelRowsInserted);
}

void SGWireframeWidget::setHighlightModel(QItemSelectionModel *selectionModel)
{
    if (m_highlightModel)
        disconnect(m_highlightModel, nullptr, this, nullptr);

    m_highlightModel = selectionModel;

    connect(m_highlightModel, &QItemSelectionModel::selectionChanged,
            this, &SGWireframeWidget::onHighlightDataChanged);
}

void SGWireframeWidget::onVertexModelReset()
{
    fetchVertices();
    update();
}

void SGWireframeWidget::onAdjacencyModelReset()
{
    fetchAdjacencyList();
    update();
}

void SGWireframeWidget::onVertexModelRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(first);
    Q_UNUSED(last);
    if (!parent.isValid()) {
        fetchVertices();
        update();
    }
}

void SGWireframeWidget::onAdjacencyModelRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(first);
    Q_UNUSED(last);
    if (!parent.isValid()) {
        fetchAdjacencyList();
        update();
    }
}

void SGWireframeWidget::onVertexModelDataChanged(const QModelIndex &topLeft,
                                           const QModelIndex &bottomRight)
{
    if (!topLeft.isValid()
        || !bottomRight.isValid()
        || m_positionColumn == -1
        || (topLeft.column() <= m_positionColumn && bottomRight.column() >= m_positionColumn)) {
        fetchVertices();
        update();
    }
}

void SGWireframeWidget::onAdjacencyModelDataChanged(const QModelIndex &topLeft,
                                           const QModelIndex &bottomRight)
{
    if (!topLeft.isValid()
        || !bottomRight.isValid()
        || m_positionColumn == -1
        || (topLeft.column() <= m_positionColumn && bottomRight.column() >= m_positionColumn)) {
        fetchAdjacencyList();
        update();
    }
}

void SGWireframeWidget::fetchVertices()
{
    // Get the column in which the vertex position data is stored in
    if (m_positionColumn == -1) {
        for (int j = 0; j < m_vertexModel->columnCount(); j++) {
            if (m_vertexModel->data(m_vertexModel->index(0, j),
                                SGVertexModel::IsCoordinateRole).toBool()) {
                m_positionColumn = j;
                break;
            }
        }
    }

    // Get all the vertices
    const int verticesCount = m_vertexModel->rowCount();
    m_vertices.clear();
    m_vertices.reserve(verticesCount);
    m_geometryWidth = 0;
    m_geometryHeight = 0;
    for (int i = 0; i < verticesCount; i++) {
        const QModelIndex index = m_vertexModel->index(i, m_positionColumn);
        const QVariantList data = m_vertexModel->data(index, SGVertexModel::RenderRole).toList();
        if (data.isEmpty()) {
            continue; // The rest of the data will be incomplete as well,
                      // but we need to fetch everything.
        }
        if (data.size() >= 2) {
            const qreal x = data[0].toReal();
            const qreal y = data[1].toReal();
            m_vertices << QPointF(x, y);
            if (x > m_geometryWidth)
                m_geometryWidth = x;
            if (y > m_geometryHeight)
                m_geometryHeight = y;
        }
    }
}

void SGWireframeWidget::fetchAdjacencyList()
{
    m_drawingMode = m_adjacencyModel->index(0, 0).data(SGAdjacencyModel::DrawingModeRole).toUInt();

    // Get all the wires
    m_adjacencyList.clear();
    for (int i = 0; i < m_adjacencyModel->rowCount(); i++) {
        const QModelIndex index = m_adjacencyModel->index(i, 0);
        const QVariant data = m_adjacencyModel->data(index, SGAdjacencyModel::RenderRole);
        if (!data.isValid()) {
            continue; // The rest of the data will be incomplete as well,
                      // but we need to fetch everything.
        }
        const quint32 value = data.value<quint32>();
        m_adjacencyList << value;
    }
}

void SGWireframeWidget::onHighlightDataChanged(const QItemSelection &selected,
                                               const QItemSelection &deselected)
{
    foreach (const QModelIndex &index, deselected.indexes()) {
        m_highlightedVertices.remove(index.row());
    }
    foreach (const QModelIndex &index, selected.indexes()) {
        if (!m_highlightedVertices.contains(index.row()))
            m_highlightedVertices << index.row();
    }

    update();
}

void SGWireframeWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (~e->modifiers() & Qt::ControlModifier)
        m_highlightModel->clear();

    for (int i = 0; i < m_vertices.size(); i++) {
        int distance = QLineF(e->pos(), m_vertices.at(i) * m_zoom + m_offset).length();
        if (distance <= 5) {
            if (e->modifiers() & Qt::ControlModifier) {
                m_highlightModel->select(m_vertexModel->index(i, m_positionColumn),
                                         QItemSelectionModel::Toggle);
            } else {
                m_highlightModel->select(m_vertexModel->index(i, m_positionColumn),
                                         QItemSelectionModel::Select);
            }
        }
    }

    QWidget::mouseReleaseEvent(e);
}
