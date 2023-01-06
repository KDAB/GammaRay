/*
  propertydoublepaireditor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROPERTYDOUBLEPAIREDITOR_H
#define GAMMARAY_PROPERTYDOUBLEPAIREDITOR_H

#include <QWidget>

namespace GammaRay {
namespace Ui {
class PropertyDoublePairEditor;
}

/** Property editor for pairs of doubles, such as PointF and SizeF. */
class PropertyDoublePairEditor : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyDoublePairEditor(QWidget *parent = nullptr);
    ~PropertyDoublePairEditor() override;

protected:
    QScopedPointer<Ui::PropertyDoublePairEditor> ui;
};

class PropertyPointFEditor : public PropertyDoublePairEditor
{
    Q_OBJECT
    Q_PROPERTY(QPointF pointF READ pointF WRITE setPointF USER true)
public:
    explicit PropertyPointFEditor(QWidget *parent = nullptr);
    QPointF pointF() const;
    void setPointF(const QPointF &point);
};

class PropertySizeFEditor : public PropertyDoublePairEditor
{
    Q_OBJECT
    Q_PROPERTY(QSizeF sizeF READ sizeF WRITE setSizeF USER true)
public:
    explicit PropertySizeFEditor(QWidget *parent = nullptr);
    QSizeF sizeF() const;
    void setSizeF(const QSizeF &size);
};
}

#endif // GAMMARAY_PROPERTYDOUBLEPAIREDITOR_H
