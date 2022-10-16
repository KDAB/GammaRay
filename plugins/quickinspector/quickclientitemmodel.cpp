/*
  quickclientitemmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "quickclientitemmodel.h"
#include "quickitemmodelroles.h"

#include <QApplication>
#include <QPalette>
#include <QIcon>
#include <QBuffer>

using namespace GammaRay;

QuickClientItemModel::QuickClientItemModel(QObject *parent)
    : ClientDecorationIdentityProxyModel(parent)
{
}

QuickClientItemModel::~QuickClientItemModel() = default;

QVariant QuickClientItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::ForegroundRole || role == Qt::ToolTipRole) {
        int flags = ClientDecorationIdentityProxyModel::data(index, QuickItemModelRole::ItemFlags).value<int>();

        // Grey out invisible items
        if (role == Qt::ForegroundRole
            && (flags & (QuickItemModelRole::Invisible | QuickItemModelRole::ZeroSize)))
            return qApp->palette().color(QPalette::Disabled, QPalette::Text);
        // Adjust tooltip to show information about items
        if (role == Qt::ToolTipRole && flags) {
            QString tooltip = ClientDecorationIdentityProxyModel::data(index, role).toString();
            tooltip.append("<p style='white-space:pre'>");
            // if flags has OutOfView it has also PartiallyOutOfView, no need to test both
            if ((flags & QuickItemModelRole::PartiallyOutOfView)
                && (~flags & QuickItemModelRole::Invisible)) {
                QByteArray byteArray;
                QBuffer buffer(&byteArray);
                QIcon::fromTheme(QStringLiteral("dialog-warning")).pixmap(16, 16).save(&buffer, "PNG");
                tooltip.append("<img src=\"data:image/png;base64,").append(byteArray.toBase64());
                if (flags & QuickItemModelRole::OutOfView)
                    tooltip.append("\"> Item is visible, but out of view.");
                else
                    tooltip.append("\"> Item is visible, but partially out of view.");

                flags &= ~QuickItemModelRole::OutOfView;
                flags &= ~QuickItemModelRole::PartiallyOutOfView;
                if (flags)
                    tooltip.append("\n");
            }
            if (flags) {
                QStringList flagStrings;
                if (flags & QuickItemModelRole::Invisible)
                    flagStrings << tr("is invisible");

                if (flags & QuickItemModelRole::ZeroSize)
                    flagStrings << tr("has a size of zero");

                if (flags & QuickItemModelRole::OutOfView)
                    flagStrings << tr("is out of view");
                else if (flags & QuickItemModelRole::PartiallyOutOfView)
                    flagStrings << tr("is partially out of view");

                if (flags & QuickItemModelRole::HasFocus
                    && ~flags & QuickItemModelRole::HasActiveFocus)
                    flagStrings << tr("has inactive focus");

                if (flags & QuickItemModelRole::HasActiveFocus)
                    flagStrings << tr("has active focus");

                if (flags & QuickItemModelRole::JustRecievedEvent)
                    flagStrings << tr("just received an event");

                QByteArray byteArray;
                QBuffer buffer(&byteArray);
                QIcon::fromTheme(QStringLiteral("dialog-information")).pixmap(16, 16).save(&buffer, "PNG");
                tooltip.append(QStringLiteral("<img src=\"data:image/png;base64,").append(byteArray.toBase64()).append("\"> Item %1.").arg(flagStrings.join(QStringLiteral(", "))));
            }
            tooltip.append("</p>");
            return tooltip;
        }
    }
    return ClientDecorationIdentityProxyModel::data(index, role);
}
