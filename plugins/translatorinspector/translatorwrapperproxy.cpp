/*
  translatorwrapperproxy.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "translatorwrapperproxy.h"
#include "translatorwrapper.h" // use only for its role enums

#include <QFont>

using namespace GammaRay;

TranslatorWrapperProxy::TranslatorWrapperProxy(QObject* parent)
    : QIdentityProxyModel(parent)
{
}

TranslatorWrapperProxy::~TranslatorWrapperProxy() = default;

QVariant TranslatorWrapperProxy::data(const QModelIndex& proxyIndex, int role) const
{
    if (hasIndex(proxyIndex.row(), proxyIndex.column(), proxyIndex.parent())) {
        if (role == Qt::FontRole) {
            const bool overridden = proxyIndex.sibling(proxyIndex.row(), 3)
                    .data(TranslationsModel::IsOverriddenRole).toBool();
            QFont font;
            font.setItalic(overridden);
            return font;
        }
    }

    return QIdentityProxyModel::data(proxyIndex, role);
}

QVariant TranslatorWrapperProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Context");
        case 1:
            return tr("Source Text");
        case 2:
            return tr("Disambiguation");
        case 3:
            return tr("Translation");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}
