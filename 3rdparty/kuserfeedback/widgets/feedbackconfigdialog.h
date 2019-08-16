/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

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

#ifndef KUSERFEEDBACK_CONSOLE_FEEDBACKCONFIGDIALOG_H
#define KUSERFEEDBACK_CONSOLE_FEEDBACKCONFIGDIALOG_H

#include "kuserfeedbackwidgets_export.h"
#include <QDialog>

#include <memory>

namespace KUserFeedback {

class FeedbackConfigDialogPrivate;
class Provider;

/*! Configure which feedback a user wants to provide.
 *
 * @see FeedbackConfigWidget
 */
class KUSERFEEDBACKWIDGETS_EXPORT FeedbackConfigDialog : public QDialog
{
    Q_OBJECT
public:
    /*! Create a new feedback configuration dialog.
     *  @param parent The parent widget.
     */
    explicit FeedbackConfigDialog(QWidget *parent = nullptr);
    ~FeedbackConfigDialog();

    /*! Set the feedback provider that this dialog configures. */
    void setFeedbackProvider(KUserFeedback::Provider *provider);

    /*! Accepts the dialog and write changes made by the user to
     *  the feedback provider.
     */
    void accept() override;

private:
    std::unique_ptr<FeedbackConfigDialogPrivate> d;
};

}

#endif // KUSERFEEDBACK_CONSOLE_FEEDBACKCONFIGDIALOG_H
