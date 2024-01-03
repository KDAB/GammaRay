/*
  propertiestab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef PROPERTIESTAB_H
#define PROPERTIESTAB_H

#include <QWidget>
#include <memory>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class PropertiesExtensionInterface;

class Ui_PropertiesTab;
class PropertyWidget;

class PropertiesTab : public QWidget
{
    Q_OBJECT
public:
    explicit PropertiesTab(PropertyWidget *parent);
    ~PropertiesTab() override;

private:
    void setObjectBaseName(const QString &baseName);

private slots:
    void updateNewPropertyValueEditor();
    void validateNewProperty();
    void propertyContextMenu(const QPoint &pos);
    void addNewProperty();
    void hasValuesChanged();

private:
    std::unique_ptr<Ui_PropertiesTab> m_ui;
    PropertiesExtensionInterface *m_interface;

    QWidget *m_newPropertyValue;
};
}

#endif // PROPERTIESTAB_H
