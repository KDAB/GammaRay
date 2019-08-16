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

#ifndef KUSERFEEDBACK_USAGETIMESOURCE_H
#define KUSERFEEDBACK_USAGETIMESOURCE_H

#include "kuserfeedbackcore_export.h"
#include "abstractdatasource.h"

namespace KUserFeedback {

class Provider;
class ProviderPrivate;
class UsageTimeSourcePrivate;

/*! Data source reporting the total usage time of the application.
 *
 *  The default telemetry mode for this source is Provider::BasicUsageStatistics.
 */
class KUSERFEEDBACKCORE_EXPORT UsageTimeSource :  public AbstractDataSource
{
public:
    Q_DECLARE_TR_FUNCTIONS(KUserFeedback::UsageTimeSource)
public:
    /*! Create a new usage time data source. */
    UsageTimeSource();

    QString name() const override;
    QString description() const override;

    QVariant data() override;

private:
    Q_DECLARE_PRIVATE(UsageTimeSource)
    friend class Provider;
    void setProvider(ProviderPrivate *p);
};

}

#endif // KUSERFEEDBACK_USAGETIMESOURCE_H
