/*
  trailingcolorlabel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Christoph Sterz <christoph.sterz@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef TRAILINGCOLORLABEL_H
#define TRAILINGCOLORLABEL_H

#include <QWidget>
#include <QLabel>

namespace GammaRay {

class TrailingColorLabel : public QLabel
{
    Q_OBJECT
public:
    explicit TrailingColorLabel(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;
    QColor pickedColor() const;

signals:

public slots:
    void setPickedColor(QRgb color);

private:
    QRgb m_pickedColor;
};

}
#endif // TRAILINGCOLORLABEL_H
