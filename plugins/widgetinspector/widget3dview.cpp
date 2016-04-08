/*
  widget3dview.cpp

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


#include "widget3dview.h"
#include "widget3dmodel.h"
#include "widget3dimagetextureimage.h"

#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <client/remotemodel.h>

#include <QWindow>
#include <QVBoxLayout>
#include <QVariant>
#include <QUrl>
#include <QWheelEvent>

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


class Widget3DModelClient : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit Widget3DModelClient(QObject *parent = Q_NULLPTR)
      : QSortFilterProxyModel(parent)
    {
    }

    ~Widget3DModelClient()
    {
    }

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE
    {
        // Looks like Qt3D does not like null QImages for textures, so let's
        // filter out the rows until a texture is received
        const QModelIndex source_idx = sourceModel()->index(source_row, 0, source_parent);
        return !source_idx.data(Widget3DModel::TextureRole).value<QImage>().isNull();
    }

    // Map the server-side role names for easy use from QML
    QHash<int, QByteArray> roleNames() const override
    {
        auto roles = QSortFilterProxyModel::roleNames();
        roles[Widget3DModel::GeometryRole] = "geometry";
        roles[Widget3DModel::TextureRole] = "frontTexture";
        roles[Widget3DModel::BackTextureRole] = "backTexture";
        roles[Widget3DModel::LevelRole] = "level";
        return roles;
    }
};


}

Q_DECLARE_METATYPE(GammaRay::Widget3DWindow*)

using namespace GammaRay;

Widget3DView::Widget3DView(QWidget* parent)
    : QWidget(parent)
{
    new QVBoxLayout(this);
    mWindow = new Widget3DWindow();
    layout()->addWidget(QWidget::createWindowContainer(mWindow, this));

    auto model = new Widget3DModelClient(this);
    model->setSourceModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.Widget3DModel")));

    qmlRegisterType<Widget3DImageTextureImage>("com.kdab.GammaRay", 1, 0, "Widget3DImageTextureImage");

    mEngine = new Qt3DCore::Quick::QQmlAspectEngine(this);
    mEngine->aspectEngine()->registerAspect(new Qt3DRender::QRenderAspect);
    mEngine->aspectEngine()->registerAspect(new Qt3DInput::QInputAspect);
    mEngine->aspectEngine()->registerAspect(new Qt3DLogic::QLogicAspect);

    mEngine->qmlEngine()->rootContext()->setContextProperty(QStringLiteral("_surface"), mWindow);
    mEngine->qmlEngine()->rootContext()->setContextProperty(QStringLiteral("_eventSource"), mWindow);
    mEngine->qmlEngine()->rootContext()->setContextProperty(QStringLiteral("_window"), mWindow);
    mEngine->qmlEngine()->rootContext()->setContextProperty(QStringLiteral("_widgetModel"), model);
    mEngine->setSource(QUrl(QStringLiteral("qrc:/assets/qml/main.qml")));
}

Widget3DView::~Widget3DView()
{
    delete mWindow;
}

#include "widget3dview.moc"

