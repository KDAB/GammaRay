#include "widget3dimagetextureimage.h"

#include <QWidget>

namespace GammaRay
{

class Widget3DImageTextureImageFunctor : public Qt3DRender::QTextureDataFunctor
{
public:
    Widget3DImageTextureImageFunctor(const QImage &image)
        : Qt3DRender::QTextureDataFunctor()
        , mImage(image)
    {
    }

    ~Widget3DImageTextureImageFunctor()
    {
    }

    Qt3DRender::QTexImageDataPtr operator()() Q_DECL_OVERRIDE
    {
        Qt3DRender::QTexImageDataPtr dataPtr(new Qt3DRender::QTexImageData);
        dataPtr->setImage(mImage);
        return dataPtr;
    }

    bool operator==(const Qt3DRender::QTextureDataFunctor &other) const Q_DECL_OVERRIDE
    {
        const auto otherFtor = functor_cast<Widget3DImageTextureImageFunctor>(&other);
        return (otherFtor != Q_NULLPTR && otherFtor->mImage == mImage);
    }

    QT3D_FUNCTOR(Widget3DImageTextureImageFunctor)

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
        mImage = image;
        Q_EMIT imageChanged();
        update();
    }
}

Qt3DRender::QTextureDataFunctorPtr Widget3DImageTextureImage::dataFunctor() const
{
    return Qt3DRender::QTextureDataFunctorPtr(new Widget3DImageTextureImageFunctor(mImage));
}

void Widget3DImageTextureImage::copy(const Qt3DCore::QNode *ref)
{
    QAbstractTextureImage::copy(ref);
    const auto img = static_cast<const Widget3DImageTextureImage *>(ref);
    mImage = img->mImage;
}
