/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef ABSTRACTDATASOURCEPRIVATE_H
#define ABSTRACTDATASOURCEPRIVATE_H

#include "provider.h"

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace KUserFeedback {

class AbstractDataSourcePrivate
{
public:
    AbstractDataSourcePrivate();
    virtual ~AbstractDataSourcePrivate();

    QString id;
    Provider::TelemetryMode mode;
    bool active;

    void storeCommonSettings(QSettings *settings);
    void loadCommonSettings(QSettings *settings);
    void resetCommonSettings(QSettings *settings);

private:
    Q_DISABLE_COPY(AbstractDataSourcePrivate)
};

}

#endif // ABSTRACTDATASOURCEPRIVATE_H
