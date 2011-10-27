/*
  toolmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_TOOLMODEL_H
#define GAMMARAY_TOOLMODEL_H

#include <qabstractitemmodel.h>
#include <qvector.h>
#include <QSet>

class QStringList;
namespace GammaRay {

class ToolFactory;

/**
 * Manages the list of available probing tools.
 */
class ToolModel : public QAbstractListModel
{
  Q_OBJECT
  public:
    enum Role {
      ToolFactoryRole = Qt::UserRole + 1,
      ToolWidgetRole,
      ToolIdRole
    };
    explicit ToolModel(QObject *parent = 0);
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

  public slots:
    /** Check if we have to activate tools for this type */
    void objectAdded(QObject *obj);
    void objectAddedMainThread(QObject *obj);

  private:
    /**
     * Check if we have to activate tools for this type
     *
     * NOTE: must be called from the GUI thread
     */
    void objectAdded(const QMetaObject *mo);

  private:
    QVector<ToolFactory*> m_tools;
    QHash<ToolFactory*, QWidget*> m_toolWidgets;
    QSet<ToolFactory*> m_inactiveTools;
};

}

#endif // GAMMARAY_TOOLMODEL_H
