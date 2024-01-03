/*
  widgetinspectorclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    ~WidgetInspectorClient() override;

private:
    void saveAsImage(const QString &fileName) override;
    void saveAsSvg(const QString &fileName) override;
    void saveAsUiFile(const QString &fileName) override;
    void analyzePainting() override;
};
}

#endif // GAMMARAY_WIDGETINSPECTORCLIENT_H
