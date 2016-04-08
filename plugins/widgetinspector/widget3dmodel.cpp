#include "widget3dmodel.h"

#include <QDebug>
#include <QEvent>
#include <QTimer>
#include <QResizeEvent>

#include <common/objectmodel.h>
#include <core/objectlistmodel.h>

using namespace GammaRay;

Widget3DWidget::Widget3DWidget(QObject *parent)
    : QObject(parent)
    , mQWidget(Q_NULLPTR)
    , mLevel(0)
    , mIsPainting(false)
    , mUpdateTimer(Q_NULLPTR)
    , mGeomDirty(false)
    , mTextureDirty(false)
{
}

Widget3DWidget::Widget3DWidget(QWidget *qWidget, int level, Widget3DWidget *parent)
    : QObject(parent)
    , mQWidget(qWidget)
    , mLevel(level)
    , mIsPainting(false)
    , mUpdateTimer(Q_NULLPTR)
    , mGeomDirty(true)
    , mTextureDirty(true)
{
    mUpdateTimer = new QTimer(this);
    mUpdateTimer->setSingleShot(true);
    mUpdateTimer->setInterval(200);
    connect(mUpdateTimer, &QTimer::timeout,
            this, &Widget3DWidget::updateTimeout);

    //mUpdateTimer->start();
    updateTimeout();

    mQWidget->installEventFilter(this);
}

Widget3DWidget::~Widget3DWidget()
{
}

bool Widget3DWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == mQWidget) {
        switch (ev->type()) {
        case QEvent::Resize: {
            QResizeEvent *re = static_cast<QResizeEvent*>(ev);
            if (re->oldSize() != re->size()) {
                mGeomDirty = true;
                startUpdateTimer();
            }
            return false;
        }
        case QEvent::Paint:
            if (!mIsPainting) {
                mTextureDirty = true;
                startUpdateTimer();
            }
            return false;
        case QEvent::Show:
        case QEvent::Hide:
            Q_EMIT visibleChanged();
            return false;
        default:
            return false;
        }
    }

    return false;
}

void Widget3DWidget::startUpdateTimer()
{
    if (!mUpdateTimer->isActive()) {
        mUpdateTimer->start();
    }
}

void GammaRay::Widget3DWidget::updateTimeout()
{
    if (mGeomDirty) {
        updateGeometry();
    }
    if (mTextureDirty) {
        updateTexture();
    }
}


void Widget3DWidget::updateGeometry()
{
    if (!mGeomDirty || !mQWidget) {
        return;
    }

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

    mTextureDirty = true;
    mGeomDirty = false;
    Q_EMIT geometryChanged();
}

void Widget3DWidget::updateTexture()
{
    if (!mTextureDirty || !mQWidget) {
        return;
    }

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

    mTextureDirty = false;
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
        return w ? w->texture() : QImage();
    }
    case BackTextureRole: {
        auto w = widgetForIndex(index);
        return w ? w->backTexture() : QImage();
    }
    case GeometryRole: {
        auto w = widgetForIndex(index);
        return w ? w->geometry() : QRect();
    }
    case LevelRole: {
        auto w = widgetForIndex(index);
        return w ? w->level() : -1;
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

Widget3DWidget *Widget3DModel::widgetForObject(QObject *obj, bool createWhenMissing) const
{
    Widget3DWidget *widget = mDataCache.value(obj, Q_NULLPTR);
    if (!widget && createWhenMissing) {
        Widget3DWidget *parent = Q_NULLPTR;
        if (obj->parent()) {
             parent = widgetForObject(obj->parent(), createWhenMissing);
             Q_ASSERT(parent);
        }
        widget = new Widget3DWidget(qobject_cast<QWidget*>(obj), parentDepth(obj), parent);
        connect(widget, &Widget3DWidget::geometryChanged,
                this, &Widget3DModel::onWidgetGeometryChanged);
        connect(widget, &Widget3DWidget::textureChanged,
                this, &Widget3DModel::onWidgetTextureChanged);
        connect(widget, &Widget3DWidget::visibleChanged,
                this, &Widget3DModel::invalidate);
        mDataCache.insert(obj, widget);
    }
    return widget;
}

Widget3DWidget *Widget3DModel::widgetForIndex(const QModelIndex &idx, bool createWhenMissing) const
{
   QObject *obj = this->QSortFilterProxyModel::data(idx, ObjectModel::ObjectRole).value<QObject*>();
   if (!obj || !obj->isWidgetType()) {
       return Q_NULLPTR;
   }

   return widgetForObject(obj, createWhenMissing);
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

void Widget3DModel::onWidgetGeometryChanged()
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

void Widget3DModel::onWidgetTextureChanged()
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

void Widget3DModel::onRowsRemoved(const QModelIndex &parent, int first, int last)
{
    for (int i = last; i >= first; --i) {
        const QModelIndex idx = index(i, 0, parent);
        Widget3DWidget *widget = widgetForIndex(idx, false);
        if (widget) {
            mDataCache.remove(widget->qWidget());
            widget->deleteLater();
        }
    }
}
