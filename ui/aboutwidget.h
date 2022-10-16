/*
  aboutwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_ABOUTWIDGET_H
#define GAMMARAY_ABOUTWIDGET_H

#include "gammaray_ui_export.h"

#include <QWidget>
#include <QPointer>

namespace GammaRay {
namespace Ui {
class AboutWidget;
}

class GAMMARAY_UI_EXPORT AboutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AboutWidget(QWidget *parent = nullptr);
    ~AboutWidget() override;

    void setLogo(const QString &iconFileName);
    void setThemeLogo(const QString &fileName);
    void setTitle(const QString &title);
    void setHeader(const QString &header);
    void setAuthors(const QString &authors);
    void setFooter(const QString &footer);

    void setText(const QString &text);

    void setBackgroundWindow(QWidget *window);

protected:
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QScopedPointer<Ui::AboutWidget> ui;
    QPointer<QWidget> m_backgroundWindow;
    QPixmap m_watermark;
};
}

#endif // GAMMARAY_ABOUTWIDGET_H
