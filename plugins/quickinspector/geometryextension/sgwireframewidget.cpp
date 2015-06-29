/*
  sgwireframewidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
  : QWidget(parent, f),
    m_model(0),
    m_positionColumn(-1),
    m_drawingMode(0),
    m_indexType(0),
    m_highlightModel(0),
    m_geometryWidth(0),
    m_geometryHeight(0),
    m_zoom(1),
    m_offset(10, 10)
{
}

SGWireframeWidget::~SGWireframeWidget()
{
}

void SGWireframeWidget::paintEvent(QPaintEvent *)
{
  if (!m_model || m_vertices.isEmpty() || m_positionColumn == -1
      || !(m_indexType == GL_UNSIGNED_INT
           || m_indexType == GL_UNSIGNED_SHORT
           || m_indexType == GL_UNSIGNED_BYTE)) {
    return;
  }

  // Prepare painting
  m_zoom = qMin((width() - 20) / m_geometryWidth, (height() - 20) / m_geometryHeight);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(qApp->palette().color(QPalette::WindowText));
  painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));

  // Paint
  int prevIndex1 = -1; // Magic value: The initial value should never used, we set it for
  int prevIndex2 = -1; // detection only, just in case something goes terribly wrong.
  int prevIndex3 = -1;
  int firstIndex;

  // Calculate the size of one VBO index
  int indexSize = 0;
  if (m_indexType == GL_UNSIGNED_INT) {
    indexSize = sizeof(quint32);
  } else if (m_indexType == GL_UNSIGNED_SHORT) {
    indexSize = sizeof(quint16);
  } else if (m_indexType == GL_UNSIGNED_BYTE) {
    indexSize = sizeof(quint8);
  }

  const int count = m_indexData.isEmpty() ? m_vertices.count() : m_indexData.size() / indexSize;

  for (int i = 0; i < count; i++) {
    // Calculate the index of the vertex we're supposed to draw a line from
    int index = i;
    if (!m_indexData.isEmpty()) {
      if (m_indexType == GL_UNSIGNED_INT) {
        index = *reinterpret_cast<const quint32*>(m_indexData.constData() + i * indexSize);
      } else if (m_indexType == GL_UNSIGNED_SHORT) {
        index = *reinterpret_cast<const quint16*>(m_indexData.constData() + i * indexSize);
      } else if (m_indexType == GL_UNSIGNED_BYTE) {
        index = *reinterpret_cast<const quint8*>(m_indexData.constData() + i * indexSize);
      }
    }
    if (i == 0) {
      firstIndex = index;
    }

    if (index >= m_vertices.size()) {
      continue;
    }

    // Draw highlighted faces
    if ((m_drawingMode == GL_TRIANGLES && i % 3 == 2) ||
        (m_drawingMode == GL_TRIANGLE_STRIP && i >= 2)) {
      drawHighlightedFace(&painter, QVector<int>() << index << prevIndex1 << prevIndex2);
    } else if (m_drawingMode == GL_TRIANGLE_FAN && i >= 2) {
      drawHighlightedFace(&painter, QVector<int>() << index << prevIndex1 << firstIndex);
    }

#ifndef QT_OPENGL_ES_2
    else if ((m_drawingMode == GL_QUADS || m_drawingMode == GL_QUAD_STRIP) && i % 4 == 3) {
      drawHighlightedFace(&painter,
                          QVector<int>() << index << prevIndex1 << prevIndex2 << prevIndex3);
    }

    else if (m_drawingMode == GL_POLYGON && i == count - 1) {
      QVector<int> vertices;
      vertices.reserve(count);
      for (int j = 0; j < count; j++) {
        vertices << j;
      }
      drawHighlightedFace(&painter, vertices);
    }
#endif

    // Draw wires

    // Draw a connection to the previous vertex
    if (((m_drawingMode == GL_LINES && i % 2) ||
         m_drawingMode == GL_LINE_LOOP ||
         m_drawingMode == GL_LINE_STRIP ||
         (m_drawingMode == GL_TRIANGLES && i % 3) ||
         m_drawingMode == GL_TRIANGLE_STRIP ||
         m_drawingMode == GL_TRIANGLE_FAN
#ifndef QT_OPENGL_ES_2
      || (m_drawingMode == GL_QUADS && i % 4 != 0) ||
         (m_drawingMode == GL_QUAD_STRIP && i % 2) ||
         m_drawingMode == GL_POLYGON
#endif
    ) && i > 0) {
      drawWire(&painter, index, prevIndex1);
    }

    // Draw a connection to the second previous vertex
    if ((m_drawingMode == GL_TRIANGLE_STRIP ||
         (m_drawingMode == GL_TRIANGLES && i % 3 == 2)
#ifndef QT_OPENGL_ES_2
      || m_drawingMode == GL_QUAD_STRIP
#endif
    ) && i > 1) {
      drawWire(&painter, index, prevIndex2);
    }

    // draw a connection to the third previous vertex
#ifndef QT_OPENGL_ES_2
    if (m_drawingMode == GL_QUADS && i % 4 == 3) {
      drawWire(&painter, index, prevIndex3);
    }
#endif

    // Draw a connection to the very first vertex
    if ((m_drawingMode == GL_LINE_LOOP && i == count - 1)
#ifndef QT_OPENGL_ES_2
      || (m_drawingMode == GL_POLYGON && i == count - 1)
#endif
      || m_drawingMode == GL_TRIANGLE_FAN) {
      drawWire(&painter, index, firstIndex);
    }

    prevIndex3 = prevIndex2;
    prevIndex2 = prevIndex1;
    prevIndex1 = index;
  }

  // Paint the vertices
  for (int i = 0; i < m_vertices.size(); ++i) {
    if (m_highlightedVertices.contains(i)) {
      painter.save();

      // Glow
      QRadialGradient radialGrad(m_vertices[i] * m_zoom + m_offset, 6);
      radialGrad.setColorAt(0, qApp->palette().color(QPalette::Highlight));
      radialGrad.setColorAt(1, Qt::transparent);

      painter.setBrush(QBrush(radialGrad));
      painter.setPen(Qt::NoPen);
      painter.drawEllipse(m_vertices[i] * m_zoom + m_offset, 12, 12);

      // Highlighted point
      painter.setBrush(QBrush(qApp->palette().color(QPalette::Highlight)));
      painter.drawEllipse(m_vertices[i] * m_zoom + m_offset, 3, 3);
      painter.restore();
    } else {
      // Normal unhighlighted point
      painter.drawEllipse(m_vertices[i] * m_zoom + m_offset, 3, 3);
    }
  }

  // Paint hint about which draw mode is used
  QString drawingMode = m_drawingMode == GL_POINTS              ? "GL_POINTS" :
                        m_drawingMode == GL_LINES               ? "GL_LINES" :
                        m_drawingMode == GL_LINE_STRIP          ? "GL_LINE_STRIP" :
                        m_drawingMode == GL_LINE_LOOP           ? "GL_LINE_LOOP" :
#ifndef QT_OPENGL_ES_2
                        m_drawingMode == GL_POLYGON             ? "GL_POLYGON" :
                        m_drawingMode == GL_QUADS               ? "GL_QUADS" :
                        m_drawingMode == GL_QUAD_STRIP          ? "GL_QUAD_STRIP" :
#endif
                        m_drawingMode == GL_TRIANGLES           ? "GL_TRIANGLES" :
                        m_drawingMode == GL_TRIANGLE_STRIP      ? "GL_TRIANGLE_STRIP" :
                        m_drawingMode == GL_TRIANGLE_FAN        ? "GL_TRIANGLE_FAN" : "Unknown";
  QString text = QObject::tr("Drawing mode: %1").arg(drawingMode);
  painter.drawText(contentsRect().width() - painter.fontMetrics().width(text),
                   contentsRect().height() - painter.fontMetrics().height(), text);
}

void SGWireframeWidget::drawWire(QPainter *painter, int vertexIndex1, int vertexIndex2)
{
  // Draw wire
  if (m_highlightedVertices.contains(vertexIndex1) &&
      m_highlightedVertices.contains(vertexIndex2)) {
    painter->save();
    painter->setPen(qApp->palette().color(QPalette::Highlight));
    painter->drawLine(m_vertices[vertexIndex1] * m_zoom + m_offset,
                      m_vertices[vertexIndex2] * m_zoom + m_offset);
    painter->restore();
  } else if (vertexIndex1 != -1 && vertexIndex2 != -1) {
    painter->drawLine(m_vertices[vertexIndex1] * m_zoom + m_offset,
                      m_vertices[vertexIndex2] * m_zoom + m_offset);
  }
}

void SGWireframeWidget::drawHighlightedFace(QPainter *painter, QVector<int> vertexIndices)
{
  QVector<QPointF> vertices;
  foreach (int index, vertexIndices) {
    if (!m_highlightedVertices.contains(index)) {
      return; // There is one vertex that is not highlighted. Don't highlight the face.
    }
    vertices << m_vertices[index] * m_zoom + m_offset;
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
  return m_model;
}

void SGWireframeWidget::setModel(QAbstractItemModel *model)
{
  if (m_model) {
    disconnect(m_model, 0, this, 0);
  }
  m_model = model;
  connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
          this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
}

void SGWireframeWidget::setHighlightModel(QItemSelectionModel *selectionModel)
{
  if (m_highlightModel) {
    disconnect(m_highlightModel, 0, this, 0);
  }

  m_highlightModel = selectionModel;

  connect(m_highlightModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(onHighlightDataChanged(QItemSelection,QItemSelection)));
}

void SGWireframeWidget::onModelDataChanged(const QModelIndex &topLeft,
                                           const QModelIndex &bottomRight)
{
  if (!topLeft.isValid() ||
      !bottomRight.isValid() ||
      m_positionColumn == -1 ||
      (topLeft.column() <= m_positionColumn && bottomRight.column() >= m_positionColumn)) {
    // Get the column in which the vertex position data is stored in
    if (m_positionColumn == -1) {
      for (int j = 0; j < m_model->columnCount(); j++) {
        if (m_model->data(m_model->index(0, j), SGGeometryModel::IsCoordinateRole).toBool()) {
          m_positionColumn = j;
          break;
        }
      }
    }

    // Get all the vertices
    m_vertices.clear();
    m_geometryWidth = 0;
    m_geometryHeight = 0;
    for (int i = 0; i < m_model->rowCount(); i++) {
      const QModelIndex index = m_model->index(i, m_positionColumn);
      const QVariantList data = m_model->data(index, SGGeometryModel::RenderRole).toList();
      if (data.isEmpty()) {
        // Data is incomplete, so no need to render the wireframe yet.
        // It will be repainted as soon as the data is available.
        return;
      }
      if (data.size() >= 2) {
        const qreal x = data[0].toReal();
        const qreal y = data[1].toReal();
        m_vertices << QPointF(x, y);
        if (x > m_geometryWidth) {
          m_geometryWidth = x;
        }
        if (y > m_geometryHeight) {
          m_geometryHeight = y;
        }
      }
    }

    update();
  }
}

void SGWireframeWidget::onHighlightDataChanged(const QItemSelection &selected,
                                               const QItemSelection &deselected)
{
  foreach (const QModelIndex &index, deselected.indexes()) {
    int i = m_highlightedVertices.indexOf(index.row());
    if (i != -1) {
      m_highlightedVertices.remove(i);
    }
  }
  foreach (const QModelIndex &index, selected.indexes()) {
    if (!m_highlightedVertices.contains(index.row())) {
      m_highlightedVertices << index.row();
    }
  }

  update();
}

void SGWireframeWidget::onGeometryChanged(uint drawingMode, QByteArray indexData, int indexType)
{
  m_drawingMode = drawingMode;
  m_indexData = indexData;
  m_indexType = indexType;
  m_vertices.clear();
  m_highlightedVertices.clear();
  m_highlightModel->clear();
}

void SGWireframeWidget::mouseReleaseEvent(QMouseEvent *e)
{
  if (~e->modifiers() & Qt::ControlModifier) {
    m_highlightModel->clear();
  }

  for (int i = 0; i < m_vertices.size(); i++) {
    int distance = QLineF(e->pos(), m_vertices[i] * m_zoom + m_offset).length();
    if (distance <= 5) {
      if (e->modifiers() & Qt::ControlModifier) {
        m_highlightModel->select(m_model->index(i, m_positionColumn), QItemSelectionModel::Toggle);
      } else {
        m_highlightModel->select(m_model->index(i, m_positionColumn), QItemSelectionModel::Select);
      }
    }
  }

  QWidget::mouseReleaseEvent(e);
}
