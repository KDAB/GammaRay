/*
  webviewmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "webviewmodel.h"
#include "webviewmodelroles.h"

#include "common/objectmodel.h"

using namespace GammaRay;

WebViewModel::WebViewModel(QObject *parent)
    : ObjectFilterProxyModelBase(parent)
{
}

WebViewModel::~WebViewModel() = default;

QVariant WebViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ((role != Qt::DisplayRole && role != WebViewModelRoles::WebKitVersionRole)
        || index.column() != 0)
        return QSortFilterProxyModel::data(index, role);

    const QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject *>();
    const bool isWk2 = obj->inherits("QQuickWebView");

    if (role == Qt::DisplayRole)
        return QString(Util::displayString(obj) + (isWk2 ? QStringLiteral(" [WebKit2]") : QStringLiteral(" [WebEngine]")));
    if (role == WebViewModelRoles::WebKitVersionRole)
        return isWk2 ? 2 : 3;

    Q_ASSERT(!"WTF?");
    return QVariant();
}

QMap<int, QVariant> WebViewModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> d;
    d.insert(Qt::DisplayRole, data(index, Qt::DisplayRole));
    d.insert(WebViewModelRoles::WebKitVersionRole,
             data(index, WebViewModelRoles::WebKitVersionRole));
    return d;
}

bool WebViewModel::filterAcceptsObject(QObject *object) const
{
    return object->inherits("QQuickWebView") || object->inherits("QQuickWebEngineView") || object->inherits("QWebEnginePage");
}
