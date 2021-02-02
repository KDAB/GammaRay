/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KUSERFEEDBACK_QTVERSIONSOURCE_H
#define KUSERFEEDBACK_QTVERSIONSOURCE_H

#include "kuserfeedbackcore_export.h"
#include "abstractdatasource.h"

namespace KUserFeedback {

/*! Data source reporting the Qt version used at runtime.
 *
 *  The default telemetry mode for this source is Provider::BasicSystemInformation.
 */
class KUSERFEEDBACKCORE_EXPORT QtVersionSource : public AbstractDataSource
{
    Q_DECLARE_TR_FUNCTIONS(KUserFeedback::QtVersionSource)
public:
    QtVersionSource();

    QString name() const override;
    QString description() const override;

    QVariant data() override;
};
}

#endif // KUSERFEEDBACK_QTVERSIONSOURCE_H
