/*
  translatorinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
