/*
  styleinspectorclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_STYLEINSPECTOR_STYLEINSPECTORCLIENT_H
#define GAMMARAY_STYLEINSPECTOR_STYLEINSPECTORCLIENT_H

#include "styleinspectorinterface.h"

namespace GammaRay {
class StyleInspectorClient : public StyleInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::StyleInspectorInterface)
public:
    explicit StyleInspectorClient(QObject *parent = nullptr);
    ~StyleInspectorClient() override;

    void init();

    void setCellHeight(int height) override;
    void setCellWidth(int width) override;
    void setCellZoom(int zoom) override;
};
}

#endif // GAMMARAY_STYLEINSPECTORCLIENT_H
