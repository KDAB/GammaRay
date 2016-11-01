/*
  widget3dmodel.cpp

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


#include "widget3dmodel.h"

#include <QDebug>
#include <QEvent>
#include <QTimer>
#include <QResizeEvent>
#include <QMenu>
#include <QMetaObject>

#include <common/objectmodel.h>
#include <core/objecttreemodel.h>

#include <iostream>

using namespace GammaRay;

Widget3DWidget::Widget3DWidget(QObject *parent)
    : QObject(parent)
    , mQWidget(Q_NULLPTR)
    , mUpdateTimer(Q_NULLPTR)
    , mDepth(0)
    , mIsPainting(false)
    , mGeomDirty(false)
    , mTextureDirty(false)
{
}

Widget3DWidget::Widget3DWidget(QWidget *qWidget, const QPersistentModelIndex &idx,
                               Widget3DWidget *parent)
    : QObject(parent)
    , mModelIndex(idx)
    , mQWidget(qWidget)
    , mUpdateTimer(Q_NULLPTR)
    , mDepth(0)
    , mIsPainting(false)
    , mGeomDirty(true)
    , mTextureDirty(true)
{
    connect(qWidget, SIGNAL(destroyed(QObject*)),
            this, SLOT(deleteLater()));

    mUpdateTimer = new QTimer(this);
    mUpdateTimer->setSingleShot(true);
    mUpdateTimer->setInterval(200);
    connect(mUpdateTimer, &QTimer::timeout,
            this, &Widget3DWidget::updateTimeout);

    if (qWidget->isVisible()) {
        updateTimeout();
    }

    Widget3DWidget *w = this;
    while (w && !isWindow()) {
        ++mDepth;
        w = qobject_cast<Widget3DWidget*>(w->parent());
    }

    mQWidget->installEventFilter(this);

    mMetaData = { { QStringLiteral("className"), QString::fromUtf8(mQWidget->metaObject()->className()) },
                  { QStringLiteral("objectName"), mQWidget->objectName() },
                  { QStringLiteral("address"), quintptr(mQWidget.data()) },
                  { QStringLiteral("geometry"), mQWidget->geometry() },
                  { QStringLiteral("parent"), mQWidget->parent() ?
                        QVariantMap{ { QStringLiteral("className"), mQWidget->parent()->metaObject()->className() },
                                     { QStringLiteral("objectName"), mQWidget->parent()->objectName() },
                                     { QStringLiteral("address"), quintptr(mQWidget->parent()) } } :
                        QVariant() }
                };
}

Widget3DWidget::~Widget3DWidget()
{
}

bool Widget3DWidget::isWindow() const
{
    if (!mQWidget->isWindow()) {
        return false;
    }

    // Those are technically windows, but we don't want them listed in the window
    // list
    // TODO: any more exceptions?
    if (qobject_cast<QMenu*>(mQWidget)
        || qstrcmp(mQWidget->metaObject()->className(), "QTipLabel") == 0) {
        return false;
    }

    return true;
}

bool Widget3DWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == mQWidget) {
        switch (ev->type()) {
        case QEvent::Resize: {
            QResizeEvent *re = static_cast<QResizeEvent*>(ev);
            if (re->oldSize() != re->size()) {
                mMetaData[QStringLiteral("geometry")] = mQWidget->geometry();
                mGeomDirty = true;
                startUpdateTimer();
            }
            return false;
        }
        case QEvent::Paint: {
            if (!mIsPainting) {
                mTextureDirty = true;
                startUpdateTimer();
            }
            return false;
        }
        case QEvent::Show: {
            mGeomDirty = true;
            mTextureDirty = true;
            updateTimeout();
            return false;
        }
        case QEvent::Hide: {
            mTextureImage = QImage();
            mBackTextureImage = QImage();
            mUpdateTimer->stop();
            Q_EMIT changed(QVector<int>() << Widget3DModel::TextureRole
                                          << Widget3DModel::BackTextureRole);
            return false;
        }
        case QEvent::ParentChange: {
            // TODO: Handle ParentChange!
        }
        default:
            return false;
        }
    }

    return false;
}

void Widget3DWidget::startUpdateTimer()
{
    if (mQWidget->isVisible() && !mUpdateTimer->isActive()) {
        mUpdateTimer->start();
    }
}

void GammaRay::Widget3DWidget::updateTimeout()
{
    QVector<int> changedRoles;
    if (mGeomDirty && updateGeometry()) {
        changedRoles << Widget3DModel::GeometryRole;
    }
    if (mTextureDirty && updateTexture()) {
        changedRoles << Widget3DModel::TextureRole
                     << Widget3DModel::BackTextureRole;
    }

    if (!changedRoles.isEmpty()) {
        Q_EMIT changed(changedRoles);
    }
}


bool Widget3DWidget::updateGeometry()
{
    if (!mGeomDirty || !mQWidget) {
        return false;
    }

    QWidget *w = mQWidget;
    if (!w->isVisible()) {
        mGeomDirty = false;
        mTextureDirty = false;
        return false;
    }

    QPoint mappedPos(0, 0);
    // TODO: Use mapTo(), but it behaved somewhat weird...
    while (w->parentWidget()) {
        mappedPos += w->geometry().topLeft();
        w = w->parentWidget();
    }

    QRect textureGeometry(0, 0, mQWidget->width(), mQWidget->height());
    QRect geometry(mappedPos, QSize(mQWidget->width(), mQWidget->height()));
    QRect parentGeom;
    if (parent()) {
        // Artificial clipping - don't shrink texture coordinates
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

    bool changed = false;
    if (textureGeometry != mTextureGeometry) {
        mTextureGeometry = textureGeometry;
        mTextureDirty = true;
        changed = true;
    }
    if (geometry != mGeometry) {
        mGeometry = geometry;
        changed = true;
    }
    mGeomDirty = false;

    return changed;
}

bool Widget3DWidget::updateTexture()
{
    if (!mTextureDirty || !mQWidget) {
        return false;
    }

    if (!mQWidget->isVisible()) {
        mTextureDirty = false;
        return false;
    }

    mIsPainting = true;

    mTextureImage = QImage(mTextureGeometry.size(), QImage::Format_RGBA8888);
    mTextureImage.fill(mQWidget->palette().button().color());
    mQWidget->render(&mTextureImage, QPoint(0, 0), QRegion(mTextureGeometry), QWidget::DrawWindowBackground);

    if (isWindow()) {
        mBackTextureImage = QImage(mTextureGeometry.size(), QImage::Format_RGBA8888);
        mQWidget->render(&mBackTextureImage, QPoint(0, 0), QRegion(mTextureGeometry));
    } else {
        mBackTextureImage = mTextureImage;
    }

    mIsPainting = false;

    mTextureDirty = false;
    return true;
}



Widget3DModel::Widget3DModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

Widget3DModel::~Widget3DModel()
{
}

QHash<int, QByteArray> Widget3DModel::roleNames() const
{
    auto roles = QSortFilterProxyModel::roleNames();
    roles[IdRole] = "objectId";
    roles[TextureRole] = "frontTexture";
    roles[BackTextureRole] = "backTexture";
    roles[IsWindowRole] = "isWindow";
    roles[GeometryRole] = "geometry";
    roles[MetaDataRole] = "metaData";
    roles[DepthRole] = "depth";
    return roles;
}

QVariant Widget3DModel::data(const QModelIndex &index, int role) const
{
    if (index.column() == 0) {
        switch (role) {
        case IdRole: {
            auto w = widgetForIndex(index);
            return w ? w->id() : QString();
        }
        case TextureRole: {
            auto w = widgetForIndex(index);
            return w ? w->texture() : QImage();
        }
        case BackTextureRole: {
            auto w = widgetForIndex(index);
            return w ? w->backTexture() : QImage();
        }
        case IsWindowRole: {
            auto w = widgetForIndex(index);
            return w ? w->isWindow() : false;
        }
        case GeometryRole: {
            auto w = widgetForIndex(index);
            return w ? w->geometry() : QRect();
        }
        case MetaDataRole: {
            auto w = widgetForIndex(index);
            return w ? w->metaData() : QVariant();
        }
        case DepthRole: {
            auto w = widgetForIndex(index);
            return w ? w->depth() : 0;
        }
        }
    }

    return QSortFilterProxyModel::data(index, role);
}

QMap<int, QVariant> Widget3DModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> data = QSortFilterProxyModel::itemData(index);
    if (index.column() == 0) {
        auto w = widgetForIndex(index);
        Q_ASSERT(w);

        // see comment in data()
        data[ObjectModel::ObjectIdRole] = this->data(index, ObjectModel::ObjectIdRole);
        data[IdRole] = w->id();
        data[TextureRole] = w->texture();
        data[BackTextureRole] = w->backTexture();
        data[IsWindowRole] = w->isWindow();
        data[GeometryRole] = w->geometry();
        data[MetaDataRole] = w->metaData();
        data[DepthRole] = w->depth();
    }
    return data;
}

Widget3DWidget *Widget3DModel::widgetForObject(QObject *obj, const QModelIndex &idx,
                                               bool createWhenMissing) const
{
    Widget3DWidget *widget = mDataCache.value(obj, Q_NULLPTR);
    if (!widget && createWhenMissing) {
        Widget3DWidget *parent = Q_NULLPTR;
        if (obj->parent() && idx.parent().isValid()) {
            parent = widgetForObject(obj->parent(), idx.parent(), createWhenMissing);
        }
        widget = new Widget3DWidget(qobject_cast<QWidget*>(obj), idx, parent);
        connect(widget, &Widget3DWidget::changed,
                this, &Widget3DModel::onWidgetChanged);
        connect(obj, &QObject::destroyed,
                this, [=](QObject *obj) {
                    mDataCache.remove(obj);
                });
        mDataCache.insert(obj, widget);
    }
    return widget;
}

Widget3DWidget *Widget3DModel::widgetForIndex(const QModelIndex &idx, bool createWhenMissing) const
{
   QObject *obj = this->QSortFilterProxyModel::data(idx, ObjectModel::ObjectRole).value<QObject*>();
   Q_ASSERT(obj); // bug in model?
   Q_ASSERT(obj->isWidgetType()); // this should be already filtered out by filterAcceptsRow()

   return widgetForObject(obj, idx, createWhenMissing);
}

bool Widget3DModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const QModelIndex sourceIdx = sourceModel()->index(source_row, 0, source_parent);
    QObject *sourceObj = sourceModel()->data(sourceIdx, ObjectModel::ObjectRole).value<QObject*>();
    return qobject_cast<QWidget *>(sourceObj);
}

void Widget3DModel::onWidgetChanged(const QVector<int> &roles)
{
    const auto widget = qobject_cast<Widget3DWidget*>(sender());
    Q_ASSERT(widget);

    const QModelIndex idx = widget->modelIndex();
    if (!idx.isValid()) {
        // ????
        return;
    }

    Q_EMIT dataChanged(idx, idx, roles);
}
