/*
  deferredtreeviewconfiguration.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_DEFERREDTREEVIEWCONFIGURATION_H
#define GAMMARAY_DEFERREDTREEVIEWCONFIGURATION_H

#include "gammaray_ui_export.h"

#include <QObject>
#include <QVector>

class QModelIndex;
class QTreeView;

namespace GammaRay {

/**
 * @brief A utility helper to configure views for remote content.
 *
 * When @p expandNewContent is set to true, the tree view will stay expanded
 * when new content is added to the model.
 *
 * When @p selectNewContent is set to true, the tree view will select new content,
 * if no selection is already present.
 *
 * @note The tree view's model and selectionModel must be set beforehand and not
 * be changed afterwards!
 */
class GAMMARAY_UI_EXPORT DeferredTreeViewConfiguration : public QObject
{
  Q_OBJECT
  public:
    explicit DeferredTreeViewConfiguration(QTreeView *view,
                                           bool expandNewContent = true, bool selectNewContent = true,
                                           QObject *parent = 0);

    void hideColumn(int column);

  private slots:
    void rowsInserted(const QModelIndex &parent);
    void columnsInserted(const QModelIndex &parent);

  private:
    QTreeView *m_view;
    bool m_expand;
    bool m_select;
    QVector<int> m_hiddenColumns;
};

}

#endif // GAMMARAY_DEFERREDTREEVIEWCONFIGURATION_H
