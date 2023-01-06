/*
  classesiconsrepositoryserver.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CLASSESICONSREPOSITORYSERVER_H
#define GAMMARAY_CLASSESICONSREPOSITORYSERVER_H

#include <common/classesiconsrepository.h>

namespace GammaRay {

/*! Probe-side classes icons index management.
 * Use this to obtain a class icon file path that can be safely used in the client.
 */
class ClassesIconsRepositoryServer : public ClassesIconsRepository
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ClassesIconsRepository)

public:
    ~ClassesIconsRepositoryServer() override;

    //! @cond internal
    static void create(QObject *parent);
    //! @endcond

protected slots:
    void requestIndex() override;

private:
    explicit ClassesIconsRepositoryServer(QObject *parent = nullptr);

    static ClassesIconsRepositoryServer *s_instance;
};

}

#endif // GAMMARAY_CLASSESICONSREPOSITORYSERVER_H
