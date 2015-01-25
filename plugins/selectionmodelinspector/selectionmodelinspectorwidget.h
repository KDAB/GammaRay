/*
  selectionmodelinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#ifndef GAMMARAY_SELECTIONMODELINSPECTOR_SELECTIONMODELINSPECTORWIDGET_H
#define GAMMARAY_SELECTIONMODELINSPECTOR_SELECTIONMODELINSPECTORWIDGET_H

#include <core/toolfactory.h>

#include <QWidget>

class QItemSelection;

namespace GammaRay {

namespace Ui {
  class SelectionModelInspectorWidget;
}

class SelectionModelInspectorWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit SelectionModelInspectorWidget(QWidget *widget = 0);
    ~SelectionModelInspectorWidget();

  private:
    QScopedPointer<Ui::SelectionModelInspectorWidget> ui;
};

class SelectionModelInspectorUiFactory : public QObject, public StandardToolUiFactory<SelectionModelInspectorWidget>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolUiFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.SelectionModelInspectorUi")
};


}

#endif // GAMMARAY_SELECTIONMODELINSPECTOR_H
