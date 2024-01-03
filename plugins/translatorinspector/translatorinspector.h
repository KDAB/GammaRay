/*
  translatorinspector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef TRANSLATORINSPECTOR_H
#define TRANSLATORINSPECTOR_H

#include "translatorinspectorinterface.h"

#include <core/toolfactory.h>

#include <QAbstractItemModel>
#include <QTranslator>

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
class QItemSelection;
class QAbstractProxyModel;
QT_END_NAMESPACE

namespace GammaRay {
class TranslatorsModel;
class TranslatorWrapper;
class FallbackTranslator;

class TranslatorInspector : public TranslatorInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::TranslatorInspectorInterface)

public:
    explicit TranslatorInspector(Probe *probe, QObject *parent = nullptr);

public slots:
    void sendLanguageChangeEvent() override;
    void resetTranslations() override;

private slots:
    void selectionChanged(const QItemSelection &selection);
    void objectSelected(QObject *obj);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    void sendLanguageChangeEvent_impl();
    static void registerMetaTypes();

    QItemSelectionModel *m_selectionModel;
    QItemSelectionModel *m_translationsSelectionModel;
    TranslatorsModel *m_translatorsModel;
    QAbstractProxyModel *m_translationsModel;
    Probe *m_probe;
    TranslatorWrapper *m_fallbackWrapper;
};

class TranslatorInspectorFactory : public QObject,
                                   public StandardToolFactory<QTranslator, TranslatorInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_translatorinspector.json")

public:
    explicit TranslatorInspectorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif
