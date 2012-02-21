/*
  widgettreemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_WIDGETTREEMODEL_H
#define GAMMARAY_WIDGETTREEMODEL_H

#include <objecttypefilterproxymodel.h>

namespace GammaRay {

/** Widget tree model.
 * @todo Show layout hierarchy instead of object hierarchy.
 */
class WidgetTreeModel : public ObjectFilterProxyModelBase
{
  Q_OBJECT
  public:
    explicit WidgetTreeModel(QObject *parent = 0);
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  protected:
    virtual bool filterAcceptsObject(QObject *object) const;
};

}

#endif // GAMMARAY_WIDGETTREEMODEL_H
