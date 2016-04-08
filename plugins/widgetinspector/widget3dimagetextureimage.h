#ifndef WIDGET3DTEXTURE_H
#define WIDGET3DTEXTURE_H

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
    explicit Widget3DImageTextureImage(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~Widget3DImageTextureImage();

    QImage image() const;

    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const Q_DECL_OVERRIDE;
public Q_SLOTS:
    void setImage(const QImage &image);

Q_SIGNALS:
    void imageChanged();

protected:
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;

private:
    QImage mImage;

    QT3D_CLONEABLE(Widget3DImageTextureImage)

};

}

#endif // WIDGET3DTEXTURE_H
