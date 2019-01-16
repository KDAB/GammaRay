/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef GAMMARAY_ACTIONINSPECTOR_ACTIONINSPECTOR_H
#define GAMMARAY_ACTIONINSPECTOR_ACTIONINSPECTOR_H

#include <core/toolfactory.h>

#include <QAction>

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
QT_END_NAMESPACE

namespace GammaRay {
class ActionInspector : public QObject
{
    Q_OBJECT

public:
    explicit ActionInspector(Probe *probe, QObject *parent = nullptr);
    ~ActionInspector() override;

public Q_SLOTS:
    void triggerAction(int row);

private Q_SLOTS:
    void objectSelected(QObject *obj);

private:
    void registerMetaTypes();
    QItemSelectionModel *m_selectionModel;
};

class ActionInspectorFactory : public QObject, public StandardToolFactory<QAction, ActionInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_actioninspector.json")

public:
    explicit ActionInspectorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_ACTIONINSPECTOR_H
