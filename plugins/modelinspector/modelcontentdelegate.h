/*
  modelcontentdelegate.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_MODELCONTENTDELEGATE_H
#define GAMMARAY_MODELCONTENTDELEGATE_H

#include <QStyledItemDelegate>

namespace GammaRay {

class ModelContentDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ModelContentDelegate(QObject *parent = nullptr);
    ~ModelContentDelegate() override;

    void paint(QPainter *painter, const QStyleOptionViewItem &origOption, const QModelIndex &index) const override;
};

}

#endif // GAMMARAY_MODELCONTENTDELEGATE_H
