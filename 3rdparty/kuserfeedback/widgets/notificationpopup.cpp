/*
    Copyright (C) 2017 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QGuiApplication::applicationDisplayName();
#else
    return QString();
#endif
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
    connect(d->ui->actionButton, SIGNAL(clicked()), this, SLOT(action()));
    connect(d->ui->closeButton, SIGNAL(clicked()), this, SLOT(hidePopup()));

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
    connect(provider, SIGNAL(showEncouragementMessage()), this, SLOT(showEncouragement()));
    connect(provider, SIGNAL(surveyAvailable(KUserFeedback::SurveyInfo)), this, SLOT(surveyAvailable(KUserFeedback::SurveyInfo)));
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
