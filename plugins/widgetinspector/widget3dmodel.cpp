#include "widget3dmodel.h"

#include <QDebug>
#include <QEvent>
#include <QTimer>

#include <common/objectmodel.h>
#include <core/objectlistmodel.h>

using namespace GammaRay;

Widget3DWidget::Widget3DWidget(QObject *parent)
    : QObject(parent)
    , mQWidget(Q_NULLPTR)
    , mLevel(0)
    , mIsPainting(false)
    , mPaintTimer(Q_NULLPTR)
{
}

Widget3DWidget::Widget3DWidget(QWidget *qWidget, int level, Widget3DWidget *parent)
    : QObject(parent)
    , mQWidget(qWidget)
    , mLevel(level)
    , mIsPainting(false)
    , mPaintTimer(Q_NULLPTR)
{
    mPaintTimer = new QTimer(this);
    mPaintTimer->setSingleShot(true);
    mPaintTimer->setInterval(200);
    connect(mPaintTimer, &QTimer::timeout,
            this, &Widget3DWidget::updateTexture);

    updateGeometry();

    mQWidget->installEventFilter(this);
}

Widget3DWidget::~Widget3DWidget()
{
}

bool Widget3DWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == mQWidget) {
        switch (ev->type()) {
        case QEvent::Resize:
            QMetaObject::invokeMethod(this, "updateGeometry", Qt::QueuedConnection);
            return false;
        case QEvent::Paint:
            if (!mIsPainting && !mPaintTimer->isActive()) {
                mPaintTimer->start();
            }
            return false;
        case QEvent::Show:
        case QEvent::Hide:
            //Q_EMIT visibleChanged();
            return false;
        default:
            return false;
        }
    }

    return false;
}

void Widget3DWidget::updateGeometry()
{
    QWidget *w = mQWidget;
    QPoint mappedPos(0, 0);
    // TODO: Use mapTo(), but it behaved somewhat weird...
    while (w->parentWidget()) {
        mappedPos += w->geometry().topLeft();
        w = w->parentWidget();
    }

    mTextureGeometry = QRect(0, 0, mQWidget->width(), mQWidget->height());
    mGeometry = QRect(mappedPos, QSize(mQWidget->width(), mQWidget->height()));
    QRect parentGeom;
    if (parent()) {
        parentGeom = parentWidget()->geometry();
        if (mGeometry.x() < parentGeom.x()) {
            mTextureGeometry.setRight(parentGeom.x() - mGeometry.x());
            mGeometry.setRight(parentGeom.x());
        }
        if (mGeometry.y() < parentGeom.y()) {
            mTextureGeometry.setTop(parentGeom.y() - mGeometry.y());
            mGeometry.setTop(parentGeom.y());
        }
        if (mGeometry.x() + mGeometry.width() > parentGeom.x() + parentGeom.width()) {
            mGeometry.setRight(parentGeom.x() + parentGeom.width());
            mTextureGeometry.setWidth(mGeometry.width());
        }
        if (mGeometry.y() + mGeometry.height() > parentGeom.y() + parentGeom.height()) {
            mGeometry.setBottom(parentGeom.y() + parentGeom.height());
            mTextureGeometry.setHeight(mGeometry.height());
        }
    }

    Q_EMIT geometryChanged();

    updateTexture();
}

void Widget3DWidget::updateTexture()
{
    mIsPainting = true;

    mTextureImage = QImage(mTextureGeometry.size(), QImage::Format_RGBA8888);
    mQWidget->render(&mTextureImage, QPoint(0, 0), QRegion(mTextureGeometry), QWidget::DrawWindowBackground);

    if (!parent()) { // tlw
        mBackTextureImage = QImage(mTextureGeometry.size(), QImage::Format_RGBA8888);
        mQWidget->render(&mBackTextureImage, QPoint(0, 0), QRegion(mTextureGeometry));
    } else {
        mBackTextureImage = mTextureImage;
    }

    mIsPainting = false;

    Q_EMIT textureChanged();
}



Widget3DModel::Widget3DModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

Widget3DModel::~Widget3DModel()
{
}

QVariant Widget3DModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case TextureRole: {
        auto w = widgetForIndex(index);
        return w ? QVariant::fromValue(w->texture()) : QVariant();
    }
    case BackTextureRole: {
        auto w = widgetForIndex(index);
        return w ? QVariant::fromValue(w->backTexture()) : QVariant();
    }
    case GeometryRole: {
        auto w = widgetForIndex(index);
        return w ? w->geometry() : QVariant();
    }
    case LevelRole: {
        auto w = widgetForIndex(index);
        return w ? w->level() : QVariant();
    }
    default:
        return QSortFilterProxyModel::data(index, role);
    }
}

QMap<int, QVariant> Widget3DModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> data;
    data[TextureRole] = index.data(TextureRole);
    data[BackTextureRole] = index.data(BackTextureRole);
    data[GeometryRole] = index.data(GeometryRole);
    data[LevelRole] = index.data(LevelRole);
    return data;
}

namespace {
    inline int parentDepth(QObject *obj) {
        QObject *p = obj;
        int depth = 0;
        while (p->parent()) {
            ++depth;
            p = p->parent();
        }
        return depth;
    }
}

Widget3DWidget *Widget3DModel::widgetForObject(QObject *obj) const
{
    Widget3DWidget *widget = mDataCache.value(obj, Q_NULLPTR);
    if (!widget) {
        Widget3DWidget *parent = Q_NULLPTR;
        if (obj->parent()) {
             parent = widgetForObject(obj->parent());
             Q_ASSERT(parent);
        }
        widget = new Widget3DWidget(qobject_cast<QWidget*>(obj), parentDepth(obj), parent);
        connect(widget, &Widget3DWidget::geometryChanged,
                this, &Widget3DModel::widgetGeometryChanged);
        connect(widget, &Widget3DWidget::textureChanged,
                this, &Widget3DModel::widgetTextureChanged);
        connect(widget, &Widget3DWidget::visibleChanged,
                this, &Widget3DModel::invalidate);
        mDataCache.insert(obj, widget);
    }
    return widget;
}

Widget3DWidget *Widget3DModel::widgetForIndex(const QModelIndex &idx) const
{
   QObject *obj = this->QSortFilterProxyModel::data(idx, ObjectModel::ObjectRole).value<QObject*>();
   if (!obj || !obj->isWidgetType()) {
       return Q_NULLPTR;
   }

   return widgetForObject(obj);
}

bool Widget3DModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const QModelIndex sourceIdx = sourceModel()->index(source_row, 0, source_parent);
    QObject *sourceObj = sourceModel()->data(sourceIdx, ObjectModel::ObjectRole).value<QObject*>();
    QWidget *w = qobject_cast<QWidget *>(sourceObj);
    return w && w->isVisible();
}

bool Widget3DModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QObject *objLeft = sourceModel()->data(source_left, ObjectModel::ObjectRole).value<QObject*>();
    QObject *objRight = sourceModel()->data(source_right, ObjectModel::ObjectRole).value<QObject*>();

    return parentDepth(objLeft) < parentDepth(objRight);
}

void Widget3DModel::widgetGeometryChanged()
{
    Widget3DWidget *widget = qobject_cast<Widget3DWidget*>(sender());
    Q_ASSERT(widget);

    // FIXME: Can this be even more inefficient?
    for (int i = 0; i < rowCount(); ++i) {
        const QModelIndex idx = index(i, 0, QModelIndex());
        if (widgetForIndex(idx) == widget) {
            Q_EMIT dataChanged(idx, idx, { GeometryRole });
            break;
        }
    }
}

void Widget3DModel::widgetTextureChanged()
{
    Widget3DWidget *widget = qobject_cast<Widget3DWidget*>(sender());
    Q_ASSERT(widget);

    // FIXME: Can this be even more inefficient?
    for (int i = 0; i < rowCount(); ++i) {
        const QModelIndex idx = index(i, 0, QModelIndex());
        if (widgetForIndex(idx) == widget) {
            Q_EMIT dataChanged(idx, idx, { TextureRole });
            break;
        }
    }
}
