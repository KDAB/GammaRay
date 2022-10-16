/*
  classesiconsrepositoryclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CLASSESICONSREPOSITORYCLIENT_H
#define GAMMARAY_CLASSESICONSREPOSITORYCLIENT_H

#include <common/classesiconsrepository.h>

namespace GammaRay {

class ClassesIconsRepositoryClient : public ClassesIconsRepository
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ClassesIconsRepository)

public:
    explicit ClassesIconsRepositoryClient(QObject *parent = nullptr);
    ~ClassesIconsRepositoryClient() override;

    QString filePath(int id) const override;

protected slots:
    void indexReceived(const QVector<QString> &index);
    void requestIndex() override;

private:
    bool m_ready = false;
};

}

#endif // GAMMARAY_CLASSESICONSREPOSITORYCLIENT_H
