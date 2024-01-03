/*
  mimetypesmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    enum Roles
    {
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

    QHash<QString, QVector<QStandardItem *>> m_mimeTypeNodes;
    QMimeDatabase m_db;
    bool m_modelFilled;
};
}

#endif // GAMMARAY_MIMETYPESMODEL_H
