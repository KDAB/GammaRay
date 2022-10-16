/*
  propertywidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROPERTYWIDGET_H
#define GAMMARAY_PROPERTYWIDGET_H

#include <QTabWidget>
#include <QPointer>
#include <QHash>

#include "gammaray_ui_export.h"
#include "propertywidgettab.h"

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAbstractItemView;
class QModelIndex;
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
class Ui_PropertyWidget;
class PropertyControllerInterface;

/** @brief Client-side counter-part GammaRay::PropertyController. */
class GAMMARAY_UI_EXPORT PropertyWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit PropertyWidget(QWidget *parent = nullptr);
    virtual ~PropertyWidget();

    QString objectBaseName() const;
    void setObjectBaseName(const QString &baseName);

    /** Register a new tab widget factory.
     * @tparam T A widget type providing the tab UI
     * @param name The internal object name of this extension.
     * @param label The user-visible tab label of this extension.
     * @param priority This is used to keep tabs in a stable order, tabs are ordered
     *   left to right with increasing priority.
     */
    template<typename T>
    static void registerTab(const QString &name, const QString &label,
                            int priority = 1000)
    {
        registerTab(new PropertyWidgetTabFactory<T>(name, label, priority));
    }

    //! \internal
    static void cleanupTabs();
signals:
    void tabsUpdated();

private:
    static void registerTab(PropertyWidgetTabFactoryBase *factory);
    void createWidgets();
    bool extensionAvailable(PropertyWidgetTabFactoryBase *factory) const;
    bool factoryInUse(PropertyWidgetTabFactoryBase *factory) const;

private slots:
    void updateShownTabs();
    void slotCurrentTabChanged();

private:
    QString m_objectBaseName;

    QTimer *m_tabsUpdatedTimer;
    QWidget *m_lastManuallySelectedWidget;

    struct PageInfo
    {
        PropertyWidgetTabFactoryBase *factory;
        QWidget *widget;
    };
    QVector<PageInfo> m_pages;

    PropertyControllerInterface *m_controller;

    static QVector<PropertyWidgetTabFactoryBase *> s_tabFactories;
    static QVector<PropertyWidget *> s_propertyWidgets;
};
}

#endif // GAMMARAY_PROPERTYWIDGET_H
