/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_VTKWIDGET_H
#define GAMMARAY_VTKWIDGET_H

#include <QVTKWidget.h>

#include <vtkSmartPointer.h>

#include <QMap>
#include <QSet>

class vtkGraphLayoutStrategy;
class vtkVariantArray;
class vtkGraphLayoutView;
class vtkMutableDirectedGraph;

class QMouseEvent;
class QTimer;

namespace GammaRay {

class VtkWidget : public QVTKWidget
{
  Q_OBJECT

  public:
    explicit VtkWidget(QWidget *parent = 0);
    virtual ~VtkWidget();

    QObject *objectFilter() const
    {
      return m_objectFilter;
    }

    vtkGraphLayoutView *layoutView() const
    {
      return m_view;
    }

  public Q_SLOTS:
    void resetCamera();

    bool addObject(QObject *);
    bool removeObject(QObject *);

    void setObjectFilter(QObject *object);

    void clear();
    void repopulate();

  private Q_SLOTS:
    bool addObjectInternal(QObject *object);
    bool removeObjectInternal(QObject *object);

    void renderViewImpl();
    void renderView();

  protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    bool filterAcceptsObject(QObject *object) const;

  private:
    void setupGraph();
    void setupRenderer();

    bool m_mousePressed;
    QTimer *m_updateTimer;
    QObject *m_objectFilter;

    // TODO: Instead of tracking all available objects, make Probe::m_validObjects public?
    QSet<QObject*> m_availableObjects;
    QMap<QObject *, vtkIdType> m_objectIdMap;

    int m_colorIndex;
    QMap<QString, int> m_typeColorMap;

    vtkSmartPointer<vtkVariantArray> m_vertexPropertyArr;
    vtkGraphLayoutView *m_view;
    vtkSmartPointer<vtkMutableDirectedGraph> m_graph;
};

}

#endif // GAMMARAY_VTKWIDGET_H
