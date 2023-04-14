/*
  styleelementproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "styleelementproxymodel.h"

using namespace GammaRay;

StyleElementProxyModel::StyleModelProxy(QObject *parent)
    : QIdentityProxyModel(parent)
    , m_sizeHint(
          64, 64)
{
}

void StyleElementProxyModel::setWidth(int width)
{
    m_sizeHint.setWidth(width);
}

void StyleElementProxyModel::setHeight(int height)
{
    m_sizeHint.setHeight(height);
}

QVariant StyleElementProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (role == Qt::SizeHintRole)
        return m_sizeHint;

    return QAbstractProxyModel::data(proxyIndex, role);
}
