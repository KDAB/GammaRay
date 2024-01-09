/*
  translatorinspector.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "translatorinspector.h"
#include "translatorwrapper.h"
#include "translatorsmodel.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/remote/serverproxymodel.h>

#include <common/objectbroker.h>
#include <common/objectid.h>
#include <common/objectmodel.h>

#include <QCoreApplication>
#include <QItemSelectionModel>
#include <private/qcoreapplication_p.h>

using namespace GammaRay;

TranslatorInspector::TranslatorInspector(Probe *probe, QObject *parent)
    : TranslatorInspectorInterface(QStringLiteral("com.kdab.GammaRay.TranslatorInspector"), parent)
    , m_probe(probe)
{
    registerMetaTypes();

    m_translatorsModel = new TranslatorsModel(this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.TranslatorsModel"),
                         m_translatorsModel);

    m_translationsModel = new ServerProxyModel<QSortFilterProxyModel>(this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.TranslationsModel"),
                         m_translationsModel);

    m_selectionModel = ObjectBroker::selectionModel(m_translatorsModel);
    connect(m_selectionModel,
            &QItemSelectionModel::selectionChanged,
            this, &TranslatorInspector::selectionChanged);

    m_translationsSelectionModel = ObjectBroker::selectionModel(m_translationsModel);

    m_fallbackWrapper = new TranslatorWrapper(new FallbackTranslator(this), this);
    m_translatorsModel->registerTranslator(m_fallbackWrapper);
    QCoreApplicationPrivate *obj = static_cast<QCoreApplicationPrivate *>(
        QCoreApplicationPrivate::get(qApp));
    obj->translators.append(m_fallbackWrapper);

    qApp->installEventFilter(this);
    sendLanguageChangeEvent_impl();

    connect(probe, &Probe::objectSelected, this, &TranslatorInspector::objectSelected);
}

void TranslatorInspector::sendLanguageChangeEvent()
{
    sendLanguageChangeEvent_impl();
}

void TranslatorInspector::sendLanguageChangeEvent_impl()
{
    QEvent event(QEvent::LanguageChange);
    qApp->sendEvent(qApp, &event);
}

void TranslatorInspector::resetTranslations()
{
    const QItemSelection translatorsSelection = m_selectionModel->selection();
    if (translatorsSelection.isEmpty())
        return;
    TranslatorWrapper *translator = m_translatorsModel->translator(translatorsSelection.first().topLeft());
    Q_ASSERT(translator);
    const QItemSelection translationsProxySelection = m_translationsSelectionModel->selection();
    const QItemSelection translationsSourceSelection = m_translationsModel->mapSelectionToSource(translationsProxySelection);
    if (translationsSourceSelection.isEmpty())
        return;
    translator->model()->resetTranslations(translationsSourceSelection);
}

bool TranslatorInspector::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        QCoreApplicationPrivate *obj = static_cast<QCoreApplicationPrivate *>(
            QCoreApplicationPrivate::get(qApp));
        for (int i = 0; i < obj->translators.size(); ++i) {
            if (obj->translators.at(i)->metaObject() == &TranslatorWrapper::staticMetaObject) {
                continue; // it's already setup correctly
            } else {
                /* wrap the translator set with installTranslator in a TranslatorWrapper
                 * and make sure we use the TranslatorWrapper instead of the original
                 * translator
                 */
                auto wrapper = new TranslatorWrapper(obj->translators.at(i), this);
                obj->translators[i] = wrapper;
                m_translatorsModel->registerTranslator(wrapper);
                connect(wrapper, &TranslatorWrapper::destroyed, m_translationsModel, [wrapper, this](QObject *) {
                    m_translatorsModel->unregisterTranslator(wrapper);
                });
            }
        }
        for (auto it = obj->translators.constBegin(); it != obj->translators.constEnd(); ++it) {
            TranslatorWrapper *wrapper = qobject_cast<TranslatorWrapper *>(*it);
            Q_ASSERT(wrapper);
            wrapper->model()->resetAllUnchanged();
        }
    }
    return QObject::eventFilter(object, event);
}

void TranslatorInspector::selectionChanged(const QItemSelection &selection)
{
    m_translationsModel->setSourceModel(nullptr);
    if (!selection.isEmpty()) {
        TranslatorWrapper *translator = m_translatorsModel->translator(selection.first().topLeft());
        if (translator)
            m_translationsModel->setSourceModel(translator->model());
    }
}

void TranslatorInspector::objectSelected(QObject *obj)
{
    auto t = qobject_cast<QTranslator *>(obj);
    if (!t)
        return;

    const auto indexList = m_translatorsModel->match(m_translationsModel->index(0, 0),
                                                     TranslatorsModel::ObjectIdRole,
                                                     QVariant::fromValue(ObjectId(t)), 1,
                                                     Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;

    const auto &index = indexList.first();
    m_selectionModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows | QItemSelectionModel::Current);
}

void TranslatorInspector::registerMetaTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(QTranslator, QObject);
    MO_ADD_PROPERTY_RO(QTranslator, isEmpty);
}
