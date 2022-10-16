/*
  mimetypes.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_MIMETYPES_MIMETYPES_H
#define GAMMARAY_MIMETYPES_MIMETYPES_H

#include "core/toolfactory.h"

QT_BEGIN_NAMESPACE
class QStandardItemModel;
QT_END_NAMESPACE

namespace GammaRay {
class MimeTypes : public QObject
{
    Q_OBJECT
public:
    explicit MimeTypes(Probe *probe, QObject *parent = nullptr);
    ~MimeTypes() override;
};

class MimeTypesFactory : public QObject, public StandardToolFactory<QObject, MimeTypes>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_mimetypes.json")
public:
    explicit MimeTypesFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_MIMETYPES_H
