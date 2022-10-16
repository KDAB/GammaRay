/*
  uiintegration.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef UIINTEGRATION_H
#define UIINTEGRATION_H

#include "gammaray_ui_export.h"

#include <QObject>

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

namespace GammaRay {
/*! Interface for UI plugins to integrate with the client UI.
 *  This is especially relevant when the client is embedded inside another application
 *  such as an IDE.
 */
class GAMMARAY_UI_EXPORT UiIntegration : public QObject
{
    Q_OBJECT
public:
    explicit UiIntegration(QObject *parent = nullptr);
    ~UiIntegration() override;

    static UiIntegration *instance();

    /*! Show the source code at the given position.
     *  The stand-alone client will launch a text editor for this,
     *  if the client is embedded in an IDE it can chose to navigate directly.
     */
    static void requestNavigateToCode(const QUrl &url, int lineNumber, int columnNumber = 0);

    static bool hasDarkUI();

Q_SIGNALS:
    void navigateToCode(const QUrl &url, int lineNumber, int columnNumber);

private:
    /*! Singleton instance. */
    static UiIntegration *s_uiIntegrationInstance;
};
} // namespace GammaRay

#endif // UIINTEGRATION_H
