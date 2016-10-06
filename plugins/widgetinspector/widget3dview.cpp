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
#include "widget3dcameracontroller.h"
#include "widget3dwindowmodel.h"
#include "widget3dsubtreemodel.h"

#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <common/objectid.h>

#include <3rdparty/kde/kdescendantsproxymodel.h>

#include <QWindow>
#include <QQuickView>
#include <QVBoxLayout>
#include <QVariant>
#include <QUrl>
#include <QWheelEvent>
#include <QTimer>
#include <QComboBox>
#include <QLabel>
#include <QTreeView>

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

class Widget3DWindow : public QQuickView
{
   Q_OBJECT
public:
    explicit Widget3DWindow(QWindow *parent = Q_NULLPTR)
        : QQuickView(parent)
    {
        resize(800, 600);
        setResizeMode(QQuickView::SizeRootObjectToView);
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

class Widget3DRoleModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit Widget3DRoleModel(QObject *parent = Q_NULLPTR)
        : QIdentityProxyModel(parent)
    {}

    ~Widget3DRoleModel() = default;

    // Map the server-side role names for easy use from QML
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE
    {
        QHash<int, QByteArray> roles;
        roles[Widget3DModel::IdRole] = "objectId";
        roles[Widget3DModel::GeometryRole] = "geometry";
        roles[Widget3DModel::TextureRole] = "frontTexture";
        roles[Widget3DModel::BackTextureRole] = "backTexture";
        roles[Widget3DModel::IsWindowRole] = "isWindow";
        roles[Widget3DModel::MetaDataRole] = "metaData";
        roles[Widget3DModel::DepthRole] = "depth";
        return roles;
    }
};

class Widget3DClientModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit Widget3DClientModel(QObject *parent = Q_NULLPTR)
        : QSortFilterProxyModel(parent)
    {
    }

    ~Widget3DClientModel() = default;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE
    {
        // Filter out rows that we don't have additional roles for yet (since data
        // are retrieved lazily by the RemoteModel). This greatly simplifies all
        // the other models (and the QML code), since we don't have to care
        // about null data everywhere.
        const QModelIndex source_idx = sourceModel()->index(source_row, 0, source_parent);
        if (source_idx.data(Widget3DModel::IdRole).isNull()) {
            // HACK: RemoteModel will reset the "Loading" flag of any node after calling
            // endInsertRows(). This method is invoked from endInsertRows() so even
            // though we accessed data() above, no data will be retrieved (well, they
            // will be retrieved, but then discarded upon retrieval, because the
            // corresponding Node does not have "Loading" flag set). So we do the sneaky
            // trick with a timer to force RemoteModel to fetch the data from the server,
            // because we really want them.
            QTimer::singleShot(0, [this, source_idx]() {
                source_idx.data(Widget3DModel::IdRole);
            });
            return false;
        }

        // Filter out rows that don't have a (valid) texture. This basically
        // filters out all invisible widgets, which we don't want to render and
        // deal with in the models.
        if (source_idx.data(Widget3DModel::TextureRole).value<QImage>().isNull()
            || source_idx.data(Widget3DModel::GeometryRole).isNull()) {
            return false;
        }

        return true;
    }
};

}

Q_DECLARE_METATYPE(GammaRay::Widget3DWindow*)

using namespace GammaRay;

Widget3DView::Widget3DView(QWidget* parent)
    : QWidget(parent)
{
    auto remoteModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.Widget3DModel"));
    // Filter out rows without data and invisible widgets
    auto clientModel = new Widget3DClientModel(this);
    clientModel->setSourceModel(remoteModel);

    // WINDOWS
    // Flatten the tree
    auto flatModel = new KDescendantsProxyModel(this);
    flatModel->setSourceModel(clientModel);
    // Filter out everything except for window-type widgets
    auto windowModel = new Widget3DWindowModel(this);
    windowModel->setSourceModel(flatModel);

    // WIDGETS
    // Filter out widget subtrees that don't belong to currently selected window
    auto subtreeModel = new Widget3DSubtreeModel(this);
    subtreeModel->setSourceModel(clientModel);
    // Flatten the filtered tree, the resulting list can be rendered by Qt3D
    auto flatSubtreeModel = new KDescendantsProxyModel(this);
    flatSubtreeModel->setSourceModel(subtreeModel);
    // Finally add a proxy which will be exposed to QML and which contains
    // C++ -> QML role mapping
    auto widgetModel = new Widget3DRoleModel(this);
    widgetModel->setSourceModel(flatSubtreeModel);

    auto vbox = new QVBoxLayout(this);

    auto hbox = new QHBoxLayout();
    hbox->addWidget(new QLabel(tr("Window: ")));

    QComboBox *combo = new QComboBox;
    combo->setModel(windowModel);
    connect(combo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=]() {
                subtreeModel->setRootObjectId(combo->currentData(Widget3DModel::IdRole).toString());
            });
    hbox->addWidget(combo, 1);
    vbox->addLayout(hbox);

    mRenderWindow = new Widget3DWindow();
    vbox->addWidget(QWidget::createWindowContainer(mRenderWindow, this), 1);

    qmlRegisterType<Widget3DCameraController>("com.kdab.GammaRay", 1, 0, "Widget3DCameraController");
    qmlRegisterType<Widget3DImageTextureImage>("com.kdab.GammaRay", 1, 0, "Widget3DImageTextureImage");
    qmlRegisterType<Widget3DClientModel>("com.kdab.GammaRay", 1, 0, "Widget3DClientModel");

    auto engine = mRenderWindow->engine();
    engine->rootContext()->setContextProperty(QStringLiteral("_renderWindow"), mRenderWindow);
    engine->rootContext()->setContextProperty(QStringLiteral("_widgetModel"), widgetModel);
    mRenderWindow->setSource(QUrl(QStringLiteral("qrc:/assets/qml/main.qml")));
}

Widget3DView::~Widget3DView()
{
    delete mRenderWindow;
}

#include "widget3dview.moc"

