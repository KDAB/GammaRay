/*
 * widgetinspectorclient.h
 *
 * This file is part of GammaRay, the Qt application inspection and
 * manipulation tool.
 *
 * Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
 * Author: Milian Wolff <milian.wolff@kdab.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTORCLIENT_H
#define GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTORCLIENT_H

#include <widgetinspectorinterface.h>

namespace GammaRay {

class WidgetInspectorClient : public WidgetInspectorInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::WidgetInspectorInterface)

  public:
    explicit WidgetInspectorClient(QObject *parent);
    ~WidgetInspectorClient();

  private:
    virtual void saveAsImage(const QString &fileName);
    virtual void saveAsSvg(const QString &fileName);
    virtual void saveAsPdf(const QString &fileName);
    virtual void saveAsUiFile(const QString &fileName);
    virtual void analyzePainting();
    virtual void checkFeatures();
};
}

#endif // GAMMARAY_WIDGETINSPECTORCLIENT_H
