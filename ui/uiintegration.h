/*
 * This file is part of GammaRay, the Qt application inspection and
 * manipulation tool.
 *
 * Copyright (C) 2014-2020 Klar?lvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
 * Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>
 *
 * Licensees holding valid commercial KDAB GammaRay licenses may use this file in
 * accordance with GammaRay Commercial License Agreement provided with the Software.
 *
 * Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
