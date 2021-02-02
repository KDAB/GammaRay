/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
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
