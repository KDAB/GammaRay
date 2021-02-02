/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KUSERFEEDBACK_PLATFORMINFOSOURCE_H
#define KUSERFEEDBACK_PLATFORMINFOSOURCE_H

#include "abstractdatasource.h"

namespace KUserFeedback {

/*! Reports information about the platform the application is running on.
 *  This includes two string data fields:
 *  - platform.os: the operating system name
 *  - platform.version: the operating system or distribution version.
 *
 *  The default telemetry mode for this source is Provider::BasicSystemInformation.
 */
class KUSERFEEDBACKCORE_EXPORT PlatformInfoSource : public AbstractDataSource
{
    Q_DECLARE_TR_FUNCTIONS(KUserFeedback::PlatformInfoSource)
public:
    /*! Create a new platform information source. */
    PlatformInfoSource();

    QString name() const override;
    QString description() const override;

    QVariant data() override;
};

}

#endif // KUSERFEEDBACK_PLATFORMINFOSOURCE_H
