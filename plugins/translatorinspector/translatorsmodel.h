/*
  translatorsmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

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

#ifndef TRANSLATORSMODEL_H
#define TRANSLATORSMODEL_H

#include <QAbstractTableModel>

namespace GammaRay {
class TranslatorWrapper;

class TranslatorsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TranslatorsModel(QObject *parent = nullptr);

    enum ExtraRoles
    {
        ObjectIdRole = Qt::UserRole + 1
    };

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QMap<int, QVariant> itemData(const QModelIndex & index) const override;

    TranslatorWrapper *translator(const QModelIndex &index) const;

public slots:
    void registerTranslator(TranslatorWrapper *translator);
    void unregisterTranslator(TranslatorWrapper *translator);

private slots:
    void sourceDataChanged();

private:
    QList<TranslatorWrapper *> m_translators;
};
}

#endif
