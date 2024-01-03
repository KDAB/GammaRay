/*
  kjobtracker.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_KJOBTRACKER_KJOBTRACKER_H
#define GAMMARAY_KJOBTRACKER_KJOBTRACKER_H

#include <core/toolfactory.h>

#include <KJob>

class KJob;
namespace GammaRay {
class KJobModel;

class KJobTracker : public QObject
{
    Q_OBJECT
public:
    explicit KJobTracker(Probe *probe, QObject *parent = nullptr);
    ~KJobTracker() override;

private:
    KJobModel *m_jobModel;
};

class KJobTrackerFactory : public QObject, public StandardToolFactory<KJob, KJobTracker>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_kjobtracker.json")

public:
    explicit KJobTrackerFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_KJOBTRACKER_H
