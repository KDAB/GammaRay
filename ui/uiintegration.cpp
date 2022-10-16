/*
  uiintegration.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "uiintegration.h"

#include <QUrl>
#include <QApplication>
#include <QPalette>

using namespace GammaRay;

UiIntegration *UiIntegration::s_uiIntegrationInstance = nullptr;

UiIntegration::UiIntegration(QObject *parent)
    : QObject(parent)
{
    Q_ASSERT(!s_uiIntegrationInstance);
    s_uiIntegrationInstance = this;
}

UiIntegration::~UiIntegration()
{
    s_uiIntegrationInstance = nullptr;
}

UiIntegration *UiIntegration::instance()
{
    return s_uiIntegrationInstance;
}

void UiIntegration::requestNavigateToCode(const QUrl &url, int lineNumber, int columnNumber)
{
    if (UiIntegration::instance())
        emit UiIntegration::instance()->navigateToCode(url, lineNumber, columnNumber);
}

bool UiIntegration::hasDarkUI()
{
    return QApplication::palette().color(QPalette::Base).lightness() < 128;
}
