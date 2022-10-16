/*
  propertyenumeditor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROPERTYENUMEDITOR_H
#define GAMMARAY_PROPERTYENUMEDITOR_H

#include <common/enumvalue.h>

#include <QComboBox>

namespace GammaRay {

class PropertyEnumEditorModel;

class PropertyEnumEditor : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(GammaRay::EnumValue enumValue READ enumValue WRITE setEnumValue USER true)
public:
    explicit PropertyEnumEditor(QWidget *parent = nullptr);
    ~PropertyEnumEditor() override;

    EnumValue enumValue() const;
    void setEnumValue(const EnumValue &value);

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void definitionChanged(int id);
    void slotCurrentIndexChanged(int index);

private:
    void updateCurrentIndex();
    void setupView();

    PropertyEnumEditorModel *m_model;
};
}

#endif // GAMMARAY_PROPERTYENUMEDITOR_H
