/*
  modelinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MODELINSPECTOR_MODELINSPECTOR_H
#define GAMMARAY_MODELINSPECTOR_MODELINSPECTOR_H

#include "toolfactory.h"

#include "common/modelinspectorinterface.h"

class QItemSelection;
class QItemSelectionModel;

namespace GammaRay {

class ModelModel;
class ModelCellModel;
class ModelTester;
class RemoteModelServer;

class ModelInspector : public ModelInspectorInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ModelInspectorInterface)
  public:
    explicit ModelInspector(ProbeInterface *probe, QObject *parent = 0);

  private slots:
    void modelSelected(const QItemSelection &selected);
    void selectionChanged(const QItemSelection &selected);

    void objectSelected(QObject* object);

  private:
    ModelModel *m_modelModel;
    QItemSelectionModel *m_modelSelectionModel;

    RemoteModelServer *m_modelContentServer;
    QItemSelectionModel *m_modelContentSelectionModel;

    ModelCellModel *m_cellModel;

    ModelTester *m_modelTester;
};

class ModelInspectorFactory : public QObject, public StandardToolFactory<QAbstractItemModel, ModelInspector>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  public:
    explicit ModelInspectorFactory(QObject *parent) : QObject(parent)
    {
    }

    QString name() const;
};

}

#endif // GAMMARAY_MODELINSPECTOR_H
