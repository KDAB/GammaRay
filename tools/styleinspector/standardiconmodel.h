/*
  standardiconmodel.h

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

#ifndef GAMMARAY_STANDARDICONMODEL_H
#define GAMMARAY_STANDARDICONMODEL_H

#include <qabstractitemmodel.h>
#include <qstyle.h>

namespace GammaRay {

/**
 * Lists all standard icons of a style.
 */
class StandardIconModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit StandardIconModel(QObject* parent = 0);

  void setStyle(QStyle* style);

  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
  QVariant dataForStandardIcon(QStyle::StandardPixmap stdPix, const QString &name, int column, int role) const;

private:
  QStyle* m_style;
};

}

#endif // GAMMARAY_STANDARDICONMODEL_H
