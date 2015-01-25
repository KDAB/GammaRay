/*
  abstractstyleelementmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_STYLEINSPECTOR_ABSTRACTSTYLEELEMENTMODEL_H
#define GAMMARAY_STYLEINSPECTOR_ABSTRACTSTYLEELEMENTMODEL_H

#include <QAbstractItemModel>
#include <QPointer>

class QStyle;

namespace GammaRay {

/**
 * Base class for all models showing style elements.
 */
class AbstractStyleElementModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    explicit AbstractStyleElementModel(QObject *parent = 0);

    void setStyle(QStyle *style);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

  protected:
    virtual QVariant doData(int row, int column, int role) const = 0;
    virtual int doColumnCount() const = 0;
    virtual int doRowCount() const = 0;

    /** Returns @c true if we are looking at the primary style of the application
     *  ie. the one set in QApplication. This takes proxy styles into account.
     */
    bool isMainStyle() const;

  protected:
    QPointer<QStyle> m_style;
};

}

#endif // GAMMARAY_ABSTRACTSTYLEELEMENTMODEL_H
