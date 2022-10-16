/*
  propertyintpaireditor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROPERTYINTPAIREDITOR_H
#define GAMMARAY_PROPERTYINTPAIREDITOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPoint;
QT_END_NAMESPACE

namespace GammaRay {
namespace Ui {
class PropertyIntPairEditor;
}

/** Property editor for QPoint, QSize and anything else consisting of two integer values. */
class PropertyIntPairEditor : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyIntPairEditor(QWidget *parent = nullptr);
    ~PropertyIntPairEditor() override;

protected:
    QScopedPointer<Ui::PropertyIntPairEditor> ui;
};

/** Property editor for points. Since QStyledItemDelegate ignore valuePropertyName and insists on
 * USER properties we need one class per type here...
 */
class PropertyPointEditor : public PropertyIntPairEditor
{
    Q_OBJECT
    Q_PROPERTY(QPoint point READ point WRITE setPoint USER true)
public:
    explicit PropertyPointEditor(QWidget *parent = nullptr);

    QPoint point() const;
    void setPoint(const QPoint &point);
};

/** Same again for size. */
class PropertySizeEditor : public PropertyIntPairEditor
{
    Q_OBJECT
    Q_PROPERTY(QSize sizeValue READ sizeValue WRITE setSizeValue USER true)
public:
    explicit PropertySizeEditor(QWidget *parent = nullptr);

    QSize sizeValue() const;
    void setSizeValue(const QSize &size);
};
}

#endif // GAMMARAY_PROPERTYINTPAIREDITOR_H
