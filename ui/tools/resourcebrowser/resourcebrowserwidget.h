/*
  resourcebrowserwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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
    void resourceDownloaded(const QString &targetFilePath, const QByteArray &contents);

    void handleCustomContextMenu(const QPoint &pos);

private:
    QScopedPointer<Ui::ResourceBrowserWidget> ui;
    UIStateManager m_stateManager;
    ResourceBrowserInterface *m_interface;
};
}

#endif // GAMMARAY_RESOURCEBROWSER_H
