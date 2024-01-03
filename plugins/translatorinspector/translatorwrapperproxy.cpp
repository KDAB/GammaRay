/*
  translatorwrapperproxy.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "translatorwrapperproxy.h"
#include "translatorwrapper.h" // use only for its role enums

#include <QFont>

using namespace GammaRay;

TranslatorWrapperProxy::TranslatorWrapperProxy(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

TranslatorWrapperProxy::~TranslatorWrapperProxy() = default;

QVariant TranslatorWrapperProxy::data(const QModelIndex &proxyIndex, int role) const
{
    if (hasIndex(proxyIndex.row(), proxyIndex.column(), proxyIndex.parent())) {
        if (role == Qt::FontRole) {
            const bool overridden = proxyIndex.sibling(proxyIndex.row(), 3)
                                        .data(TranslationsModel::IsOverriddenRole)
                                        .toBool();
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
