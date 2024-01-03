/*
  widgettreemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_WIDGETINSPECTOR_WIDGETTREEMODEL_H
#define GAMMARAY_WIDGETINSPECTOR_WIDGETTREEMODEL_H

#include <core/objecttypefilterproxymodel.h>

namespace GammaRay {
/** Widget tree model.
 * @todo Show layout hierarchy instead of object hierarchy.
 */
class WidgetTreeModel : public ObjectFilterProxyModelBase
{
    Q_OBJECT
public:
    explicit WidgetTreeModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

    Q_INVOKABLE static QPair<int, QVariant> defaultSelectedItem();

protected:
    bool filterAcceptsObject(QObject *object) const override;
};
}

#endif // GAMMARAY_WIDGETTREEMODEL_H
