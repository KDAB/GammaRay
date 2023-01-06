/*
  methodstab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef METHODSTAB_H
#define METHODSTAB_H

#include <QWidget>
#include <memory>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class MethodsExtensionInterface;

class Ui_MethodsTab;
class PropertyWidget;

class MethodsTab : public QWidget
{
    Q_OBJECT
public:
    explicit MethodsTab(PropertyWidget *parent);
    ~MethodsTab() override;

private:
    void setObjectBaseName(const QString &baseName);

private slots:
    void methodActivated(const QModelIndex &index);
    void methodContextMenu(const QPoint &pos);

private:
    std::unique_ptr<Ui_MethodsTab> m_ui;
    MethodsExtensionInterface *m_interface;

    QString m_objectBaseName;
};
}

#endif // METHODSTAB_H
