/**************************************************************************
**
** This code is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#ifndef GAMMARAY_PROCESSLIST_H
#define GAMMARAY_PROCESSLIST_H

#include "gammaray_launcher_ui_export.h"

#include <launcher/core/probeabi.h>

#include <QString>
#include <QList>

struct ProcData
{
    qint64 ppid;
    QString name;
    QString image;
    QString state;
    QString user;
    GammaRay::ProbeABI abi;

    inline bool equals(const ProcData &other) const
    {
        return ppid == other.ppid &&
                name == other.name &&
                image == other.image &&
                state == other.state &&
                user == other.user &&
                abi == other.abi;
    }
};

typedef QList<ProcData> ProcDataList;

GAMMARAY_LAUNCHER_UI_EXPORT ProcDataList processList(const ProcDataList &previous);

#endif // PROCESSLIST_H
