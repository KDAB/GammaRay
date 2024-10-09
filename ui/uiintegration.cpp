/*
  uiintegration.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
