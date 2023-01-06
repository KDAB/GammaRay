/*
  sgwireframewidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_SGWIREFRAMEWIDGET_H
#define GAMMARAY_QUICKINSPECTOR_SGWIREFRAMEWIDGET_H

#include <QWidget>
#include <qopengl.h>
#include <QSet>

QT_BEGIN_NAMESPACE
class QItemSelection;
class QPainter;
class QItemSelectionModel;
class QAbstractItemModel;
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class SGWireframeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SGWireframeWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~SGWireframeWidget() override;

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *vertexModel, QAbstractItemModel *adjacencyModel);
    void setHighlightModel(QItemSelectionModel *selectionModel);

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

private slots:
    void onVertexModelReset();
    void onAdjacencyModelReset();
    void onVertexModelRowsInserted(const QModelIndex &parent, int first, int last);
    void onAdjacencyModelRowsInserted(const QModelIndex &parent, int first, int last);
    void onVertexModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onAdjacencyModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onHighlightDataChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    void drawWire(QPainter *painter, int vertexIndex1, int vertexIndex2);
    void drawHighlightedFace(QPainter *painter, const QVector<int> &vertexIndices);
    void fetchVertices();
    void fetchAdjacencyList();

private:
    QAbstractItemModel *m_vertexModel;
    QAbstractItemModel *m_adjacencyModel;
    QItemSelectionModel *m_highlightModel;
    int m_positionColumn;
    GLenum m_drawingMode;
    QVector<QPointF> m_vertices;
    QSet<int> m_highlightedVertices;
    QVector<int> m_adjacencyList;

    qreal m_geometryWidth;
    qreal m_geometryHeight;
    qreal m_zoom;
    const QPointF m_offset;
};
}

#endif // SGWIREFRAMEWIDGET_H
