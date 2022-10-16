/*
  clienttoolmanager.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CLIENTTOOLMANAGER_H
#define GAMMARAY_CLIENTTOOLMANAGER_H

#include "gammaray_ui_export.h"

#include <QHash>
#include <QPointer>
#include <QVector>
#include <QObject>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QItemSelectionModel;
class QWidget;
QT_END_NAMESPACE

namespace GammaRay {

class ObjectId;
class ProbeControllerInterface;
struct ToolData;
class ToolManagerInterface;
class ToolUiFactory;

/*! Data structure describing a single GammaRay tool. */
class GAMMARAY_UI_EXPORT ToolInfo
{
public:
    ToolInfo() = default;
    ///@cond internal
    explicit ToolInfo(const ToolData &toolData, ToolUiFactory *factory);
    ///@endcond
    ~ToolInfo();

    QString id() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    bool hasUi() const;
    QString name() const;
    bool remotingSupported() const;
    bool isValid() const;

private:
    QString m_toolId;
    bool m_isEnabled = false;
    bool m_hasUi = false;
    ToolUiFactory *m_factory = nullptr;
};

/*! Tool Management API for the client to find out, which tools exist,
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
public:
    explicit ClientToolManager(QObject *parent = nullptr);
    ~ClientToolManager() override;

    void setToolParentWidget(QWidget *parent);

    bool isToolListLoaded() const;
    QWidget *widgetForId(const QString &toolId) const;
    QWidget *widgetForIndex(int index) const;
    int toolIndexForToolId(const QString &toolId) const;
    ToolInfo toolForToolId(const QString &toolId) const;

    inline QVector<ToolInfo> tools() const
    {
        return m_tools;
    }

    QAbstractItemModel *model();
    QItemSelectionModel *selectionModel();

    void requestToolsForObject(const ObjectId &id);
    void selectObject(const ObjectId &id, const ToolInfo &toolInfo);

    static ClientToolManager *instance();

public slots:
    void requestAvailableTools();
    void clear();

signals:
    void toolEnabled(const QString &toolId);
    void toolEnabledByIndex(int toolIndex);
    void aboutToReceiveData();
    void toolListAvailable();
    void toolSelected(const QString &toolId);
    void toolSelectedByIndex(int index);
    void toolsForObjectResponse(const GammaRay::ObjectId &id, const QVector<GammaRay::ToolInfo> &toolInfos);
    void aboutToReset();
    void reset();

private slots:
    void gotTools(const QVector<GammaRay::ToolData> &tools);
    void toolGotEnabled(const QString &toolId);
    void toolGotSelected(const QString &toolId);
    void toolsForObjectReceived(const GammaRay::ObjectId &id, const QVector<QString> &toolIds);

private:
    typedef QHash<QString, QPointer<QWidget>> WidgetsHash;
    mutable WidgetsHash m_widgets; // ToolId -> Widget
    QVector<ToolInfo> m_tools;
    QPointer<ToolManagerInterface> m_remote;
    QWidget *m_parentWidget;
    QAbstractItemModel *m_model;
    QItemSelectionModel *m_selectionModel;

    static ClientToolManager *s_instance;
};
}

#endif // GAMMARAY_CLIENTTOOLMANAGER_H
