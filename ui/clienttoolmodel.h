/*
  clienttoolmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_CLIENTTOOLMODEL_H
#define GAMMARAY_CLIENTTOOLMODEL_H

#include "gammaray_ui_export.h"

#include <QHash>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QSet>

class QWidget;

namespace GammaRay {

class ToolUiFactory;

/** @brief Tool model for the client that implements the custom roles that return widget/factory pointers.
 *
 *  This is needed when implementing your own client UI embedded into a different application.
 */
class GAMMARAY_UI_EXPORT ClientToolModel : public QSortFilterProxyModel
{
  Q_OBJECT
public:
  explicit ClientToolModel(QObject* parent = 0);
  ~ClientToolModel();

  QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

private slots:
  void updateToolInitialization(const QModelIndex & topLeft, const QModelIndex & bottomRight);

private:
  typedef QHash<QString, QPointer<QWidget>> WidgetsHash;
  mutable WidgetsHash m_widgets; // ToolId -> Widget
  QPointer<QWidget> m_parentWidget;
};

}

#endif // GAMMARAY_CLIENTTOOLMODEL_H
