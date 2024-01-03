/*
  widgetclientmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_WIDGETINSPECTOR_WIDGETCLIENTMODEL_H
#define GAMMARAY_WIDGETINSPECTOR_WIDGETCLIENTMODEL_H

#include <ui/clientdecorationidentityproxymodel.h>

namespace GammaRay {
/** UI-dependent (and thus client-side) bits of the widget tree model. */
class WidgetClientModel : public ClientDecorationIdentityProxyModel
{
    Q_OBJECT
public:
    explicit WidgetClientModel(QObject *parent = nullptr);
    ~WidgetClientModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};
}

#endif // GAMMARAY_WIDGETINSPECTOR_WIDGETCLIENTMODEL_H
