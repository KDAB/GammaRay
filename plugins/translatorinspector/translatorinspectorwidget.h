/*
  translatorinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef TRANSLATORINSPECTORWIDGET_H
#define TRANSLATORINSPECTORWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>
#include <QWidget>

#include "translatorinspectorinterface.h"

namespace GammaRay {
namespace Ui {
class TranslatorInspectorWidget;
}

class TranslatorInspectorClient : public TranslatorInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::TranslatorInspectorInterface)

public:
    explicit TranslatorInspectorClient(const QString &name, QObject *parent = nullptr);

public slots:
    void sendLanguageChangeEvent() override;
    void resetTranslations() override;
};

class TranslatorInspectorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TranslatorInspectorWidget(QWidget *parent);
    ~TranslatorInspectorWidget() override;

private:
    void translatorContextMenu(QPoint pos);
    void translationsContextMenu(QPoint pos);
    void updateActions();

    QScopedPointer<Ui::TranslatorInspectorWidget> ui;
    UIStateManager m_stateManager;
    TranslatorInspectorInterface *m_inspector;
};

class TranslatorInspectorWidgetFactory : public QObject,
                                         public StandardToolUiFactory<TranslatorInspectorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_translatorinspector.json")

public:
    void initUi() override;
};
}

#endif
