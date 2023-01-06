/*
  materialtab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
