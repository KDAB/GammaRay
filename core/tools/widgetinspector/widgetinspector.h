/*
  widgetinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTOR_H
#define GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTOR_H

#include "include/toolfactory.h"

#include <QLibrary>
#include <QWidget>

class QModelIndex;

namespace GammaRay {

class OverlayWidget;

namespace Ui {
  class WidgetInspector;
}

class WidgetInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit WidgetInspector(ProbeInterface *probe, QWidget *parent = 0);

  private:
    void setActionsEnabled(bool enabled);
    QWidget *selectedWidget() const;
    void callExternalExportAction(const char *name, QWidget *widget, const QString &fileName);

  private slots:
    void widgetSelected(const QModelIndex &index);
    void widgetSelected(QWidget *widget);

    void saveAsImage();
    void saveAsSvg();
    void saveAsPdf();
    void saveAsUiFile();

    void analyzePainting();

  private:
    OverlayWidget *m_overlayWidget;
    QScopedPointer<Ui::WidgetInspector> ui;
    QLibrary m_externalExportActions;
};

class WidgetInspectorFactory
  : public QObject, public StandardToolFactory<QWidget, WidgetInspector>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  public:
    explicit WidgetInspectorFactory(QObject *parent) : QObject(parent)
    {
    }

    inline QString name() const
    {
      return tr("Widgets");
    }
};

}

#endif // GAMMARAY_WIDGETINSPECTOR_H
