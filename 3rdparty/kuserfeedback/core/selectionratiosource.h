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

#ifndef KUSERFEEDBACK_SELECTIONRATIOSOURCE_H
#define KUSERFEEDBACK_SELECTIONRATIOSOURCE_H

#include "kuserfeedbackcore_export.h"
#include "abstractdatasource.h"

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
QT_END_NAMESPACE

namespace KUserFeedback {

class SelectionRatioSourcePrivate;

/*! Records the time ratio a given entry is selected via a QItemSelectionModel.
 *
 *  An example use-case would be the usage ratio of a applications
 *  views/modes selected using a model-based view sidebar (such as
 *  used in e.g. Kontact).
 *
 *  The default telemetry mode for this source is Provider::DetailedUsageStatistics.
 */
class KUSERFEEDBACKCORE_EXPORT SelectionRatioSource : public AbstractDataSource
{
public:
    /*! Create a new selection ratio data source.
     * @param selectionModel The selection to monitor.
     * @param sampleName This is the name of the database field this data source is
     * associated with.
     */
    explicit SelectionRatioSource(QItemSelectionModel *selectionModel, const QString &sampleName);

    /*! Determine which role to consider for the reported value.
     *  By default this is Qt::DisplayRole.
     */
    void setRole(int role);

    QString description() const override;
    /*! Set human-readable and translated description of the data provided by this source.
     *  @note This must be set before adding this source, sources without description are
     *  discarded.
     *  @param desc The description.
     */
    void setDescription(const QString &desc);

    QVariant data() override;
    void loadImpl(QSettings *settings) override;
    void storeImpl(QSettings *settings) override;
    void resetImpl(QSettings *settings) override;

private:
    Q_DECLARE_PRIVATE(SelectionRatioSource)
};

}

#endif // KUSERFEEDBACK_SELECTIONRATIOSOURCE_H
