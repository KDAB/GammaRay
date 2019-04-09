/*
  objectinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    void registerPCExtensions();

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
