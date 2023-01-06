/*
  contextmenuextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CONTEXTMENUEXTENSION_H
#define GAMMARAY_CONTEXTMENUEXTENSION_H

#include "gammaray_ui_export.h"

#include <common/toolmanagerinterface.h>
#include <common/sourcelocation.h>

#include <QObject>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QMenu;
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class GAMMARAY_UI_EXPORT ContextMenuExtension
{
public:
    // UI presentation depend the order of this enum
    enum Location
    {
        GoTo,
        ShowSource,
        Creation,
        Declaration
    };

    explicit ContextMenuExtension(const ObjectId &id = ObjectId());

    void setLocation(Location location, const SourceLocation &sourceLocation);

    bool discoverSourceLocation(Location location, const QUrl &url);
    // Given a model index from a PropertyModel, try to found a valid url and call
    // setLocation() with the given location.
    bool discoverPropertySourceLocation(Location location, const QModelIndex &index);

    /// Populate @p menu with entries related to the captured object id. Only supported on Qt5
    void populateMenu(QMenu *menu);

    void setCanFavoriteItems(bool);

private:
    ObjectId m_id;
    QVector<QPair<Location, SourceLocation>> m_locations;
    bool canFavorite = false;
};
}

#endif // GAMMARAY_CONTEXTMENUEXTENSION_H
