/*
  widget3dimagetextureimage.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "widget3dimagetextureimage.h"

#include <QWidget>

namespace GammaRay
{

class Widget3DImageTextureDataGenerator : public Qt3DRender::QTextureImageDataGenerator
{
public:
    Widget3DImageTextureDataGenerator(const QImage &image)
        : Qt3DRender::QTextureImageDataGenerator()
        , mImage(image)
    {
    }

    ~Widget3DImageTextureDataGenerator()
    {
    }

    Qt3DRender::QTexImageDataPtr operator()() Q_DECL_OVERRIDE
    {
        Qt3DRender::QTexImageDataPtr dataPtr(new Qt3DRender::QTexImageData);
        dataPtr->setImage(mImage);
        return dataPtr;
    }

    bool operator==(const Qt3DRender::QTextureImageDataGenerator &other) const Q_DECL_OVERRIDE
    {
        const auto otherFtor = functor_cast<Widget3DImageTextureDataGenerator>(&other);
        return (otherFtor != Q_NULLPTR && otherFtor->mImage == mImage);
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
    cleanup();
}

QImage Widget3DImageTextureImage::image() const
{
    return mImage;
}

void Widget3DImageTextureImage::setImage(const QImage &image)
{
    if (mImage != image) {
        mImage = image;
        Q_EMIT imageChanged();
        update();
    }
}

Qt3DRender::QTextureImageDataGeneratorPtr Widget3DImageTextureImage::dataGenerator() const
{
    return Qt3DRender::QTextureImageDataGeneratorPtr(new Widget3DImageTextureDataGenerator(mImage));
}

void Widget3DImageTextureImage::copy(const Qt3DCore::QNode *ref)
{
    QAbstractTextureImage::copy(ref);
    const auto img = static_cast<const Widget3DImageTextureImage *>(ref);
    mImage = img->mImage;
}
