/*
  translatorinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "translatorinspector.h"

#include <QCoreApplication>
#include <QItemSelectionModel>
#include <QIdentityProxyModel>
#include <private/qcoreapplication_p.h>

#include <core/probeinterface.h>
#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <core/objecttypefilterproxymodel.h>

#include "translatorwrapper.h"
#include "translatorsmodel.h"

using namespace GammaRay;

TranslatorInspector::TranslatorInspector(ProbeInterface *probe,
                                       QObject *parent)
    : TranslatorInspectorInterface("com.kdab.GammaRay.TranslatorInspector",
                                  parent),
      m_probe(probe)
{
  m_translatorsModel = new TranslatorsModel(this);
  probe->registerModel("com.kdab.GammaRay.TranslatorsModel",
                       m_translatorsModel);

  m_translationsModel = new QIdentityProxyModel(this);
  probe->registerModel("com.kdab.GammaRay.TranslationsModel",
                       m_translationsModel);

  m_selectionModel = ObjectBroker::selectionModel(m_translatorsModel);
  connect(m_selectionModel,
          SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(selectionChanged(QItemSelection)));

  m_translationsSelectionModel =
      ObjectBroker::selectionModel(m_translationsModel);

  m_fallbackWrapper = new TranslatorWrapper(new FallbackTranslator(this), this);
  m_translatorsModel->registerTranslator(m_fallbackWrapper);
  QCoreApplicationPrivate *obj = static_cast<QCoreApplicationPrivate *>(
      QCoreApplicationPrivate::get(qApp));
  obj->translators.append(m_fallbackWrapper);

  qApp->installEventFilter(this);
  sendLanguageChangeEvent();
}

void TranslatorInspector::sendLanguageChangeEvent()
{
  QEvent event(QEvent::LanguageChange);
  qApp->sendEvent(qApp, &event);
}
void TranslatorInspector::resetTranslations()
{
  const QItemSelection translatorsSelection = m_selectionModel->selection();
  if (translatorsSelection.isEmpty()) {
    return;
  }
  TranslatorWrapper *translator =
      m_translatorsModel->translator(translatorsSelection.first().topLeft());
  Q_ASSERT(translator);
  const QItemSelection translationsSelection =
      m_translationsSelectionModel->selection();
  if (translationsSelection.isEmpty()) {
    return;
  }
  translator->model()->resetTranslations(
      translationsSelection.first().topLeft(),
      translationsSelection.last().bottomRight());
}
bool TranslatorInspector::eventFilter(QObject *object, QEvent *event)
{
  if (event->type() == QEvent::LanguageChange) {
    QCoreApplicationPrivate *obj = static_cast<QCoreApplicationPrivate *>(
        QCoreApplicationPrivate::get(qApp));
    for (int i = 0; i < obj->translators.size(); ++i) {
      if (obj->translators.at(i)->metaObject()->className() ==
          TranslatorWrapper::staticMetaObject.className()) {
        continue; // it's already setup correctly
      } else {
        /* wrap the translator set with installTranslator in a TranslatorWrapper
         * and make sure we use the TranslatorWrapper instead of the original
         * translator
         */
        TranslatorWrapper *wrapper =
            new TranslatorWrapper(obj->translators[i], this);
        obj->translators[i] = wrapper;
        m_translatorsModel->registerTranslator(wrapper);
        connect(wrapper,
                &TranslatorWrapper::destroyed,
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
                m_translationsModel,
#endif
                [wrapper, this](QObject *) { m_translatorsModel->unregisterTranslator(wrapper); });
      }
    }
    for (auto it = obj->translators.begin(); it != obj->translators.end(); ++it)
    {
      TranslatorWrapper *wrapper = qobject_cast<TranslatorWrapper *>(*it);
      Q_ASSERT(wrapper);
      wrapper->model()->resetAllUnchanged();
    }
  }
  return QObject::eventFilter(object, event);
}
void TranslatorInspector::selectionChanged(const QItemSelection &selection)
{
  m_translationsModel->setSourceModel(0);
  if (!selection.isEmpty()) {
    TranslatorWrapper *translator =
        m_translatorsModel->translator(selection.first().topLeft());
    if (translator) {
      m_translationsModel->setSourceModel(translator->model());
    }
  }
}
