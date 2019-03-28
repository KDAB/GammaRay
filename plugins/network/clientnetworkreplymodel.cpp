/*
  clientnetworkreplymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "clientnetworkreplymodel.h"
#include "networkreplymodeldefs.h"

#include <ui/uiresources.h>

#include <QApplication>
#include <QIcon>
#include <QNetworkAccessManager>
#include <QPalette>
#include <QStyle>

using namespace GammaRay;

ClientNetworkReplyModel::ClientNetworkReplyModel(QObject* parent)
    : QIdentityProxyModel(parent)
{
}

ClientNetworkReplyModel::~ClientNetworkReplyModel() = default;

QVariant ClientNetworkReplyModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole && index.column() == NetworkReplyModelColumn::OpColumn) {
        const auto op = QIdentityProxyModel::data(index, role).toInt();
        switch (op) {
            case QNetworkAccessManager::HeadOperation: return QStringLiteral("HEAD");
            case QNetworkAccessManager::GetOperation: return QStringLiteral("GET");
            case QNetworkAccessManager::PutOperation: return QStringLiteral("PUT");
            case QNetworkAccessManager::PostOperation: return QStringLiteral("POST");
            case QNetworkAccessManager::DeleteOperation: return QStringLiteral("DELETE");
            case QNetworkAccessManager::CustomOperation: return tr("Custom");
        }
    }

    if (role == Qt::ForegroundRole) {
        const auto state = QIdentityProxyModel::data(index.sibling(index.row(), NetworkReplyModelColumn::ObjectColumn), NetworkReplyModelRole::ReplyStateRole).toInt();
        if (state & NetworkReply::Deleted) {
            return QGuiApplication::palette().color(QPalette::Disabled, QPalette::Text);
        }
        if (state & NetworkReply::Error) {
            return QColor(Qt::red);
        }
    }

    if (role == Qt::DecorationRole && index.parent().isValid()) {
        const auto state = QIdentityProxyModel::data(index.sibling(index.row(), NetworkReplyModelColumn::ObjectColumn), NetworkReplyModelRole::ReplyStateRole).toInt();
        switch (index.column()) {
            case NetworkReplyModelColumn::ObjectColumn:
                if (state & NetworkReply::Error) {
                    return QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton);
                } else if (state & NetworkReply::Finished) {
                    return QApplication::style()->standardIcon(QStyle::SP_DialogOkButton);
                }
                return QApplication::style()->standardIcon(QStyle::SP_BrowserReload);
            case NetworkReplyModelColumn::UrlColumn:
            {
                const auto url = QIdentityProxyModel::data(index, Qt::DisplayRole).toString();
                if ((state & NetworkReply::Encrypted) || ((state & NetworkReply::Unencrypted) == 0 && url.startsWith(QLatin1String("https")))) {
                    return UIResources::themedIcon(QLatin1String("lock.png"));
                }
                return UIResources::themedIcon(QLatin1String("lock-open.png"));
            }
        }
    }

    if (role == Qt::ToolTipRole) {
        if (index.column() == NetworkReplyModelColumn::UrlColumn) {
            return index.data(Qt::DisplayRole);
        }

        const auto errMsgs = QIdentityProxyModel::data(index.sibling(index.row(), NetworkReplyModelColumn::ObjectColumn), NetworkReplyModelRole::ReplyErrorRole).toStringList();
        if (errMsgs.isEmpty()) {
            return {};
        }
        if (errMsgs.size() == 1) {
            return errMsgs.at(0);
        }
        QString tt;
        for (const auto &msg : errMsgs) {
            tt += QLatin1String("<li>") + msg + QLatin1String("</li>");
        }
        return tt;
    }

    return QIdentityProxyModel::data(index, role);
}

QVariant ClientNetworkReplyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case NetworkReplyModelColumn::ObjectColumn: return tr("Reply");
            case NetworkReplyModelColumn::OpColumn: return tr("Operation");
            case NetworkReplyModelColumn::TimeColumn: return tr("Duration");
            case NetworkReplyModelColumn::SizeColumn: return tr("Size");
            case NetworkReplyModelColumn::UrlColumn: return tr("URL");
        }
    }

    return QIdentityProxyModel::headerData(section, orientation, role);
}
