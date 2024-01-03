/*
  fontbrowserwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_FONTBROWSER_FONTBROWSERWIDGET_H
#define GAMMARAY_FONTBROWSER_FONTBROWSERWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
class FontBrowserInterface;

namespace Ui {
class FontBrowserWidget;
}

class FontBrowserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FontBrowserWidget(QWidget *parent = nullptr);
    ~FontBrowserWidget() override;

private slots:
    void delayedInit();

private:
    QScopedPointer<Ui::FontBrowserWidget> ui;
    UIStateManager m_stateManager;
    QAbstractItemModel *m_selectedFontModel;
    FontBrowserInterface *m_fontBrowser;
};

class FontBrowserUiFactory : public QObject, public StandardToolUiFactory<FontBrowserWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_fontbrowser.json")
};
}

#endif // GAMMARAY_FONTBROWSERWIDGET_H
