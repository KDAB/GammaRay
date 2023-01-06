/*
  widget3dmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef WIDGET3DMODEL_H
#define WIDGET3DMODEL_H

#include <QSortFilterProxyModel>
#include <QRect>
#include <QWidget>
#include <QMap>
#include <QPointer>
#include <QString>

#include <common/objectmodel.h>

#include <cstring>

namespace GammaRay {

class Widget3DWidget : public QObject
{
    Q_OBJECT

public:
    Widget3DWidget(QWidget *qWidget, const QPersistentModelIndex &modelIndex, Widget3DWidget *parent);
    ~Widget3DWidget() override;

    inline QImage texture() const
    {
        return mTextureImage;
    }
    inline QImage backTexture() const
    {
        return mBackTextureImage;
    }
    inline QRect geometry() const
    {
        return mGeometry;
    }
    inline QWidget *qWidget() const
    {
        return mQWidget;
    }
    inline Widget3DWidget *parentWidget() const
    {
        return static_cast<Widget3DWidget *>(parent());
    }
    inline bool isVisible() const
    {
        return mQWidget->isVisible();
    }
    inline QVariantMap metaData() const
    {
        return mMetaData;
    }
    inline bool isWindow() const;
    inline int depth() const
    {
        return mDepth;
    }
    inline QPersistentModelIndex modelIndex() const
    {
        return mModelIndex;
    }
    // QML does not handle 64bit integers, so use string instead, we only need
    // the value for comparison, we can convert back to quintptr in C++
    inline QString id() const
    {
        QString str(8, QLatin1Char('0'));
        quint64 ptr = reinterpret_cast<quint64>(mQWidget.data());
        // this is almost certainly wrong, but keep it for backwards compat:
        std::memcpy(static_cast<void *>(str.data()), static_cast<void *>(&ptr), 8);
        return str;
    };

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

Q_SIGNALS:
    void changed(const QVector<int> &roles);

private Q_SLOTS:
    void updateTimeout();
    bool updateTexture();
    bool updateGeometry();

private:
    void startUpdateTimer();

private:
    QPersistentModelIndex mModelIndex;
    QPointer<QWidget> mQWidget;
    QImage mTextureImage;
    QImage mBackTextureImage;
    QRect mTextureGeometry;
    QRect mGeometry;
    QVariantMap mMetaData;
    QTimer *mUpdateTimer;
    int mDepth;
    bool mIsPainting;
    bool mGeomDirty;
    bool mTextureDirty;
};

class Widget3DModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    enum Roles
    {
        IdRole = ObjectModel::UserRole + 1,
        TextureRole,
        BackTextureRole,
        IsWindowRole,
        GeometryRole,
        MetaDataRole,
        DepthRole,

        UserRole
    };

    explicit Widget3DModel(QObject *parent = nullptr);
    ~Widget3DModel() override;

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private Q_SLOTS:
    void onWidgetChanged(const QVector<int> &roles);
    void onWidgetDestroyed(QObject *obj);

private:
    Widget3DWidget *widgetForObject(QObject *obj, const QModelIndex &idx, bool createWhenMissing = true) const;
    Widget3DWidget *widgetForIndex(const QModelIndex &idx, bool createWhenMissing = true) const;

    // mutable because we populate it lazily from data() const
    mutable QHash<QObject *, Widget3DWidget *> mDataCache;
};

}

Q_DECLARE_METATYPE(GammaRay::Widget3DWidget *)

#endif // WIDGET3DMODEL_H
