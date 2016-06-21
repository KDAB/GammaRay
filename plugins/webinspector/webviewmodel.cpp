/*
  webviewmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>
#include "webviewmodel.h"
#include "webviewmodelroles.h"

#include "common/objectmodel.h"

#ifdef HAVE_QT_WEBKIT1
#include <QWebPage>
#endif

using namespace GammaRay;

WebViewModel::WebViewModel(QObject *parent)
    : ObjectFilterProxyModelBase(parent)
{
}

WebViewModel::~WebViewModel()
{
}

QVariant WebViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ((role != Qt::DisplayRole && role != WebViewModelRoles::WebKitVersionRole)
        || index.column() != 0)
        return QSortFilterProxyModel::data(index, role);

    const QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject *>();
#ifdef HAVE_QT_WEBKIT1
    const bool isWk1 = qobject_cast<const QWebPage *>(obj);
#else
    const bool isWk1 = false;
#endif

    if (role == Qt::DisplayRole)
        return QString(Util::displayString(obj) + (isWk1 ? " [WebKit1]" : " [WebKit2]"));
    if (role == WebViewModelRoles::WebKitVersionRole)
        return isWk1 ? 1 : 2;

    Q_ASSERT(!"WTF?");
    return QVariant();
}

QMap< int, QVariant > WebViewModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> d = ObjectFilterProxyModelBase::itemData(index);
    d.insert(WebViewModelRoles::WebKitVersionRole,
             data(index, WebViewModelRoles::WebKitVersionRole));
    return d;
}

bool WebViewModel::filterAcceptsObject(QObject *object) const
{
    return
#ifdef HAVE_QT_WEBKIT1
        qobject_cast<QWebPage *>(object) ||
#endif
        object->inherits("QQuickWebView");
}
