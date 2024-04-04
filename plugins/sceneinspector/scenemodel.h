/*
  scenemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SCENEINSPECTOR_SCENEMODEL_H
#define GAMMARAY_SCENEINSPECTOR_SCENEMODEL_H

#include <QAbstractItemModel>
#include <common/modelroles.h>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsItem;
QT_END_NAMESPACE

namespace GammaRay {
class SceneModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Role
    {
        SceneItemRole = UserRole + 1
    };
    explicit SceneModel(QObject *parent = nullptr);
    void setScene(QGraphicsScene *scene);
    QGraphicsScene *scene() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    QList<QGraphicsItem *> topLevelItems() const;
    int rowForItem(QGraphicsItem *item) const;

    /// Returns a string type name for the given QGV item type id
    QString typeName(int itemType) const;

    QGraphicsScene *m_scene;
    QHash<int, QString> m_typeNames;
};
}

#endif // GAMMARAY_SCENEMODEL_H
