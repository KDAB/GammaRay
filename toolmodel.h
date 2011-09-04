/*
  toolmodel.h

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krauss@kdab.com>

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

#ifndef ENDOSCOPE_TOOLMODEL_H
#define ENDOSCOPE_TOOLMODEL_H

#include <qabstractitemmodel.h>
#include <qvector.h>
#include <QSet>
#include <QPointer>

namespace Endoscope {

class ToolFactory;

/** Manages the list of available probing tools.
 */
class ToolModel : public QAbstractListModel
{
  Q_OBJECT
  public:
    enum Role {
      ToolFactoryRole = Qt::UserRole + 1,
      ToolWidgetRole
    };
    explicit ToolModel(QObject *parent = 0);
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

  public slots:
    /** Check if we have to activate tools for this type */
    void objectAdded(const QPointer<QObject> &obj);

  private:
    /** Check if we have to activate tools for this type */
    void objectAdded(const QMetaObject *mo);

  private:
    QVector<ToolFactory*> m_tools;
    QHash<ToolFactory*, QWidget*> m_toolWidgets;
    QSet<ToolFactory*> m_inactiveTools;
};

}

#endif // ENDOSCOPE_TOOLMODEL_H
