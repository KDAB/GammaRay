/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KUSERFEEDBACK_APPLICATIONVERSIONSOURCE_H
#define KUSERFEEDBACK_APPLICATIONVERSIONSOURCE_H

#include "kuserfeedbackcore_export.h"
#include "abstractdatasource.h"

namespace KUserFeedback {

/*! Data source for the application version.
 *  The application version is retrieved via QCoreApplication::applicationVersion.
 *
 *  The default telemetry mode for this source is Provider::BasicSystemInformation.
 */
class KUSERFEEDBACKCORE_EXPORT ApplicationVersionSource : public AbstractDataSource
{
    Q_DECLARE_TR_FUNCTIONS(KUserFeedback::ApplicationVersionSource)
public:
    /*! Create a new application version source. */
    ApplicationVersionSource();

    QString name() const override;
    QString description() const override;

    QVariant data() override;
};

}

#endif // KUSERFEEDBACK_APPLICATIONVERSIONSOURCE_H
