/*
  widgetclientmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "widgetclientmodel.h"
#include "widgetmodelroles.h"

#include <QApplication>
#include <QPalette>

using namespace GammaRay;

WidgetClientModel::WidgetClientModel(QObject *parent)
    : ClientDecorationIdentityProxyModel(parent)
{
}

WidgetClientModel::~WidgetClientModel() = default;

QVariant WidgetClientModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == Qt::ForegroundRole) {

        int flags = ClientDecorationIdentityProxyModel::data(index, WidgetModelRoles::WidgetFlags).value<int>();

        if (flags & WidgetModelRoles::Invisible)
            return qApp->palette().color(QPalette::Disabled, QPalette::Text);
    }
    return ClientDecorationIdentityProxyModel::data(index, role);
}
