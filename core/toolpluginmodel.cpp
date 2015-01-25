/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "toolpluginmodel.h"
#include "toolfactory.h"

using namespace GammaRay;

ToolPluginModel::ToolPluginModel(const QVector<ToolFactory*> &plugins, QObject* parent):
  QAbstractTableModel(parent),
  m_tools(plugins)
{
}

ToolPluginModel::~ToolPluginModel()
{
}

int ToolPluginModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 3;
}

int ToolPluginModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_tools.size();
}

QVariant ToolPluginModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  const int row = index.row();
  const int column = index.column();
  if (role == Qt::DisplayRole) {
    ToolFactory *factory = m_tools[row];
    switch (column) {
    case 0:
      return factory->id();
    case 1:
      return factory->name();
    case 2:
      return factory->supportedTypes().join(", ");
    }
  }
  return QVariant();
}

QVariant ToolPluginModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("Id");
    case 1:
      return tr("Name");
    case 2:
      return tr("Supported types");
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

