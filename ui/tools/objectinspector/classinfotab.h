/*
  classinfotab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
