/*
  toolmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_TOOLMODEL_H
#define GAMMARAY_TOOLMODEL_H

#include <common/pluginmanager.h>
#include <common/modelroles.h>

#include <QAbstractListModel>
#include <QSet>
#include <QVector>
#include <QPointer>

namespace GammaRay {

class ToolFactory;
class ProxyToolFactory;

typedef PluginManager<ToolFactory, ProxyToolFactory> ToolPluginManager;

/**
 * Manages the list of available probing tools.
 */
class ToolModel : public QAbstractListModel
{
  Q_OBJECT
  public:
    explicit ToolModel(QObject *parent = 0);
    ~ToolModel();
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QMap<int, QVariant> itemData(const QModelIndex& index) const Q_DECL_OVERRIDE;

    /** returns all tools provided by plugins for the ToolPluginModel. */
    QVector<ToolFactory*> plugins() const;
    /** returns all plugin load errors. */
    PluginLoadErrors pluginErrors() const;
    /** returns the tool that is best suited to show information about \p object. */
    QModelIndex toolForObject(QObject *object) const;
    /** returns the tool that is best suited to show information about \p object. */
    QModelIndex toolForObject(const void *object, const QString &typeName) const;

  public slots:
    /** Check if we have to activate tools for this type */
    void objectAdded(QObject *obj);

  private:
    /**
     * Check if we have to activate tools for this type
     *
     * NOTE: must be called from the GUI thread
     */
    void objectAdded(const QMetaObject *mo);

    void addToolFactory(ToolFactory* tool);

  private:
    QVector<ToolFactory*> m_tools;
    QSet<ToolFactory*> m_inactiveTools;
    QSet<const QMetaObject*> m_knownMetaObjects;
    QPointer<QWidget> m_parentWidget;
    QScopedPointer<ToolPluginManager> m_pluginManager;
};

}

#endif // GAMMARAY_TOOLMODEL_H
