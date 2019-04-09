/*
  widget3dimagetextureimage.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_WIDGET3DIMAGETEXTUREIMAGE_H
#define GAMMARAY_WIDGET3DIMAGETEXTUREIMAGE_H

#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QTextureImageDataGenerator>

#include <QImage>

namespace GammaRay
{

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
