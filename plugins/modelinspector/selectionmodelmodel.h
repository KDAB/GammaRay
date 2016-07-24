/*
  selectionmodelmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SELECTIONMODELMODEL_H
#define GAMMARAY_SELECTIONMODELMODEL_H

#include <core/objecttypefilterproxymodel.h>

#include <QItemSelectionModel>

namespace GammaRay {

/*! Selection models for a specific model. */
class SelectionModelModel : public ObjectTypeFilterProxyModel<QItemSelectionModel>
{
    Q_OBJECT
public:
    explicit SelectionModelModel(QObject *parent = Q_NULLPTR);
    ~SelectionModelModel();

    /*! show only selection models for @p model */
    void setModel(QAbstractItemModel *model);

protected:
    bool filterAcceptsObject(QObject *object) const Q_DECL_OVERRIDE;

private:
    QAbstractItemModel *m_model;
};
}

#endif // GAMMARAY_SELECTIONMODELMODEL_H
