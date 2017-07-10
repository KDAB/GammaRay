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

#ifndef KUSERFEEDBACK_NOTIFICATIONPOPUP_H
#define KUSERFEEDBACK_NOTIFICATIONPOPUP_H

#include "kuserfeedbackwidgets_export.h"

#include <QWidget>

#include <memory>

namespace KUserFeedback {

class NotificationPopupPrivate;
class Provider;

/*!
 * Notification popup that overlays a small part of the application for
 * encouraging contributions or inform about surveys.
 */
class KUSERFEEDBACKWIDGETS_EXPORT NotificationPopup : public QWidget
{
    Q_OBJECT
public:
    /*!
     * Create a new notification popup.
     * Do not put this widget into a layout.
     * @param parent The parent widget. This must not be @c nullptr.
     */
    explicit NotificationPopup(QWidget *parent);
    ~NotificationPopup();

    /*!
     * Set the feedback provider that is going to drive this notification popup.
     */
    void setFeedbackProvider(Provider *provider);

protected:
    ///@cond internal
    void keyReleaseEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *receiver, QEvent *event) override;
    ///@endcond

private:
    Q_PRIVATE_SLOT(d, void showEncouragement())
    Q_PRIVATE_SLOT(d, void surveyAvailable(const KUserFeedback::SurveyInfo &info))
    Q_PRIVATE_SLOT(d, void hidePopup())
    Q_PRIVATE_SLOT(d, void action())
    std::unique_ptr<NotificationPopupPrivate> d;
};

}

#endif // KUSERFEEDBACK_NOTIFICATIONPOPUP_H
