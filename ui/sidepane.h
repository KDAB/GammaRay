/*
  sidepane.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SIDEPANE_H
#define GAMMARAY_SIDEPANE_H

#include <QListView>

namespace GammaRay {
class SidePane : public QListView
{
    Q_OBJECT
public:
    explicit SidePane(QWidget *parent = nullptr);
    ~SidePane() override;

    QSize sizeHint() const override;
    void setModel(QAbstractItemModel *model) override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void updateSizeHint();

private:
    QPixmap m_background;
};
}

#endif // GAMMARAY_SIDEPANE_H
