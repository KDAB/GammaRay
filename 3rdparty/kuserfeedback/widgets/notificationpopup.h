/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
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
    std::unique_ptr<NotificationPopupPrivate> d;
};

}

#endif // KUSERFEEDBACK_NOTIFICATIONPOPUP_H
