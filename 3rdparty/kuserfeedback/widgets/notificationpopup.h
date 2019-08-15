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
