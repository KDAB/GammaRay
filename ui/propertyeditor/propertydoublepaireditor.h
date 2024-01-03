/*
  propertydoublepaireditor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    Q_PROPERTY(QPointF pointF READ pointF WRITE setPointF USER true) // clazy:exclude=qproperty-without-notify
public:
    explicit PropertyPointFEditor(QWidget *parent = nullptr);
    QPointF pointF() const;
    void setPointF(const QPointF &point);
};

class PropertySizeFEditor : public PropertyDoublePairEditor
{
    Q_OBJECT
    Q_PROPERTY(QSizeF sizeF READ sizeF WRITE setSizeF USER true) // clazy:exclude=qproperty-without-notify
public:
    explicit PropertySizeFEditor(QWidget *parent = nullptr);
    QSizeF sizeF() const;
    void setSizeF(const QSizeF &size);
};
}

#endif // GAMMARAY_PROPERTYDOUBLEPAIREDITOR_H
