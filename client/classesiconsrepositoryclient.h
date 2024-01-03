/*
  classesiconsrepositoryclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
