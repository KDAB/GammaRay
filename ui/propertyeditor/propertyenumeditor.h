/*
  propertyenumeditor.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
