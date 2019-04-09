/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>
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

#include "vtkwidget.h"
#include "objectvisualizermodel.h"

#include "common/objectmodel.h"

#include <compat/qasconst.h>

#include <QAbstractItemModel>
#include <QDebug>
#include <QItemSelectionModel>
#include <QTimer>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkSmartPointer.h>
#include <vtkGraphLayout.h>
#include <vtkGraphToPolyData.h>
#include <vtkGlyphSource2D.h>
#include <vtkGlyph3D.h>
#include <vtkGraphLayoutView.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkStringArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkVariantArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkVertexListIterator.h>
#include <vtkIdTypeArray.h>
#include <vtkLookupTable.h>
#include <vtkViewTheme.h>

#include <iostream>

using namespace GammaRay;

// #define WITH_DEBUG

#ifdef WITH_DEBUG
#define DEBUG(msg) std::cout << Q_FUNC_INFO << " " << msg << std::endl;
#else
#define DEBUG(msg) qt_noop();
#endif

#define VTK_CREATE(type, name) \
    vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

VtkWidget::VtkWidget(QWidget *parent)
    : QVTKWidget(parent)
    , m_mousePressed(false)
    , m_updateTimer(new QTimer(this))
    , m_model(nullptr)
    , m_selectionModel(nullptr)
    , m_repopulateTimer(new QTimer(this))
    , m_colorIndex(0)
{
    setupRenderer();
    setupGraph();
    show();

    m_updateTimer->setInterval(0);
    m_updateTimer->setSingleShot(true);
    connect(m_updateTimer, SIGNAL(timeout()), SLOT(renderViewImpl()));

    m_repopulateTimer->setInterval(100);
    m_repopulateTimer->setSingleShot(true);
    connect(m_repopulateTimer, SIGNAL(timeout()), SLOT(doRepopulate()));
}

VtkWidget::~VtkWidget()
{
    clear();

    DEBUG("")
}

void VtkWidget::setModel(QAbstractItemModel *model)
{
    m_model = model;

    connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
            SLOT(objectRowsInserted(QModelIndex,int,int)));

    connect(m_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            SLOT(objectRowsAboutToBeRemoved(QModelIndex,int,int)));

    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            SLOT(objectDataChanged(QModelIndex,QModelIndex)));

    doRepopulate(); // no delay here, otherwise we race against the signals
}

void VtkWidget::setSelectionModel(QItemSelectionModel *selectionModel)
{
    m_selectionModel = selectionModel;
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(selectionChanged()));
}

void VtkWidget::setupRenderer()
{
}

void VtkWidget::resetCamera()
{
    m_view->ResetCamera();
}

void VtkWidget::mousePressEvent(QMouseEvent *event)
{
    m_mousePressed = true;

    QVTKWidget::mousePressEvent(event);
}

void VtkWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_mousePressed = false;

    QVTKWidget::mouseReleaseEvent(event);
}

void VtkWidget::setupGraph()
{
    DEBUG("start")

    VTK_CREATE(vtkMutableDirectedGraph, graph);
    m_graph = graph;

    VTK_CREATE(vtkVariantArray, vertexPropertyArr);
    vertexPropertyArr->SetNumberOfValues(3);
    m_vertexPropertyArr = vertexPropertyArr;

    VTK_CREATE(vtkStringArray, vertexProp0Array);
    vertexProp0Array->SetName("labels");
    m_graph->GetVertexData()->AddArray(vertexProp0Array);

    // currently not used
    VTK_CREATE(vtkIntArray, vertexProp1Array);
    vertexProp1Array->SetName("weight");
    m_graph->GetVertexData()->AddArray(vertexProp1Array);

    // coloring
    vtkSmartPointer<vtkIntArray> vertexColors = vtkSmartPointer<vtkIntArray>::New();
    vertexColors->SetName("Color");
    m_graph->GetVertexData()->AddArray(vertexColors);

    vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
    colorLookupTable->Build();

    vtkSmartPointer<vtkViewTheme> theme = vtkSmartPointer<vtkViewTheme>::New();
    theme->SetPointLookupTable(colorLookupTable);

    vtkGraphLayoutView *graphLayoutView = vtkGraphLayoutView::New();
    graphLayoutView->AddRepresentationFromInput(graph);
    graphLayoutView->SetVertexLabelVisibility(true);
    graphLayoutView->SetVertexLabelArrayName("labels");
    graphLayoutView->SetLayoutStrategyToSpanTree();
    graphLayoutView->SetVertexColorArrayName("Color");
    graphLayoutView->SetColorVertices(true);
    graphLayoutView->ApplyViewTheme(theme);
    m_view = graphLayoutView;

    VTK_CREATE(vtkInteractorStyleTrackballCamera, style);

    vtkSmartPointer<QVTKInteractor> renderWindowInteractor = vtkSmartPointer<QVTKInteractor>::New();
    renderWindowInteractor->SetRenderWindow(graphLayoutView->GetRenderWindow());
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->Initialize();
    SetRenderWindow(graphLayoutView->GetRenderWindow());

    // code for generating edge arrow heads, needs some love
    // currently it modifies the layouting
    // how to use:
    // comment the AddRepresentationFromInput call to vtkGraphLayoutView and uncomment this
#if 0
    VTK_CREATE(vtkGraphLayout, layout);
    layout->SetInput(graph);
    layout->SetLayoutStrategy(strategy);

    // Tell the view to use the vertex layout we provide
    graphLayoutView->SetLayoutStrategyToPassThrough();
    // The arrows will be positioned on a straight line between two
    // vertices so tell the view not to draw arcs for parallel edges
    graphLayoutView->SetEdgeLayoutStrategyToPassThrough();

    // Add the graph to the view. This will render vertices and edges,
    // but not edge arrows.
    graphLayoutView->AddRepresentationFromInputConnection(layout->GetOutputPort());

    // Manually create an actor containing the glyphed arrows.
    VTK_CREATE(vtkGraphToPolyData, graphToPoly);
    graphToPoly->SetInputConnection(layout->GetOutputPort());
    graphToPoly->EdgeGlyphOutputOn();

    // Set the position (0: edge start, 1: edge end) where
    // the edge arrows should go.
    graphToPoly->SetEdgeGlyphPosition(0.98);

    // Make a simple edge arrow for glyphing.
    VTK_CREATE(vtkGlyphSource2D, arrowSource);
    arrowSource->SetGlyphTypeToEdgeArrow();
    arrowSource->SetScale(0.001);
    arrowSource->Update();

    // Use Glyph3D to repeat the glyph on all edges.
    VTK_CREATE(vtkGlyph3D, arrowGlyph);
    arrowGlyph->SetInputConnection(0, graphToPoly->GetOutputPort(1));
    arrowGlyph->SetInputConnection(1, arrowSource->GetOutputPort());

    // Add the edge arrow actor to the view.
    VTK_CREATE(vtkPolyDataMapper, arrowMapper);
    arrowMapper->SetInputConnection(arrowGlyph->GetOutputPort());
    VTK_CREATE(vtkActor, arrowActor);
    arrowActor->SetMapper(arrowMapper);
    graphLayoutView->GetRenderer()->AddActor(arrowActor);
#endif

    graphLayoutView->ResetCamera();
    graphLayoutView->Render();
    graphLayoutView->GetInteractor()->Start();

    DEBUG("end")
}

qulonglong VtkWidget::addObject(const QModelIndex &index)
{
    // ignore new objects during scene interaction
    // TODO: Add some code to add the objects later on => queue objects
    if (m_mousePressed) {
        DEBUG("Ignoring new object during scene interaction: "
              << object
              << " "
              << object->metaObject()->className())
        return 0;
    }

    qulonglong objectId = index.data(ObjectVisualizerModel::ObjectId).toULongLong();
    const QString className = index.data(ObjectVisualizerModel::ClassName).toString();
#if 0 // FIXME this breaks the graph structure since this will cause orphan children!
    if (className == "QVTKInteractorInternal")
        return 0;

#endif

    if (!objectId || m_objectIdMap.contains(objectId))
        return 0;

    if (!filterAcceptsObject(index))
        return 0;

    const QString label = index.data(ObjectVisualizerModel::ObjectDisplayName).toString();
    const int weight = 1; // TODO: Make weight somewhat usable?
    m_vertexPropertyArr->SetValue(0, vtkUnicodeString::from_utf16(label.utf16()));
    m_vertexPropertyArr->SetValue(1, weight);
    static int colorIndex = 0;
    colorIndex = colorIndex % 10;

    auto it = m_typeColorMap.constFind(className);
    if (it != m_typeColorMap.constEnd()) {
        m_vertexPropertyArr->SetValue(2, it.value());
    } else {
        m_vertexPropertyArr->SetValue(2, m_colorIndex);
        m_typeColorMap.insert(className, m_colorIndex);
        ++m_colorIndex;
    }

    const vtkIdType type = m_graph->AddVertex(m_vertexPropertyArr);
    DEBUG("Add: " << type << " " << object->metaObject()->className())
    m_objectIdMap[objectId] = type;

    // recursively add our children
    for (int i = 0; i < index.model()->rowCount(index); ++i)
        addObject(index.child(i, 0));

    // add edge to parent
    if (index.parent().isValid()) {
        const qulonglong parentId
            = index.parent().data(ObjectVisualizerModel::ObjectId).toULongLong();
        if (parentId) {
            Q_ASSERT(m_objectIdMap.contains(parentId));
            const vtkIdType parentType = m_objectIdMap.value(parentId);
            m_graph->AddEdge(parentType, type);
        }
    }

    renderView();
    return objectId;
}

bool VtkWidget::removeObject(const QModelIndex &index)
{
    for (int i = 0; i < index.model()->rowCount(index); ++i)
        removeObject(index.child(i, 0));

    const qulonglong objectId = index.data(ObjectVisualizerModel::ObjectId).toULongLong();
    return removeObjectInternal(objectId);
}

bool VtkWidget::removeObjectInternal(qulonglong objectId)
{
    if (!m_objectIdMap.contains(objectId))
        return false;

    // Remove id-for-object from VTK's graph data structure
    const vtkIdType type = m_objectIdMap[objectId];
    const int size = m_graph->GetNumberOfVertices();
    m_graph->RemoveVertex(type);

    // VTK re-orders the vertex IDs after removal!
    // we have to copy this behavior to track the associated QObject instances
    const vtkIdType lastId = m_objectIdMap.size() - 1;
    DEBUG("Type: " << type << " Last: " << lastId)
    if (type != lastId) {
        qulonglong lastObjectId = m_objectIdMap.key(lastId);
        Q_ASSERT(lastObjectId);
        m_objectIdMap[lastObjectId] = type;
    }

    // Remove object from our map
    if (size > m_graph->GetNumberOfVertices())
        Q_ASSERT(m_objectIdMap.remove(objectId) == 1);
    else
        DEBUG("Warning: Should not happen: Could not remove vertice with id: " << type)

    renderView();
    return true;
}

/// Schedules the re-rendering of the VTK view
void VtkWidget::renderView()
{
    m_updateTimer->start();
}

void VtkWidget::clear()
{
    // TODO: there must be an easier/faster way to clean the graph data
    // Just re-create the vtk graph data object?
    for (const qulonglong &objectId : qAsConst(m_objectIdMap)) {
        removeObjectInternal(objectId);
    }
    m_objectIdMap.clear();

    renderView();
}

void VtkWidget::renderViewImpl()
{
    DEBUG("")

    m_view->Render();
    m_view->ResetCamera();
}

void VtkWidget::selectionChanged()
{
    repopulate();
    resetCamera();
}

void VtkWidget::repopulate()
{
    if (!m_repopulateTimer->isActive())
        m_repopulateTimer->start();
}

void VtkWidget::doRepopulate()
{
    DEBUG("")

    clear();

    for (int i = 0; i < m_model->rowCount(); ++i) {
        const QModelIndex index = m_model->index(i, 0);
        addObject(index);
    }
}

// TODO: Move to Util.h?
static bool descendantOf(const QModelIndex &ascendant, const QModelIndex &index)
{
    const QModelIndex parent = index.parent();
    if (!parent.isValid())
        return false;
    if (parent == ascendant)
        return true;
    return descendantOf(ascendant, parent);
}

static QModelIndex mapToSource(const QModelIndex &proxyIndex)
{
    if (proxyIndex.model()->inherits("GammaRay::ObjectVisualizerModel"))
        return proxyIndex;

    const QAbstractProxyModel *proxyModel
        = qobject_cast<const QAbstractProxyModel *>(proxyIndex.model());

    if (proxyModel)
        return mapToSource(proxyModel->mapToSource(proxyIndex));
    else
        return proxyIndex;
}

bool VtkWidget::filterAcceptsObject(const QModelIndex &index) const
{
    if (!m_selectionModel)
        return true;

    const QModelIndexList rows = m_selectionModel->selectedRows();
    for (const QModelIndex &row : rows) {
        const QModelIndex sourceRow = mapToSource(row);
        if (index == sourceRow)
            return true;
        return descendantOf(sourceRow, index);
    }

    return true; // empty selection
}

void VtkWidget::objectRowsInserted(const QModelIndex &parent, int start, int end)
{
    for (int i = start; i <= end; ++i) {
        const QModelIndex index = m_model->index(i, 0, parent);
        addObject(index);
    }
}

void VtkWidget::objectRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    for (int i = start; i <= end; ++i) {
        const QModelIndex index = m_model->index(i, 0, parent);
        removeObject(index);
    }
}

void VtkWidget::objectDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        const QModelIndex index = m_model->index(i, 0, topLeft.parent());
        addObject(index);
    }
}
