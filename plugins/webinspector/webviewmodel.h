/*
  webviewmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_WEBINSPECTOR_WEBVIEWMODEL_H
#define GAMMARAY_WEBINSPECTOR_WEBVIEWMODEL_H

#include <core/objecttypefilterproxymodel.h>

namespace GammaRay {
class WebViewModel : public ObjectFilterProxyModelBase
{
    Q_OBJECT
public:
    explicit WebViewModel(QObject *parent = nullptr);
    ~WebViewModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    bool filterAcceptsObject(QObject *object) const override;
};
}

#endif // GAMMARAY_WEBVIEWMODEL_H
