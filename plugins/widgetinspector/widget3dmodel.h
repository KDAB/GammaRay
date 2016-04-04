#ifndef WIDGET3DMODEL_H
#define WIDGET3DMODEL_H

#include <QSortFilterProxyModel>
#include <QRect>
#include <QWidget>
#include <QMap>

namespace GammaRay {

class ProbeInterface;

class Widget3DWidget : public QObject
{
    Q_OBJECT

public:
    Widget3DWidget(QObject *parent = Q_NULLPTR); // don't use
    Widget3DWidget(QWidget *qWidget, int level, Widget3DWidget *parent);
    ~Widget3DWidget();

    inline QImage texture() const { return mTextureImage; }
    inline QImage backTexture() const { return mBackTextureImage; }
    inline int level() const { return mLevel; }
    inline QRect geometry() const { return mGeometry; }
    inline QWidget *qWidget() const { return mQWidget; }
    inline Widget3DWidget *parentWidget() const { return static_cast<Widget3DWidget*>(parent()); }
    inline bool isVisible() const { return mQWidget->isVisible(); }

protected:
    bool eventFilter(QObject *obj, QEvent *ev) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void textureChanged();
    void geometryChanged();
    void visibleChanged();

private Q_SLOTS:
    void updateTexture();
    void updateGeometry();

private:
    QWidget *mQWidget;
    QImage mTextureImage;
    QImage mBackTextureImage;
    QRect mTextureGeometry;
    QRect mGeometry;
    int mLevel;
    bool mIsPainting;
    QTimer *mPaintTimer;
};

class Widget3DModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    enum Roles {
        TextureRole = Qt::UserRole + 1,
        BackTextureRole,
        GeometryRole,
        LevelRole
    };

    explicit Widget3DModel(QObject *parent = Q_NULLPTR);
    ~Widget3DModel();

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

    QMap<int, QVariant> itemData(const QModelIndex &index) const Q_DECL_OVERRIDE;
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const Q_DECL_OVERRIDE;

private Q_SLOTS:
    void widgetGeometryChanged();
    void widgetTextureChanged();

private:
    Widget3DWidget *widgetForObject(QObject *object) const;
    Widget3DWidget *widgetForIndex(const QModelIndex &idx) const;

    // mutable becasue we populate it lazily from data() const
    mutable QHash<QObject *, Widget3DWidget*> mDataCache;
};

}

Q_DECLARE_METATYPE(GammaRay::Widget3DWidget*)

#endif // WIDGET3DMODEL_H
