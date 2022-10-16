/*
  vtkwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTVISUALIZER_VTKWIDGET_H
#define GAMMARAY_OBJECTVISUALIZER_VTKWIDGET_H

#include <QVTKWidget.h>

#include <vtkSmartPointer.h>

#include <QMap>
#include <QSet>

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
class QModelIndex;
class QAbstractItemModel;
class QMouseEvent;
class QTimer;
QT_END_NAMESPACE

class vtkGraphLayoutStrategy;
class vtkVariantArray;
class vtkGraphLayoutView;
class vtkMutableDirectedGraph;

namespace GammaRay {
class VtkWidget : public QVTKWidget
{
    Q_OBJECT

public:
    explicit VtkWidget(QWidget *parent = nullptr);
    virtual ~VtkWidget();

    vtkGraphLayoutView *layoutView() const
    {
        return m_view;
    }

    void setModel(QAbstractItemModel *model);
    void setSelectionModel(QItemSelectionModel *selectionModel);

public Q_SLOTS:
    void resetCamera();

    qulonglong addObject(const QModelIndex &index);
    bool removeObject(const QModelIndex &index);

    void clear();
    void repopulate();

private Q_SLOTS:
    bool removeObjectInternal(qulonglong objectId);
    void doRepopulate();
    void selectionChanged();

    void renderViewImpl();
    void renderView();

    void objectRowsInserted(const QModelIndex &parent, int start, int end);
    void objectRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void objectDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    bool filterAcceptsObject(const QModelIndex &index) const;

private:
    void setupGraph();
    void setupRenderer();

    bool m_mousePressed;
    QTimer *m_updateTimer;
    QAbstractItemModel *m_model;
    QItemSelectionModel *m_selectionModel;
    QTimer *m_repopulateTimer;

    // TODO: Instead of tracking all available objects, make Probe::m_validObjects public?
    QMap<qulonglong, vtkIdType> m_objectIdMap;

    int m_colorIndex;
    QMap<QString, int> m_typeColorMap;

    vtkSmartPointer<vtkVariantArray> m_vertexPropertyArr;
    vtkGraphLayoutView *m_view;
    vtkSmartPointer<vtkMutableDirectedGraph> m_graph;
};
}

#endif // GAMMARAY_VTKWIDGET_H
