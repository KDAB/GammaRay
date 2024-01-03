/*
  modelinspector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_MODELINSPECTOR_MODELINSPECTOR_H
#define GAMMARAY_MODELINSPECTOR_MODELINSPECTOR_H

#include "modelinspectorinterface.h"

#include <core/toolfactory.h>

#include <QAbstractItemModel>


QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QItemSelection;
class QItemSelectionModel;
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class ModelCellModel;
class ModelContentProxyModel;
class SelectionModelModel;

class ModelInspector : public ModelInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ModelInspectorInterface)
public:
    explicit ModelInspector(Probe *probe, QObject *parent = nullptr);

private slots:
    void modelSelected(const QItemSelection &selected);
    void cellSelectionChanged(const QItemSelection &selection);
    void selectionModelSelected(const QItemSelection &selected);

    void objectSelected(QObject *object);
    void objectCreated(QObject *object);

private:
    Probe *m_probe;
    QAbstractItemModel *m_modelModel;
    QItemSelectionModel *m_modelSelectionModel;

    SelectionModelModel *m_selectionModelsModel;
    QItemSelectionModel *m_selectionModelsSelectionModel;

    QItemSelectionModel *m_modelContentSelectionModel;
    ModelContentProxyModel *m_modelContentProxyModel;

    ModelCellModel *m_cellModel;
};

class ModelInspectorFactory : public QObject,
                              public StandardToolFactory<QAbstractItemModel, ModelInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_modelinspector.json")
public:
    explicit ModelInspectorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_MODELINSPECTOR_H
