/*
  webinspector.h

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krauss@kdab.com>

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

#ifndef ENDOSCOPE_WEBINSPECTOR_H
#define ENDOSCOPE_WEBINSPECTOR_H

#include <toolfactory.h>
#include <qwidget.h>
#include <qwebpage.h>

namespace Endoscope {

namespace Ui { class WebInspector; }

class WebInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit WebInspector(Endoscope::ProbeInterface* probe, QWidget* parent = 0);

  private slots:
    void webPageSelected(int index);

  private:
    QScopedPointer<Ui::WebInspector> ui;
};

class WebInspectorFactory : public QObject, public StandardToolFactory<QWebPage, WebInspector>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolFactory)
  public:
    inline QString name() const { return tr("Web Pages"); }
};

}

#endif // ENDOSCOPE_WEBINSPECTOR_H
