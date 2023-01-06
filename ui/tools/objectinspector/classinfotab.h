/*
  classinfotab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef CLASSINFOTAB_H
#define CLASSINFOTAB_H

#include <QWidget>
#include <memory>

namespace GammaRay {
class Ui_ClassInfoTab;
class PropertyWidget;

class ClassInfoTab : public QWidget
{
    Q_OBJECT
public:
    explicit ClassInfoTab(PropertyWidget *parent);
    ~ClassInfoTab() override;

private:
    void setObjectBaseName(const QString &baseName);

private:
    std::unique_ptr<Ui_ClassInfoTab> m_ui;
};
}

#endif // CLASSINFOTAB_H
