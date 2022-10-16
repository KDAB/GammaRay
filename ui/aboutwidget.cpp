/*
  aboutwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "aboutwidget.h"
#include "ui_aboutwidget.h"

#include <ui/uiresources.h>

#include <QPainter>
#include <QScrollBar>

using namespace GammaRay;

AboutWidget::AboutWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AboutWidget)
{
    ui->setupUi(this);
    // Try to reduce the scrollbar width to hide it a bit...
    auto vsb = ui->textAuthors->verticalScrollBar();
#if defined(Q_OS_MAC)
    vsb->setAttribute(Qt::WA_MacSmallSize);
#else
    vsb->setFixedWidth(10);
#endif
}

AboutWidget::~AboutWidget() = default;

void AboutWidget::setLogo(const QString &iconFileName)
{
    ui->logoLabel->setPixmap(iconFileName);
}

void AboutWidget::setThemeLogo(const QString &fileName)
{
    ui->logoLabel->setThemeFileName(fileName);
}

void AboutWidget::setTitle(const QString &title)
{
    ui->titleLabel->setText(title);
}

void AboutWidget::setHeader(const QString &header)
{
    ui->textHeader->setText(header);
}

void AboutWidget::setAuthors(const QString &authors)
{
    ui->textAuthors->setHtml(authors);
}

void AboutWidget::setFooter(const QString &footer)
{
    ui->textFooter->setText(footer);
}

void AboutWidget::setText(const QString &text)
{
    setHeader(text);
    ui->textAuthors->setVisible(false);
    ui->textFooter->setVisible(false);
}

void AboutWidget::setBackgroundWindow(QWidget *window)
{
    if (m_backgroundWindow == window)
        return;

    if (m_backgroundWindow) {
        m_backgroundWindow->removeEventFilter(this);
        m_backgroundWindow->update();
    }

    m_backgroundWindow = window;
    m_watermark = QPixmap();

    if (m_backgroundWindow) {
        m_backgroundWindow->installEventFilter(this);
        m_backgroundWindow->update();
    }
}

void AboutWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    setBackgroundWindow(window());
}

bool AboutWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_backgroundWindow) {
        if (event->type() == QEvent::ScreenChangeInternal)
            m_watermark = QPixmap();
        else if (event->type() == QEvent::Paint) {
            if (m_watermark.isNull())
                m_watermark = UIResources::themedPixmap(QStringLiteral("watermark.png"), this);

            qreal dpr = 1.0;
            dpr = m_watermark.devicePixelRatio();
            QPainter p(m_backgroundWindow);
            p.drawPixmap(m_backgroundWindow->width() - (m_watermark.width() / dpr),
                         m_backgroundWindow->height() - (m_watermark.height() / dpr), m_watermark);
        }
    }

    return QWidget::eventFilter(object, event);
}
