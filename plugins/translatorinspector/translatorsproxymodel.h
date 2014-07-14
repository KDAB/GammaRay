/*
  translatorsproxymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

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

#ifndef TRANSLATORSPROXYMODEL_H
#define TRANSLATORSPROXYMODEL_H

#include <QIdentityProxyModel>

namespace GammaRay {
class TranslatorWrapper;

class TranslatorsProxyModel : public QIdentityProxyModel
{
  Q_OBJECT

  public:
    TranslatorsProxyModel(QObject *parent = 0);

    void setSourceModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;
    int columnCount(
            const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &proxyIndex, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const Q_DECL_OVERRIDE;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    TranslatorWrapper *translator(const QModelIndex &index) const;

  private slots:
    void sourceDataChanged();
    void translatorsAdded(const QModelIndex &, const int start, const int end);
    void translatorsRemoved(const QModelIndex &, const int start, const int end);
};

}

#endif
