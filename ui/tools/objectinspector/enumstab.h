/*
  enumstab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef ENUMSTAB_H
#define ENUMSTAB_H

#include <QWidget>
#include <memory>

namespace GammaRay {
class Ui_EnumsTab;
class PropertyWidget;

class EnumsTab : public QWidget
{
    Q_OBJECT
public:
    explicit EnumsTab(PropertyWidget *parent);
    ~EnumsTab() override;

private:
    void setObjectBaseName(const QString &baseName);

private:
    std::unique_ptr<Ui_EnumsTab> m_ui;
};
}

#endif // ENUMSTAB_H
