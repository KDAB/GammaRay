/*
  materialtab.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef GAMMARAY_QUICKINSPECTOR_MATERIALTAB_H
#define GAMMARAY_QUICKINSPECTOR_MATERIALTAB_H

#include <QWidget>

namespace GammaRay {
class MaterialExtensionInterface;

class Ui_MaterialTab;
class PropertyWidget;

class MaterialTab : public QWidget
{
    Q_OBJECT
public:
    explicit MaterialTab(PropertyWidget *parent);
    ~MaterialTab() override;

private:
    void setObjectBaseName(const QString &baseName);
    void propertyContextMenu(QPoint pos);

private slots:
    void shaderSelectionChanged(int idx);
    void showShader(const QString &shaderSource);

private:
    QScopedPointer<Ui_MaterialTab> m_ui;
    MaterialExtensionInterface *m_interface;
};
}

#endif // MATERIALTAB_H
