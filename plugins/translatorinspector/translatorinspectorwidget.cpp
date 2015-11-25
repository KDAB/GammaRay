/*
  translatorinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#include "translatorinspectorwidget.h"
#include "ui_translatorinspectorwidget.h"

#include <QSortFilterProxyModel>

#include <ui/searchlinecontroller.h>
#include <common/objectbroker.h>
#include <common/endpoint.h>

using namespace GammaRay;

TranslatorInspectorClient::TranslatorInspectorClient(const QString &name, QObject *parent)
  : TranslatorInspectorInterface(name, parent)
{
}
void TranslatorInspectorClient::sendLanguageChangeEvent()
{
  Endpoint::instance()->invokeObject(name(), "sendLanguageChangeEvent");
}
void TranslatorInspectorClient::resetTranslations()
{
  Endpoint::instance()->invokeObject(name(), "resetTranslations");
}

TranslatorInspectorWidget::TranslatorInspectorWidget(QWidget *parent) :
  QWidget(parent), ui(new Ui::TranslatorInspectorWidget)
{
  ui->setupUi(this);
  QAbstractItemModel *translators = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TranslatorsModel"));
  ui->translatorList->setModel(translators);
  ui->translatorList->setSelectionModel(ObjectBroker::selectionModel(translators));

  m_inspector = ObjectBroker::object<TranslatorInspectorInterface *>(QStringLiteral("com.kdab.GammaRay.TranslatorInspector"));

  connect(ui->languageChangeButton, SIGNAL(clicked()), m_inspector, SLOT(sendLanguageChangeEvent()));
  connect(ui->resetTranslationsButton, SIGNAL(clicked()), m_inspector, SLOT(resetTranslations()));

  // searching for translations
  {
    auto translationsModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TranslationsModel"));
    ui->translationsView->setModel(translationsModel);
    new SearchLineController(ui->translationsSearchLine, translationsModel);
    ui->translationsView->setSelectionModel(ObjectBroker::selectionModel(translationsModel));
  }
}
TranslatorInspectorWidget::~TranslatorInspectorWidget()
{
}

static QObject* translatorInspectorClientFactory(const QString &name, QObject *parent)
{
  return new TranslatorInspectorClient(name, parent);
}

void TranslatorInspectorWidgetFactory::initUi()
{
  ObjectBroker::registerClientObjectFactoryCallback<TranslatorInspectorInterface *>(translatorInspectorClientFactory);
}
