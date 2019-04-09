/*
  mimetypesmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MIMETYPES_MIMETYPESMODEL_H
#define GAMMARAY_MIMETYPES_MIMETYPESMODEL_H

#include "common/modelroles.h"

#include <QMimeDatabase>
#include <QStandardItemModel>

namespace GammaRay {
class MimeTypesModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum Roles {
        IconNameRole = UserRole + 1,
        GenericIconNameRole
    };

    explicit MimeTypesModel(QObject *parent = nullptr);
    ~MimeTypesModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void fillModel();
    QVector<QStandardItem *> itemsForType(const QString &mimeTypeName);
    void makeItemsForType(const QString &mimeTypeName);
    static QList<QStandardItem *> makeRowForType(const QMimeType &mt);
    QSet<QString> normalizedMimeTypeNames(const QStringList &typeNames) const;

    QHash<QString, QVector<QStandardItem *> > m_mimeTypeNodes;
    QMimeDatabase m_db;
    bool m_modelFilled;
};
}

#endif // GAMMARAY_MIMETYPESMODEL_H
