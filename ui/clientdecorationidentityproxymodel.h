/*
  clientdecorationidentityproxymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  Copyright (C) 2010-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#ifndef GAMMARAY_CLIENTDECORATIONIDENTITYPROXYMODEL_H
#define GAMMARAY_CLIENTDECORATIONIDENTITYPROXYMODEL_H

#include "gammaray_ui_export.h"

#include <QIdentityProxyModel>
#include <QHash>
#include <QIcon>
#include <QPointer>

namespace GammaRay {
class ClassesIconsRepository;

/*! Proxy model for client-side resolution of class icons. */
class GAMMARAY_UI_EXPORT ClientDecorationIdentityProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    explicit ClientDecorationIdentityProxyModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QPointer<ClassesIconsRepository> m_classesIconsRepository;
    mutable QHash<int, QIcon> m_icons;
};
}

#endif // GAMMARAY_CLIENTDECORATIONIDENTITYPROXYMODEL_H
