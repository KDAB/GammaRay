/*
  widget3dimagetextureimage.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_WIDGET3DIMAGETEXTUREIMAGE_H
#define GAMMARAY_WIDGET3DIMAGETEXTUREIMAGE_H

#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QTextureImageDataGenerator>

#include <QImage>

namespace GammaRay {

class Widget3DTextureDataFunctor;
class Widget3DImageTextureImage : public Qt3DRender::QAbstractTextureImage
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
public:
    explicit Widget3DImageTextureImage(Qt3DCore::QNode *parent = nullptr);
    ~Widget3DImageTextureImage();

    QImage image() const;

    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override;
public Q_SLOTS:
    void setImage(const QImage &image);

Q_SIGNALS:
    void imageChanged();

private:
    QImage mImage;
};

}

#endif // GAMMARAY_WIDGET3DIMAGETEXTUREIMAGE_H
