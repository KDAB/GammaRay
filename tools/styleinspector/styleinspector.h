/*
  styleinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_STYLEINSPECTOR_H
#define GAMMARAY_STYLEINSPECTOR_H

#include <toolfactory.h>

#include <qstyle.h>
#include <qwidget.h>

namespace GammaRay {

class PixelMetricModel;
class StandardIconModel;

namespace Ui {
class StyleInspector;
}

class StyleInspector : public QWidget
{
  Q_OBJECT
public:
  explicit StyleInspector(ProbeInterface *probe, QWidget *parent = 0);
  virtual ~StyleInspector();

private slots:
  void styleSelected(int index);

private:
  Ui::StyleInspector *ui;
  PixelMetricModel *m_pixelMetricModel;
  StandardIconModel *m_standardIconModel;
};

class StyleInspectorFactory : public QObject, public StandardToolFactory<QStyle, StyleInspector>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)

public:
  explicit StyleInspectorFactory(QObject* parent = 0) : QObject(parent) {}
  virtual QString name() const { return tr("Style"); }
};

}

#endif // GAMMARAY_STYLEINSPECTOR_H
