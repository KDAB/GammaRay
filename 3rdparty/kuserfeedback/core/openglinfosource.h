/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KUSERFEEDBACK_OPENGLINFOSOURCE_H
#define KUSERFEEDBACK_OPENGLINFOSOURCE_H

#include "abstractdatasource.h"

namespace KUserFeedback {

/*! Data source reporting type, version and vendor of the OpenGL stack.
 *
 *  The default telemetry mode for this source is Provider::DetailedSystemInformation.
 */
class KUSERFEEDBACKCORE_EXPORT OpenGLInfoSource : public AbstractDataSource
{
public:
    Q_DECLARE_TR_FUNCTIONS(KUserFeedback::OpenGLInfoSource)
public:
    OpenGLInfoSource();

    QString name() const override;
    QString description() const override;

    QVariant data() override;
};

}

#endif // KUSERFEEDBACK_OPENGLINFOSOURCE_H
