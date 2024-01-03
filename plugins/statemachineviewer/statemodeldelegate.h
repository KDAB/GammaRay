/*
  statemodeldelegate.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_STATEMODELDELEGATE_H
#define GAMMARAY_STATEMODELDELEGATE_H

#include <QStyledItemDelegate>

namespace GammaRay {
class StateModelDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StateModelDelegate(QObject *parent = nullptr);

protected:
    void initStyleOption(QStyleOptionViewItem *option,
                         const QModelIndex &index) const override;
};
}

#endif // GAMMARAY_STATEMODELDELEGATE_H
