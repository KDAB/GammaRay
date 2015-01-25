/*
  widgetinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTORINTERFACE_H
#define GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTORINTERFACE_H

#include <QObject>

class QPixmap;

namespace GammaRay {

class WidgetInspectorInterface : public QObject
{
  Q_OBJECT
  public:
    explicit WidgetInspectorInterface(QObject *parent = 0);
    virtual ~WidgetInspectorInterface();

  public slots:
    virtual void checkFeatures() = 0;
    virtual void saveAsImage(const QString &fileName) = 0;
    virtual void saveAsSvg(const QString &fileName) = 0;
    virtual void saveAsPdf(const QString &fileName) = 0;
    virtual void saveAsUiFile(const QString &fileName) = 0;

    virtual void analyzePainting() = 0;

  signals:
    void widgetPreviewAvailable(const QPixmap &pixmap);
    void paintAnalyzed(const QPixmap &pixmap);
    void features(bool svg, bool print, bool designer, bool privateHeaders);
};

}

Q_DECLARE_INTERFACE(GammaRay::WidgetInspectorInterface, "com.kdab.GammaRay.WidgetInspector")

#endif // GAMMARAY_WIDGETINSPECTORINTERFACE_H
