/*
  clienttoolmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_CLIENTTOOLMODEL_H
#define GAMMARAY_CLIENTTOOLMODEL_H

#include <QHash>
#include <QPointer>
#include <QSortFilterProxyModel>

class QWidget;

namespace GammaRay {

class ToolUiFactory;

/** Tool model for the client that implements the custom roles that return widget/factory pointers. */
class ClientToolModel : public QSortFilterProxyModel
{
  Q_OBJECT
public:
  explicit ClientToolModel(QObject* parent = 0);
  ~ClientToolModel();

  virtual QVariant data(const QModelIndex& index, int role) const;
  virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
  virtual Qt::ItemFlags flags(const QModelIndex &index) const;

private:
  void insertFactory(ToolUiFactory* factory);

private:
  QHash<QString, ToolUiFactory*> m_factories; // ToolId -> ToolUiFactory
  mutable QHash<QString, QWidget*> m_widgets; // ToolId -> Widget
  QPointer<QWidget> m_parentWidget;
};

}

#endif // GAMMARAY_CLIENTTOOLMODEL_H
