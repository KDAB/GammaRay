/*
  widget3dmodel.h

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


#ifndef WIDGET3DMODEL_H
#define WIDGET3DMODEL_H

#include <QSortFilterProxyModel>
#include <QRect>
#include <QWidget>
#include <QMap>
#include <QPointer>

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
    void updateTimeout();
    void updateTexture();
    void updateGeometry();

private:
    void startUpdateTimer();

private:
    QPointer<QWidget> mQWidget;
    QImage mTextureImage;
    QImage mBackTextureImage;
    QRect mTextureGeometry;
    QRect mGeometry;
    int mLevel;
    bool mIsPainting;

    QTimer *mUpdateTimer;
    bool mGeomDirty;
    bool mTextureDirty;
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
    void onWidgetGeometryChanged();
    void onWidgetTextureChanged();
    void onRowsRemoved(const QModelIndex &parent, int first, int last);

private:
    Widget3DWidget *widgetForObject(QObject *object, bool createWhenMissing = true) const;
    Widget3DWidget *widgetForIndex(const QModelIndex &idx, bool createWhenMissing = true) const;

    // mutable becasue we populate it lazily from data() const
    mutable QHash<QObject *, Widget3DWidget*> mDataCache;
};

}

Q_DECLARE_METATYPE(GammaRay::Widget3DWidget*)

#endif // WIDGET3DMODEL_H
