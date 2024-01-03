/*
  mimetypeswidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_MIMETYPESWIDGET_H
#define GAMMARAY_MIMETYPESWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>

#include <QWidget>

namespace GammaRay {
namespace Ui {
class MimeTypesWidget;
}

class MimeTypesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MimeTypesWidget(QWidget *parent = nullptr);
    ~MimeTypesWidget() override;

private:
    QScopedPointer<Ui::MimeTypesWidget> ui;
    UIStateManager m_stateManager;
};

class MimeTypesWidgetFactory : public QObject, public StandardToolUiFactory<MimeTypesWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_mimetypes.json")
};

}

#endif // GAMMARAY_MIMETYPESWIDGET_H
