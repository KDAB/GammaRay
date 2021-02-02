/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KUSERFEEDBACK_COMPILERINFOSOURCE_H
#define KUSERFEEDBACK_COMPILERINFOSOURCE_H

#include "abstractdatasource.h"

namespace KUserFeedback {

/*! Data source reporting which compiler was used to build this code.
 *  @note This will report which compiler was used to build the feedback
 *  library, which technically does not need to be the same as used for
 *  the application code. This is particularly true for compilers that
 *  don't break ABI regularly (such as Clang and GCC), so this information
 *  is most reliable when this is not the case, e.g. with MSVC.
 *
 *  The default telemetry mode for this source is Provider::BasicSystemInformation.
 */
class KUSERFEEDBACKCORE_EXPORT CompilerInfoSource : public AbstractDataSource
{
public:
    Q_DECLARE_TR_FUNCTIONS(KUserFeedback::CompilerInfoSource)
public:
    CompilerInfoSource();

    QString name() const override;
    QString description() const override;

    QVariant data() override;
};

}

#endif // KUSERFEEDBACK_COMPILERINFOSOURCE_H
