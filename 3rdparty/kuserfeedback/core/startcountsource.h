/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KUSERFEEDBACK_STARTCOUNTSOURCE_H
#define KUSERFEEDBACK_STARTCOUNTSOURCE_H

#include "kuserfeedbackcore_export.h"
#include "abstractdatasource.h"

namespace KUserFeedback {

class Provider;
class ProviderPrivate;
class StartCountSourcePrivate;

/*! Data source reporting the total amount of applications starts.
 *
 *  The default telemetry mode for this source is Provider::BasicUsageStatistics.
 */
class KUSERFEEDBACKCORE_EXPORT StartCountSource :  public AbstractDataSource
{
    Q_DECLARE_TR_FUNCTIONS(KUserFeedback::StartCountSource)
public:
    /*! Create a new start count data source. */
    StartCountSource();

    QString name() const override;
    QString description() const override;

    QVariant data() override;

private:
    Q_DECLARE_PRIVATE(StartCountSource)
    friend class Provider;
    void setProvider(ProviderPrivate *p);
};

}

#endif // KUSERFEEDBACK_STARTCOUNTSOURCE_H
