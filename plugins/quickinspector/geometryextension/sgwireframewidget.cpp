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
#include <QPainter>

using namespace GammaRay;

SGWireframeWidget::SGWireframeWidget(QWidget* parent, Qt::WindowFlags f)
  : QWidget(),
    m_model(0),
    m_positionColumn(-1)
{
}

SGWireframeWidget::~SGWireframeWidget()
{

}

void SGWireframeWidget::paintEvent(QPaintEvent* )
{
  if (!m_model)
    return;
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

  QPointF shift(10, 10);
  qreal zoom = qMin((width() - 20) / w, (height() - 20) / h);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::black);
  painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));

  QPointF prevVertex1;
  QPointF prevVertex2;
  int i = 0;
  foreach (QPointF vertex, vertices) {
    painter.drawEllipse(vertex * zoom + shift, 3, 3);
    if (i > 0)
      painter.drawLine(prevVertex1 * zoom + shift, vertex * zoom + shift);
    if (i > 1)
      painter.drawLine(prevVertex2 * zoom + shift, vertex * zoom + shift);
    prevVertex2 = prevVertex1;
    prevVertex1 = vertex;
    i++;
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
  connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
}

void SGWireframeWidget::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  if (!topLeft.isValid() || !bottomRight.isValid() || m_positionColumn == -1 || (topLeft.column() <= m_positionColumn && bottomRight.column() >= m_positionColumn)) {
    update();
  }
}
