/*
  probecreator.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROBECREATOR_H
#define GAMMARAY_PROBECREATOR_H

#include <QObject>

namespace GammaRay {
/**
 * Creates Probe instance in main thread and deletes self afterwards.
 */
class ProbeCreator : public QObject
{
    Q_OBJECT
public:
    enum CreateFlag
    {
        Create = 0,
        FindExistingObjects = 1,
        ResendServerAddress = 2
    };
    Q_DECLARE_FLAGS(CreateFlags, CreateFlag)
    explicit ProbeCreator(CreateFlags flags);

private slots:
    void createProbe();

private:
    CreateFlags m_flags;
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(GammaRay::ProbeCreator::CreateFlags)

#endif // GAMMARAY_PROBECREATOR_H
