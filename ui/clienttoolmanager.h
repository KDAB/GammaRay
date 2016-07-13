/*
  clienttoolmanager.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_CLIENTTOOLMANAGER_H
#define GAMMARAY_CLIENTTOOLMANAGER_H

#include "gammaray_ui_export.h"

#include <common/toolmanagerinterface.h>

#include <QHash>
#include <QPointer>
#include <QAbstractListModel>
#include <QItemSelectionModel>
#include <QSet>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace GammaRay {
class ProbeControllerInterface;
class ToolUiFactory;

/** @brief Tool Management API for the client to find out, which tools exist,
 *  get the tool's widgets, etc.
 *
 *  This is needed when implementing your own client UI embedded into a different application.
 *  If you want to provide a list with all supported tools, you can use
 *  \e ClientToolManager::model() to get all the tool information as a model.
 *
 *  This class also takes care of loading client-sided tool plugins.
 */
class GAMMARAY_UI_EXPORT ClientToolManager : public QObject
{
    Q_OBJECT
    class Model;
    class SelectionModel;

public:
    explicit ClientToolManager(QObject *parent = 0);
    ~ClientToolManager();

    void setToolParentWidget(QWidget *parent);

    bool isToolListLoaded() const;
    QWidget *widgetForId(const QString &toolId) const;
    QWidget *widgetForIndex(int index) const;
    int toolIndexForToolId(const QString &toolId) const;

    inline ToolInfos tools() const
    {
        return m_tools;
    }

    QAbstractItemModel *model();
    QItemSelectionModel *selectionModel();

signals:
    void toolEnabled(const QString &toolId);
    void toolEnabledByIndex(int toolIndex);
    void aboutToReceiveData();
    void toolListAvailable();
    void toolSelected(const QString &toolId);
    void toolSelectedByIndex(int index);

private slots:
    void gotTools(const GammaRay::ToolInfos &tools);
    void toolGotEnabled(const QString &toolId);
    void toolGotSelected(const QString &toolId);

private:
    typedef QHash<QString, QPointer<QWidget> > WidgetsHash;
    mutable WidgetsHash m_widgets; // ToolId -> Widget
    ToolInfos m_tools;
    ToolManagerInterface *m_remote;
    QWidget *m_parentWidget;
    Model *m_model;
    SelectionModel *m_selectionModel;
};
}

#endif // GAMMARAY_CLIENTTOOLMANAGER_H
