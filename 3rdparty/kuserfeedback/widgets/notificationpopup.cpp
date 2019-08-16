/*
    Copyright (C) 2017 Volker Krause <vkrause@kde.org>

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "notificationpopup.h"
#include "ui_notificationpopup.h"
#include "feedbackconfigdialog.h"

#include <provider.h>
#include <surveyinfo.h>

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QStyle>

using namespace KUserFeedback;

namespace KUserFeedback {
namespace Ui
{
class NotificationPopup;
}

class NotificationPopupPrivate {
public:
    NotificationPopupPrivate(QWidget *qq);
    void showEncouragement();
    void surveyAvailable(const SurveyInfo &info);

    void showPopup();
    void hidePopup();
    void action();
    void reposition();
    int xPosition() const;

    static QString appName();

    Provider *provider;
    SurveyInfo survey;
    QPropertyAnimation *animation;
    std::unique_ptr<Ui::NotificationPopup> ui;
    QWidget *q;
};

}

NotificationPopupPrivate::NotificationPopupPrivate(QWidget *qq) :
    provider(nullptr),
    animation(nullptr),
    q(qq)
{
}

void NotificationPopupPrivate::showEncouragement()
{
    if (q->isVisible())
        return;

    survey = SurveyInfo();
    const auto name = appName();
    if (name.isEmpty()) {
        ui->title->setText(NotificationPopup::tr("Help us make this application better!"));
        ui->message->setText(NotificationPopup::tr("You can help us improving this application by sharing statistics and participate in surveys."));
    } else {
        ui->title->setText(NotificationPopup::tr("Help us make %1 better!").arg(name));
        ui->message->setText(NotificationPopup::tr("You can help us improving %1 by sharing statistics and participate in surveys.").arg(name));
    }
    ui->actionButton->setText(NotificationPopup::tr("Contribute..."));
    showPopup();
}

void NotificationPopupPrivate::surveyAvailable(const SurveyInfo &info)
{
    if (q->isVisible())
        return;

    survey = info;
    const auto name = appName();
    ui->title->setText(NotificationPopup::tr("We are looking for your feedback!"));
    if (name.isEmpty())
        ui->message->setText(NotificationPopup::tr("We would like a few minutes of your time to provide feedback about this application in a survey."));
    else
        ui->message->setText(NotificationPopup::tr("We would like a few minutes of your time to provide feedback about %1 in a survey.").arg(name));
    ui->actionButton->setText(NotificationPopup::tr("Participate"));
    showPopup();
}

void NotificationPopupPrivate::showPopup()
{
    q->show();

    q->resize(q->sizeHint());
    const auto startPos = QPoint(xPosition(), q->parentWidget()->height());
    q->move(startPos);

    if (!animation)
        animation = new QPropertyAnimation(q, "pos", q);
    animation->setStartValue(startPos);
    animation->setEndValue(QPoint(xPosition(), q->parentWidget()->height() - q->height()));
    animation->setDuration(100);
    animation->setEasingCurve(QEasingCurve::InQuad);
    animation->start();

    ui->actionButton->setFocus();
}

void NotificationPopupPrivate::hidePopup()
{
    if (animation)
        animation->stop();
    q->hide();
}

void NotificationPopupPrivate::action()
{
    if (survey.isValid()) {
        QDesktopServices::openUrl(survey.url());
        provider->surveyCompleted(survey);
    } else {
        FeedbackConfigDialog dlg(q);
        dlg.setFeedbackProvider(provider);
        dlg.exec();
    }

    hidePopup();
}

void NotificationPopupPrivate::reposition()
{
    const auto pos = QPoint(xPosition(), q->parentWidget()->height() - q->height());
    if (animation->state() == QAbstractAnimation::Running)
        animation->setEndValue(pos);
    else
        q->move(pos);
}

int NotificationPopupPrivate::xPosition() const
{
    if (QApplication::layoutDirection() == Qt::LeftToRight) {
        return q->parentWidget()->width() - q->width();
    }
    return 0;
}

QString NotificationPopupPrivate::appName()
{
    return QGuiApplication::applicationDisplayName();
}


NotificationPopup::NotificationPopup(QWidget* parent)
    : QWidget(parent)
    , d(new NotificationPopupPrivate(this))
{
    Q_ASSERT(parent);

    d->ui.reset(new Ui::NotificationPopup);
    d->ui->setupUi(this);

    d->ui->frame->setAutoFillBackground(true);
    d->ui->closeButton->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    connect(d->ui->actionButton, &QPushButton::clicked, this, [this]() { d->action(); });
    connect(d->ui->closeButton, &QPushButton::clicked, this, [this]() { d->hidePopup(); });

    parent->installEventFilter(this);
    setVisible(false);
}

NotificationPopup::~NotificationPopup()
{
}

void NotificationPopup::setFeedbackProvider(Provider* provider)
{
    Q_ASSERT(provider);
    d->provider = provider;
    connect(provider, &Provider::showEncouragementMessage, this, [this]() { d->showEncouragement(); });
    connect(provider, &Provider::surveyAvailable, this, [this](const SurveyInfo &info) { d->surveyAvailable(info); });
}

void NotificationPopup::keyReleaseEvent(QKeyEvent* event)
{
    if (isVisible() && event->key() == Qt::Key_Escape)
        d->hidePopup();
}

bool NotificationPopup::eventFilter(QObject* receiver, QEvent* event)
{
    if (receiver == parentWidget() && isVisible()) {
        d->reposition();
    }
    return QWidget::eventFilter(receiver, event);
}

#include "moc_notificationpopup.cpp"
