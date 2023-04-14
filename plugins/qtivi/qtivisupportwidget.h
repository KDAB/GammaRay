/*
  qtivisupportwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QTIVISUPPORTWIDGET_H
#define GAMMARAY_QTIVISUPPORTWIDGET_H

#include <QWidget>

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QTreeView;
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
class DeferredTreeView;

class QtIVIWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QtIVIWidget(QWidget *parent = nullptr);

private slots:
    void objectSelected(const QItemSelection &selection);
    void contextMenu(QPoint pos);

private:
    DeferredTreeView *m_objectTreeView;
    UIStateManager m_stateManager;
};

class QtIVIUiFactory : public QObject, public StandardToolUiFactory<QtIVIWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_qtivi.json")
};

}

#endif
