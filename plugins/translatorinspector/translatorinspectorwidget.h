/*
  translatorinspectorwidget.h

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

#ifndef TRANSLATORINSPECTORWIDGET_H
#define TRANSLATORINSPECTORWIDGET_H

#include <ui/tooluifactory.h>
#include <QWidget>

#include "translatorinspectorinterface.h"

namespace GammaRay {

namespace Ui
{
class TranslatorInspectorWidget;
}

class TranslatorInspectorClient : public TranslatorInspectorInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::TranslatorInspectorInterface)

  public:
    explicit TranslatorInspectorClient(const QString &name, QObject *parent = 0);

  public slots:
    void sendLanguageChangeEvent() Q_DECL_OVERRIDE;
    void resetTranslations() Q_DECL_OVERRIDE;
};

class TranslatorInspectorWidget : public QWidget
{
  Q_OBJECT

  public:
    explicit TranslatorInspectorWidget(QWidget *parent);
    ~TranslatorInspectorWidget();

  private:
    QScopedPointer<Ui::TranslatorInspectorWidget> ui;
    TranslatorInspectorInterface *m_inspector;
};

class TranslatorInspectorWidgetFactory
    : public QObject,
      public StandardToolUiFactory<TranslatorInspectorWidget>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolUiFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.TranslatorInspectorUi")

  public:
    void initUi() Q_DECL_OVERRIDE;
};

}

#endif
