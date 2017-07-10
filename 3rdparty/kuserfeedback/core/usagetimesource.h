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
    QString description() const override;
    QVariant data() override;

private:
    Q_DECLARE_PRIVATE(UsageTimeSource)
    friend class Provider;
    void setProvider(ProviderPrivate *p);
};

}

#endif // KUSERFEEDBACK_USAGETIMESOURCE_H
