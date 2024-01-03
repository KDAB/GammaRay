/*
  propertyenumeditor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    Q_PROPERTY(GammaRay::EnumValue enumValue READ enumValue WRITE setEnumValue USER true) // clazy:exclude=qproperty-without-notify
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
