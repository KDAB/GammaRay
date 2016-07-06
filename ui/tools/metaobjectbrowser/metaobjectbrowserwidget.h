/*
  metaobjectbrowserwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    explicit MetaObjectBrowserWidget(QWidget *parent = 0);

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
