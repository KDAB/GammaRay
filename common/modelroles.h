/*
  modelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_MODELROLES_H
#define GAMMARAY_MODELROLES_H

/**
 * @file modelroles.h
 * @brief A collection of custom model roles shared between client and server.
 */
namespace GammaRay {
/*! Portable replacement for Qt::UserRole.
 * Qt4 uses 32, Qt5 256, use the latter globally to allow combining Qt4/5 client/servers.
 */
static const int UserRole = 256;

/*! Custom roles for GammaRay::ToolModel.
 * @todo These can be split again, between core tool model and UI tool model.
 */
namespace ToolModelRole {
/*! Tool model data roles. */
enum Role
{
    ToolFactory = UserRole + 1,
    ToolWidget,
    ToolId,
    ToolWidgetParent,
    ToolEnabled,
    ToolHasUi,
    ToolFeedbackId
};
}
}

#endif
