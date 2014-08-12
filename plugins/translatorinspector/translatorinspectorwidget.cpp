/*
  translatorinspectorwidget.cpp

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

#include "translatorinspectorwidget.h"
#include "ui_translatorinspectorwidget.h"

#include <QSortFilterProxyModel>

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
  QAbstractItemModel *translators = ObjectBroker::model("com.kdab.GammaRay.TranslatorsModel");
  ui->translatorList->setModel(translators);
  ui->translatorList->setSelectionModel(ObjectBroker::selectionModel(translators));

  m_inspector = ObjectBroker::object<TranslatorInspectorInterface *>("com.kdab.GammaRay.TranslatorInspector");

  connect(ui->languageChangeButton, SIGNAL(clicked()), m_inspector, SLOT(sendLanguageChangeEvent()));
  connect(ui->resetTranslationsButton, SIGNAL(clicked()), m_inspector, SLOT(resetTranslations()));

  // searching for translations
  {
    QSortFilterProxyModel *translationsFilter = new QSortFilterProxyModel(this);
    translationsFilter->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.TranslationsModel"));
    ui->translationsView->setModel(translationsFilter);
    ui->translationsSearchLine->setProxy(translationsFilter);
    ui->translationsView->setSelectionModel(ObjectBroker::selectionModel(translationsFilter));
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
