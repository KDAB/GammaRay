/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

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
    Q_PRIVATE_SLOT(d, void updateButtonState())
    std::unique_ptr<FeedbackConfigDialogPrivate> d;
};

}

#endif // KUSERFEEDBACK_CONSOLE_FEEDBACKCONFIGDIALOG_H
