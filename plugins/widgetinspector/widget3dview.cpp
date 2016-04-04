#include "widget3dview.h"
#include "widget3dmodel.h"
#include "widget3dimagetextureimage.h"

#include <common/objectbroker.h>
#include <common/objectmodel.h>

#include <QWindow>
#include <QVBoxLayout>
#include <QVariant>
#include <QUrl>
#include <QWheelEvent>
#include <QProgressBar>

#include <Qt3DQuick/QQmlAspectEngine>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DInput/QInputAspect>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DLogic/QLogicAspect>

#include <QtQml>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

namespace GammaRay
{

class Widget3DWindow : public QWindow
{
   Q_OBJECT
public:
    explicit Widget3DWindow(QScreen *parent = Q_NULLPTR)
        : QWindow(parent)
    {
        setSurfaceType(QSurface::OpenGLSurface);
        resize(800, 600);
        QSurfaceFormat format;
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setDepthBufferSize(32);
        format.setSamples(4);
        format.setStencilBufferSize(8);
        setFormat(format);
        create();
    }

    ~Widget3DWindow()
    {
    }

Q_SIGNALS:
    void wheel(float delta);

protected:
    void wheelEvent(QWheelEvent *ev)
    {
        Q_EMIT wheel(ev->delta());
    }
};

class Widget3DModelData : public QObject
{
    Q_OBJECT
public:
    explicit Widget3DModelData(QAbstractItemModel *model, QObject *parent = Q_NULLPTR)
        : QObject(parent)
        , mModel(model)
    {
        connect(mModel, &QAbstractItemModel::dataChanged,
                this, &Widget3DModelData::onDataChanged);
    }

    ~Widget3DModelData()
    {
    }

    Q_INVOKABLE QVariant get(int row, const QString &role) const
    {
        const QModelIndex idx = mModel->index(row, 0);
        if (!idx.isValid()) {
            return QVariant();
        }
        QVariant data;
        if (role == QLatin1String("level")) {
            data = mModel->data(idx, Widget3DModel::LevelRole);
        } else if (role == QLatin1String("geometry")) {
            data = mModel->data(idx, Widget3DModel::GeometryRole);
        } else if (role == QLatin1String("frontTexture")) {
            data = mModel->data(idx, Widget3DModel::TextureRole);
        } else if (role == QLatin1String("backTexture")) {
            data = mModel->data(idx, Widget3DModel::BackTextureRole);
        }
        return data;
    }

Q_SIGNALS:
    void dataChanged(int row, const QString &role);

private Q_SLOTS:
    void onDataChanged(const QModelIndex &tl, const QModelIndex &br, const QVector<int> &roles)
    {
        if (roles.contains(Widget3DModel::GeometryRole)) {
            Q_EMIT dataChanged(tl.row(), QStringLiteral("geometry"));
        }
        if (roles.contains(Widget3DModel::TextureRole)) {
            Q_EMIT dataChanged(tl.row(), QStringLiteral("frontTexture"));
        }
        if (roles.contains(Widget3DModel::BackTextureRole)) {
            Q_EMIT dataChanged(tl.row(), QStringLiteral("backTexture"));
        }
    }

private:
    QAbstractItemModel *mModel;
};

}

Q_DECLARE_METATYPE(GammaRay::Widget3DWindow*)

using namespace GammaRay;

Widget3DView::Widget3DView(QWidget* parent)
    : QWidget(parent)
{
    mProgressWidget = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(mProgressWidget);
    l->addStretch(1);
    mProgress = new QProgressBar;
    l->addWidget(mProgress);
    l->addStretch(1);


    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(mProgressWidget);

    // Get the model, if it is empty, it has probably not been initialized yet
    mModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.Widget3DModel"));
    if (mModel->rowCount() > 0) {
        rowCountChanged();
    } else {
        // Wait for some rows
        connect(mModel, &QAbstractItemModel::rowsInserted,
                this, &Widget3DView::rowCountChanged);
    }
}

Widget3DView::~Widget3DView()
{
}

void Widget3DView::rowCountChanged()
{
    disconnect(mModel, &QAbstractItemModel::rowsInserted,
               this, &Widget3DView::rowCountChanged);

    qDebug() << mModel->rowCount() << "objects";
    mProgress->setMaximum(mModel->rowCount() - 1);
    // datachanged = Loading -> actual data
    connect(mModel, &QAbstractItemModel::dataChanged,
            mProgressWidget, [this](const QModelIndex &, const QModelIndex &, const QVector<int> &) {
                mProgress->setValue(mProgress->value() + 1);
                if (mProgress->value() == mProgress->maximum()) {
                    QTimer::singleShot(0, this, &Widget3DView::delayedInit);
                }
            });
    // Force-populate the model
    for (int i = 0; i < mModel->rowCount(); ++i) {
        mModel->itemData(mModel->index(i, 0));
    }
}

void Widget3DView::delayedInit()
{
    layout()->removeWidget(mProgressWidget);
    mProgressWidget->deleteLater();
    mProgressWidget = Q_NULLPTR;

    mWindow = new Widget3DWindow();
    QWidget *w = QWidget::createWindowContainer(mWindow, this);
    layout()->addWidget(w);

    qmlRegisterType<Widget3DImageTextureImage>("com.kdab.GammaRay", 1, 0, "Widget3DImageTextureImage");

    mEngine = new Qt3DCore::Quick::QQmlAspectEngine(this);
    mEngine->aspectEngine()->registerAspect(new Qt3DRender::QRenderAspect);
    mEngine->aspectEngine()->registerAspect(new Qt3DInput::QInputAspect);
    mEngine->aspectEngine()->registerAspect(new Qt3DLogic::QLogicAspect);

    QVariantMap data;
    data[QStringLiteral("surface")] = QVariant::fromValue(static_cast<QSurface*>(mWindow));
    data[QStringLiteral("eventSource")] = QVariant::fromValue(mWindow);
    mEngine->aspectEngine()->setData(data);

    mEngine->qmlEngine()->rootContext()->setContextProperty(QStringLiteral("_window"), mWindow);
    mEngine->qmlEngine()->rootContext()->setContextProperty(QStringLiteral("_widgetModel"), mModel);
    auto modelData = new Widget3DModelData(mModel, this);
    mEngine->qmlEngine()->rootContext()->setContextProperty(QStringLiteral("_modelData"), modelData);
    mEngine->setSource(QUrl(QStringLiteral("qrc:/assets/qml/main.qml")));
}

#include "widget3dview.moc"

