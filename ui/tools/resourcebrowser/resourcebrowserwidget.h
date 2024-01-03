/*
  resourcebrowserwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_RESOURCEBROWSER_RESOURCEBROWSERWIDGET_H
#define GAMMARAY_RESOURCEBROWSER_RESOURCEBROWSERWIDGET_H

#include <ui/uistatemanager.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
class ResourceBrowserInterface;

namespace Ui {
class ResourceBrowserWidget;
}

class ResourceBrowserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ResourceBrowserWidget(QWidget *parent = nullptr);
    ~ResourceBrowserWidget() override;

public slots:
    void selectResource(const QString &sourceFilePath, int line = -1, int column = -1);

private slots:
    void setupLayout();
    void resourceDeselected();
    void resourceSelected(const QByteArray &contents, int line, int column);
    static void resourceDownloaded(const QString &targetFilePath, const QByteArray &contents);

    void handleCustomContextMenu(const QPoint &pos);

private:
    QScopedPointer<Ui::ResourceBrowserWidget> ui;
    UIStateManager m_stateManager;
    ResourceBrowserInterface *m_interface;
};
}

#endif // GAMMARAY_RESOURCEBROWSER_H
