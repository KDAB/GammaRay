/*
  actionmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef GAMMARAY_ACTIONINSPECTOR_ACTIONMODEL_H
#define GAMMARAY_ACTIONINSPECTOR_ACTIONMODEL_H

#include "include/objecttypefilterproxymodel.h"

class QAction;

namespace GammaRay {

class ActionValidator;

/**
 * This QSFPM additionally overwrites index(...) and flags(...)
 * to support additional columns
 *
 * @see http://www.koders.com/cpp/fid78857CC6C2407C42429B1F9AC7F65F292F59CA9E.aspx?s=search
 */
class ActionModel : public ObjectFilterProxyModelBase
{
  Q_OBJECT

  public:
    enum Column {
      AddressColumn,
      NameColumn,
      CheckablePropColumn,
      CheckedPropColumn,
      PriorityPropColumn,
      ShortcutsPropColumn,
      /** Mark column count */
      ColumnCount
    };

    explicit ActionModel(QObject *parent = 0);

    virtual ~ActionModel();

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  protected:
    virtual bool filterAcceptsObject(QObject *object) const;

  private Q_SLOTS:
    void handleRowsInserted(const QModelIndex &, int, int);
    void handleRowsRemoved(const QModelIndex &, int, int);
    void handleModelReset();

  private:
    int sourceColumnCount(const QModelIndex &parent) const;

    QAction *actionForIndex(const QModelIndex &index) const;
    QList<QAction*> actions() const;
    QList<QAction*> actions(const QModelIndex &parent, int start, int end);

    ActionValidator *m_duplicateFinder;
};

}

#endif // GAMMARAY_ACTIONMODEL_H
