/*
  sgwireframewidget.h

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

#ifndef GAMMARAY_QUICKINSPECTOR_SGWIREFRAMEWIDGET_H
#define GAMMARAY_QUICKINSPECTOR_SGWIREFRAMEWIDGET_H

#include <QWidget>
#include <qopengl.h>

class QItemSelection;
class QPainter;
class QItemSelectionModel;
class QAbstractItemModel;
class QModelIndex;

namespace GammaRay {

class SGWireframeWidget : public QWidget
{
  Q_OBJECT

  public:
    explicit SGWireframeWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~SGWireframeWidget();

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *m_model);
    void setHighlightModel(QItemSelectionModel *selectionModel);

  public slots:
    void onGeometryChanged(uint drawingMode, QByteArray indexData, int indexType);

  protected:
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent*) Q_DECL_OVERRIDE;

  private slots:
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onHighlightDataChanged(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    void drawWire(QPainter *painter, int vertexIndex1, int vertexIndex2);
    void drawHighlightedFace(QPainter *painter, QVector< int > vertexIndices);

  private:
    QAbstractItemModel *m_model;
    int m_positionColumn;
    GLenum m_drawingMode;
    QByteArray m_indexData;
    int m_indexType;
    QItemSelectionModel *m_highlightModel;
    QVector<QPointF> m_vertices;
    QVector<int> m_highlightedVertices;
    qreal m_geometryWidth;
    qreal m_geometryHeight;
    qreal m_zoom;
    const QPointF m_offset;
};

}

#endif // SGWIREFRAMEWIDGET_H
