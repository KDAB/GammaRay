/*
  palettemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PALETTEMODEL_H
#define GAMMARAY_PALETTEMODEL_H

#include <qabstractitemmodel.h>
#include <qpalette.h>

namespace GammaRay {

/**
 * Model showing the content of a QPalette.
 */
class PaletteModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit PaletteModel(QObject* parent = 0);

  QPalette palette() const;
  void setPalette(const QPalette &palette);

  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
  QPalette m_palette;
};

}

#endif // GAMMARAY_PALETTEMODEL_H
