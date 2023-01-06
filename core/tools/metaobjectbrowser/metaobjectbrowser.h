/*
  metaobjectbrowser.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_METAOBJECTBROWSER_METATYPEBROWSER_H
#define GAMMARAY_METAOBJECTBROWSER_METATYPEBROWSER_H

#include "toolfactory.h"

QT_BEGIN_NAMESPACE
class QAbstractProxyModel;
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
class MetaObjectTreeModel;
class PropertyController;

class MetaObjectBrowser : public QObject
{
    Q_OBJECT

public:
    explicit MetaObjectBrowser(Probe *probe, QObject *parent = nullptr);

public Q_SLOTS:
    static void rescanMetaTypes();

private Q_SLOTS:
    void objectSelectionChanged(const QItemSelection &selection);
    void qobjectSelected(QObject *obj);
    void voidPtrObjectSelected(void *obj, const QString &typeName);

private:
    void metaObjectSelected(const QMetaObject *mo);

    static void scanForMetaObjectProblems();
    static void doProblemScan(const QMetaObject *parent);

    PropertyController *m_propertyController;
    MetaObjectTreeModel *m_motm;
    QAbstractProxyModel *m_model;
};

class MetaObjectBrowserFactory : public QObject,
                                 public StandardToolFactory<QObject, MetaObjectBrowser>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)

public:
    explicit MetaObjectBrowserFactory(QObject *parent)
        : QObject(parent)
    {
    }

    QVector<QByteArray> selectableTypes() const override;
};
}

#endif // GAMMARAY_METAOBJECTBROWSER_METATYPEBROWSER_H
