/*
  sgwireframewidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

using namespace GammaRay;

SGWireframeWidget::SGWireframeWidget(QWidget* parent, Qt::WindowFlags f)
  : QWidget(),
    m_model(0),
    m_positionColumn(-1),
    m_drawingMode(0)
{
}

SGWireframeWidget::~SGWireframeWidget()
{

}

void SGWireframeWidget::paintEvent(QPaintEvent* )
{
  if (!m_model)
    return;

  // Get the column in which the vertex position data is stored in
  if (m_positionColumn == -1) {
    for(int j = 0; j <= m_model->columnCount(); j++) {
        if (m_model->data(m_model->index(0, j), SGGeometryModel::IsCoordinateRole).toBool()) {
          m_positionColumn = j;
          break;
        }
    }
  }
  if (m_positionColumn == -1)
    return;

  // Get all the vertices
  QVector<QPointF> vertices;
  qreal w = 0;
  qreal h = 0;
  bool isComplete = true;

  for (int i = 0; i < m_model->rowCount(); i++) {
    QModelIndex index = m_model->index(i, m_positionColumn);
    QVariantList data = m_model->data(index, SGGeometryModel::RenderRole).toList();
    if (data.isEmpty()) // Data is incomplete, so no need to render the wireframe yet. It will be repainted as soon as the data is available.
      return;
    if (data.size() >= 2) {
      qreal x = data[0].toReal();
      qreal y = data[1].toReal();
      vertices << QPointF(x, y);
      if (x > w)
        w = x;
      if (y > h)
        h = y;
    }
  }


  // Paint the vertices
  QPointF shift(10, 10);
  qreal zoom = qMin((width() - 20) / w, (height() - 20) / h);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(qApp->palette().color(QPalette::WindowText));
  painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));

  foreach (QPointF vertex, vertices)
    painter.drawEllipse(vertex * zoom + shift, 3, 3);

  // Paint the wires
  QPointF prevVertex1;
  QPointF prevVertex2;
  QPointF prevVertex3;

  int indexSize = 0;
  if (m_indexType == GL_UNSIGNED_INT)
    indexSize = sizeof(quint32);
  else if (m_indexType == GL_UNSIGNED_SHORT)
    indexSize = sizeof(quint16);
  else if (m_indexType == GL_UNSIGNED_BYTE)
    indexSize = sizeof(quint8);

  int count = m_indexData.isEmpty() ? vertices.count() : m_indexData.size() / indexSize;
  int i = 0;

  for (int i = 0; i < count; i++) {
    int index = i;
    if (!m_indexData.isEmpty()) {
      if (m_indexType == GL_UNSIGNED_INT)
        index = *reinterpret_cast<const quint32*>(m_indexData.constData() + i * indexSize);
      else if (m_indexType == GL_UNSIGNED_SHORT)
        index = *reinterpret_cast<const quint16*>(m_indexData.constData() + i * indexSize);
      else if (m_indexType == GL_UNSIGNED_BYTE)
        index = *reinterpret_cast<const quint8*>(m_indexData.constData() + i * indexSize);
    }
    QPointF vertex = vertices[index];


    if (((m_drawingMode == GL_LINES && i % 2)
      || m_drawingMode == GL_LINE_LOOP
      || m_drawingMode == GL_LINE_STRIP
      ||(m_drawingMode == GL_TRIANGLES && i % 3)
      || m_drawingMode == GL_TRIANGLE_STRIP
      || m_drawingMode == GL_TRIANGLE_FAN
#ifndef QT_OPENGL_ES_2
      ||(m_drawingMode == GL_QUADS && i % 4 != 0)
      ||(m_drawingMode == GL_QUAD_STRIP && i % 2)
      || m_drawingMode == GL_POLYGON
#endif
    ) && i > 0) {

      // draw a connection to the last vertex
      painter.drawLine(prevVertex1 * zoom + shift, vertex * zoom + shift);
    }
    if ((m_drawingMode == GL_TRIANGLE_STRIP
      ||(m_drawingMode == GL_TRIANGLES && i % 3 == 2)
#ifndef QT_OPENGL_ES_2
      || m_drawingMode == GL_QUAD_STRIP
#endif
    ) && i > 1) {
      // draw a connection to the second last vertex
      painter.drawLine(prevVertex2 * zoom + shift, vertex * zoom + shift);
    }
#ifndef QT_OPENGL_ES_2
    if (m_drawingMode == GL_QUADS && i % 4 == 3) {
      // draw a connection to the second last vertex
      painter.drawLine(prevVertex3 * zoom + shift, vertex * zoom + shift);
    }
#endif
    if ((m_drawingMode == GL_LINE_LOOP && i == count - 1)
#ifndef QT_OPENGL_ES_2
      || (m_drawingMode == GL_POLYGON && i == count - 1)
#endif
      || m_drawingMode == GL_TRIANGLE_FAN) {
      // draw a connection to the first vertex
      painter.drawLine(vertices[0] * zoom + shift, vertex * zoom + shift);
    }
    prevVertex3 = prevVertex2;
    prevVertex2 = prevVertex1;
    prevVertex1 = vertex;
  }
}

QAbstractItemModel *SGWireframeWidget::model() const
{
  return m_model;
}

void SGWireframeWidget::setModel(QAbstractItemModel *model)
{
  if (m_model)
    disconnect(m_model, 0, this, 0);
  m_model = model;
  connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
}

void SGWireframeWidget::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  if (!topLeft.isValid() || !bottomRight.isValid() || m_positionColumn == -1 || (topLeft.column() <= m_positionColumn && bottomRight.column() >= m_positionColumn)) {
    update();
  }
}

void SGWireframeWidget::onGeometryChanged(uint drawingMode, QByteArray indexData, int indexType)
{
  m_drawingMode = drawingMode;
  m_indexData = indexData;
  m_indexType = indexType;
}
