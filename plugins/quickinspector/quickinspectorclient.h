/*
  quickinspectorclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKINSPECTORCLIENT_H
#define GAMMARAY_QUICKINSPECTOR_QUICKINSPECTORCLIENT_H

#include "quickinspectorinterface.h"

QT_BEGIN_NAMESPACE
class QEvent;
QT_END_NAMESPACE

namespace GammaRay {
struct QuickDecorationsSettings;

class QuickInspectorClient : public QuickInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::QuickInspectorInterface)

public:
    explicit QuickInspectorClient(QObject *parent = nullptr);
    ~QuickInspectorClient() override;

public slots:
    void selectWindow(int index) override;

    void setCustomRenderMode(GammaRay::QuickInspectorInterface::RenderMode customRenderMode)
        override;

    void checkFeatures() override;

    void setOverlaySettings(const GammaRay::QuickDecorationsSettings &settings) override;

    void checkOverlaySettings() override;

    void analyzePainting() override;

    void checkSlowMode() override;
    void setSlowMode(bool slow) override;
};
}

#endif // GAMMARAY_QUICKINSPECTORCLIENT_H
