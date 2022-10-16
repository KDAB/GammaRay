/*
  textureextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TEXTUREEXTENSION_H
#define GAMMARAY_TEXTUREEXTENSION_H

#include <core/propertycontrollerextension.h>

#include <QObject>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QImage;
class QSGDistanceFieldTextMaterial;
class QSGTexture;
QT_END_NAMESPACE

namespace GammaRay {

class RemoteViewServer;

class TextureExtension : public QObject, public PropertyControllerExtension
{
    Q_OBJECT
public:
    explicit TextureExtension(PropertyController *controller);
    ~TextureExtension() override;

    bool setQObject(QObject *obj) override;
    bool setObject(void *object, const QString &typeName) override;

private slots:
    void textureGrabbed(QSGTexture *tex, const QImage &img);
    void textureGrabbed(void *data, const QImage &img);

private:
    bool ensureSetup();
    void triggerGrab();

    QPointer<QSGTexture> m_currentTexture;
    QSGDistanceFieldTextMaterial *m_currentMaterial;
    RemoteViewServer *m_remoteView;
    bool m_connected;
};

}

#endif // GAMMARAY_TEXTUREEXTENSION_H
