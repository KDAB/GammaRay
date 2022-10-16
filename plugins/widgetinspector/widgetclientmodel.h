/*
  widgetclientmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
