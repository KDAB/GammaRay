/*
  abstractstyleelementmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "abstractstyleelementmodel.h"
#include "dynamicproxystyle.h"

#include <QApplication>
#include <QProxyStyle>
#include <QStyle>

using namespace GammaRay;

AbstractStyleElementModel::AbstractStyleElementModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void AbstractStyleElementModel::setStyle(QStyle *style)
{
    beginResetModel();
    m_style = QPointer<QStyle>(style);
    endResetModel();
}

QVariant AbstractStyleElementModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_style)
        return QVariant();
    return doData(index.row(), index.column(), role);
}

int AbstractStyleElementModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return doColumnCount();
}

int AbstractStyleElementModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_style)
        return 0;
    return doRowCount();
}

bool AbstractStyleElementModel::isMainStyle() const
{
    QStyle *style = qApp->style();
    forever {
        if (style == m_style)
            return true;
        QProxyStyle *proxy = qobject_cast<QProxyStyle *>(style);
        if (!proxy)
            return false;
        style = proxy->baseStyle();
    }
}

QStyle *AbstractStyleElementModel::effectiveStyle() const
{
    return (isMainStyle() && DynamicProxyStyle::exists()) ? DynamicProxyStyle::instance() : m_style;
}
