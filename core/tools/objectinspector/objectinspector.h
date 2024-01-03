/*
  objectinspector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_OBJECTINSPECTOR_OBJECTINSPECTOR_H
#define GAMMARAY_OBJECTINSPECTOR_OBJECTINSPECTOR_H

#include "toolfactory.h"

#include <QObject>

QT_BEGIN_NAMESPACE
class QItemSelection;
class QItemSelectionModel;
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class PropertyController;

class ObjectInspector : public QObject
{
    Q_OBJECT
public:
    explicit ObjectInspector(Probe *probe, QObject *parent = nullptr);

private slots:
    void modelIndexSelected(const QModelIndex &index);
    void objectSelectionChanged(const QItemSelection &selection);
    void objectSelected(QObject *object);

private:
    static void registerPCExtensions();

    static void scanForConnectionIssues();
    static void scanForThreadAffinityIssues();

    PropertyController *m_propertyController;
    QItemSelectionModel *m_selectionModel;
};

class ObjectInspectorFactory : public QObject, public StandardToolFactory<QObject, ObjectInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
public:
    explicit ObjectInspectorFactory(QObject *parent)
        : QObject(parent)
    {
    }

    QVector<QByteArray> selectableTypes() const override;
};
}

#endif // GAMMARAY_OBJECTINSPECTOR_H
