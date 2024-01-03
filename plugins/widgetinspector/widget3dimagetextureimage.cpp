/*
  widget3dimagetextureimage.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/


#include "widget3dimagetextureimage.h"

#include <QWidget>

namespace GammaRay {

class Widget3DImageTextureDataGenerator : public Qt3DRender::QTextureImageDataGenerator
{
public:
    explicit Widget3DImageTextureDataGenerator(const QImage &image)
        : Qt3DRender::QTextureImageDataGenerator()
        , mImage(image)
    {
    }

    ~Widget3DImageTextureDataGenerator()
    {
    }

    Qt3DRender::QTextureImageDataPtr operator()() override
    {
        Qt3DRender::QTextureImageDataPtr dataPtr(new Qt3DRender::QTextureImageData);
        dataPtr->setImage(mImage);
        return dataPtr;
    }

    bool operator==(const Qt3DRender::QTextureImageDataGenerator &other) const override
    {
        const auto otherFtor = Qt3DRender::functor_cast<Widget3DImageTextureDataGenerator>(&other);
        return (otherFtor != nullptr && otherFtor->mImage == mImage);
    }

    QT3D_FUNCTOR(Widget3DImageTextureDataGenerator)

private:
    QImage mImage;
};

}

using namespace GammaRay;

Widget3DImageTextureImage::Widget3DImageTextureImage(Qt3DCore::QNode *parent)
    : Qt3DRender::QAbstractTextureImage(parent)
{
}

Widget3DImageTextureImage::~Widget3DImageTextureImage()
{
}

QImage Widget3DImageTextureImage::image() const
{
    return mImage;
}

void Widget3DImageTextureImage::setImage(const QImage &image)
{
    if (mImage != image) {
        if (image.format() == QImage::Format_RGBA8888) {
            mImage = image;
        } else {
            // Qt4 does not have RGBA8888 so the probe sends us ARGB32
            mImage = image.convertToFormat(QImage::Format_RGBA8888);
        }
        Q_EMIT imageChanged();
        notifyDataGeneratorChanged();
    }
}

Qt3DRender::QTextureImageDataGeneratorPtr Widget3DImageTextureImage::dataGenerator() const
{
    return Qt3DRender::QTextureImageDataGeneratorPtr(new Widget3DImageTextureDataGenerator(mImage));
}
