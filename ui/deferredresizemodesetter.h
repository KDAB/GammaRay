/*
  deferredresizemodesetter.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_DEFERREDRESIZEMODESETTER_H
#define GAMMARAY_DEFERREDRESIZEMODESETTER_H

#include "gammaray_ui_export.h"

#include <QObject>
#include <QHeaderView>

namespace GammaRay {

/** Sets the resize mode on a QHeaderView section once that section is actually available.
 *
 * This is a workaround for QHeaderView asserting when manipulating if the corresponding section
 * hasn't been loaded yet by the corresponding model, as well as forgetting the setting if the
 * model reports a columnCount of 0 again inbetween.
 */
class GAMMARAY_UI_EXPORT DeferredResizeModeSetter : public QObject
{
  Q_OBJECT
public:
  DeferredResizeModeSetter(QHeaderView *headerView, int section, QHeaderView::ResizeMode resizeMode);
  ~DeferredResizeModeSetter();

private slots:
  void setSectionResizeMode();

private:
  QHeaderView *m_view;
  int m_section;
  QHeaderView::ResizeMode m_resizeMode;
};

}

#endif // GAMMARAY_DEFERREDRESIZEMODESETTER_H
