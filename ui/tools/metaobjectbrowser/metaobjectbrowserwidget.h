/*
  metaobjectbrowserwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_METAOBJECTBROWSERWIDGET_H
#define GAMMARAY_METAOBJECTBROWSERWIDGET_H

#include <ui/uistatemanager.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
class PropertyWidget;
class DeferredTreeView;

class MetaObjectBrowserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MetaObjectBrowserWidget(QWidget *parent = nullptr);

private slots:
    void selectionChanged(const QItemSelection &selection);
    void propertyWidgetTabsChanged();

private:
    UIStateManager m_stateManager;
    PropertyWidget *m_propertyWidget;
    DeferredTreeView *m_treeView;
};
}

#endif // GAMMARAY_METAOBJECTBROWSERWIDGET_H
