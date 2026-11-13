/*
  quickrhigrabbertest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

// Tests the QtQuick remote view with the QRhi based screen grabber (RhiScreenGrabber).
// This needs a GPU: every test skips itself when the window is not rendered through QRhi,
// e.g. with QT_QUICK_BACKEND=software as on the Linux CI.

#include "basequicktest.h"

#include <plugins/quickinspector/quickinspectorinterface.h>
#include <common/objectbroker.h>
#include <common/remoteviewframe.h>
#include <common/remoteviewinterface.h>
#include <core/probe.h>
#include <core/remoteviewserver.h>

#include <QElapsedTimer>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickGraphicsConfiguration>
#include <QQuickItem>
#include <QQuickRenderControl>
#include <QQuickRenderTarget>
#include <QQuickWidget>
#include <QRegularExpression>
#include <QSGRendererInterface>
#include <QtGui/qtgui-config.h>
// QVulkanInstance is only defined when vulkan.h is available when building the application,
// a Vulkan enabled Qt alone is not enough
#if QT_CONFIG(vulkan) && __has_include(<vulkan/vulkan.h>)
#include <QVulkanInstance>
#define HAVE_QVULKANINSTANCE
#endif
#include <rhi/qrhi.h>

#include <functional>

using namespace GammaRay;

namespace {
const char *const s_qmlFile = "qrc:/manual/rotationinvariant.qml"; // four coloured quadrants

bool isRhiBased(QSGRendererInterface::GraphicsApi api)
{
    return QSGRendererInterface::isApiRhiBased(api);
}

bool isRhiBased(QQuickWindow *window)
{
    return isRhiBased(window->rendererInterface()->graphicsApi());
}

/// The frame as the client displays it.
QImage displayedImage(const RemoteViewFrame &frame)
{
    return frame.image().transformed(frame.transform());
}

/// Checks the colours of rotationinvariant.qml near the four corners of @p image.
/// Sampling relative to the image size works for any device pixel ratio.
bool hasQuadrants(const QImage &image)
{
    if (image.isNull())
        return false;
    const int r = image.width() - 3;
    const int b = image.height() - 3;
    return QColor(image.pixel(2, 2)) == QColor(255, 0, 0)
        && QColor(image.pixel(r, 2)) == QColor(0, 255, 0)
        && QColor(image.pixel(2, b)) == QColor(0, 0, 255)
        && QColor(image.pixel(r, b)) == QColor(255, 255, 0);
}
}

class QuickRhiGrabberTest : public BaseQuickTest
{
    Q_OBJECT

private:
    RemoteViewInterface *m_remoteView = nullptr;
    QuickInspectorInterface *m_inspector = nullptr;

    /**
     * Keeps rendering (@p render) and acknowledging received frames, as the client would, until a
     * frame for which @p accept returns true arrives. Returns an invalid frame on timeout.
     * Rendering continues while waiting because QRhi may only complete a readback in a later frame.
     */
    RemoteViewFrame waitForFrame(const std::function<void()> &render,
                                 const std::function<bool(const RemoteViewFrame &)> &accept)
    {
        QSignalSpy frameSpy(m_remoteView, &RemoteViewInterface::frameUpdated);
        QElapsedTimer timer;
        timer.start();
        while (timer.elapsed() < 5000) {
            m_remoteView->clientViewUpdated();
            render();
            QTest::qWait(20);
            for (const auto &args : std::as_const(frameSpy)) {
                const auto frame = args.at(0).value<RemoteViewFrame>();
                if (frame.isValid() && accept(frame))
                    return frame;
            }
            frameSpy.clear();
        }
        return {};
    }

    RemoteViewFrame waitForViewFrame(const std::function<bool(const RemoteViewFrame &)> &accept)
    {
        return waitForFrame([this] { view()->update(); }, accept);
    }

    /// Shows rotationinvariant.qml, or skips the test when that doesn't use QRhi.
    bool showRhiView(QQuickView::ResizeMode resizeMode = QQuickView::SizeViewToRootObject)
    {
        view()->setResizeMode(resizeMode);
        if (!showSource(QString::fromLatin1(s_qmlFile)))
            return false;
        if (!isViewExposed() || !isRhiBased(view()))
            return false;
        Probe::instance()->selectObject(view());
        m_remoteView->setViewActive(true);
        return true;
    }

private slots:
    void initTestCase()
    {
        // In release builds of Qt, QRhi only reports unreleased resources with this set.
        qputenv("QT_RHI_LEAK_CHECK", "1");
    }

    void init() override
    {
        BaseQuickTest::init();
        QTest::failOnWarning(QRegularExpression(QStringLiteral("unreleased resources")));

        m_remoteView = ObjectBroker::object<RemoteViewInterface *>(QStringLiteral("com.kdab.GammaRay.QuickRemoteView"));
        QVERIFY(m_remoteView);
        m_inspector = ObjectBroker::object<QuickInspectorInterface *>();
        QVERIFY(m_inspector);
        m_inspector->setServerSideDecorationsEnabled(false);
    }

    void cleanup() override
    {
        if (m_remoteView)
            m_remoteView->setViewActive(false);
        BaseQuickTest::cleanup();
    }

    // Swapchain readback, orientation, and the graphics API sent with the frame.
    void testQuickView()
    {
        if (!showRhiView())
            QSKIP("The view is not exposed or not rendered through QRhi");

        const auto frame = waitForViewFrame([](const RemoteViewFrame &) { return true; });
        QVERIFY(frame.isValid());
        QCOMPARE(frame.graphicsApi(), static_cast<int>(view()->rendererInterface()->graphicsApi()));

        const QImage image = displayedImage(frame);
        QCOMPARE(image.size(), view()->size() * view()->devicePixelRatio());
        QVERIFY(hasQuadrants(image));
    }

    // The decoration texture is recreated on the render thread and the frame follows the new size.
    void testResize()
    {
        if (!showRhiView(QQuickView::SizeRootObjectToView))
            QSKIP("The view is not exposed or not rendered through QRhi");
        QVERIFY(waitForViewFrame([](const RemoteViewFrame &) { return true; }).isValid());

        view()->resize(160, 120);
        const QSize expected = QSize(160, 120) * view()->devicePixelRatio();
        const auto frame = waitForViewFrame([&](const RemoteViewFrame &f) {
            return displayedImage(f).size() == expected;
        });
        QVERIFY(frame.isValid());
        QVERIFY(hasQuadrants(displayedImage(frame)));
    }

    // With server side decorations the RHI grabber draws them into the frame; they go away again
    // when disabled.
    void testDecorations()
    {
        if (!showRhiView())
            QSKIP("The view is not exposed or not rendered through QRhi");

        const QImage plain = displayedImage(waitForViewFrame([](const RemoteViewFrame &) { return true; }));
        QVERIFY(!plain.isNull());

        auto rootItem = view()->rootObject();
        QVERIFY(rootItem);
        QVERIFY(!rootItem->childItems().isEmpty());
        Probe::instance()->selectObject(rootItem->childItems().first());
        m_inspector->setServerSideDecorationsEnabled(true);

        const auto decorated = waitForViewFrame([&](const RemoteViewFrame &f) {
            const QImage image = displayedImage(f);
            return image.size() == plain.size() && image != plain;
        });
        QVERIFY2(decorated.isValid(), "no frame with decorations");

        m_inspector->setServerSideDecorationsEnabled(false);
        const auto plainAgain = waitForViewFrame([&](const RemoteViewFrame &f) {
            return displayedImage(f) == plain;
        });
        QVERIFY2(plainAgain.isValid(), "decorations were not removed");
    }

    // A QQuickWidget has no swapchain; the frame is read back from the texture it renders into.
    void testQuickWidget()
    {
        QQuickWidget widget;
        widget.setResizeMode(QQuickWidget::SizeRootObjectToView);
        widget.setSource(QUrl(QString::fromLatin1(s_qmlFile)));
        widget.resize(120, 90);
        widget.show();
        if (!QTest::qWaitForWindowExposed(&widget))
            QSKIP("The widget is not exposed");
        QQuickWindow *window = widget.quickWindow();
        if (!isRhiBased(window))
            QSKIP("The QQuickWidget is not rendered through QRhi");

        Probe::instance()->selectObject(window);
        m_remoteView->setViewActive(true);

        const QSize expected = QSize(120, 90) * widget.devicePixelRatioF();
        const auto frame = waitForFrame([window] { window->update(); }, [&](const RemoteViewFrame &f) {
            return displayedImage(f).size() == expected;
        });
        QVERIFY2(frame.isValid(), "no frame from the QQuickWidget");
        QVERIFY(hasQuadrants(displayedImage(frame)));
    }

    // A window driven by a custom QQuickRenderControl that renders into its own texture.
    void testRenderControl()
    {
        if (!isRhiBased(QQuickWindow::graphicsApi()))
            QSKIP("Qt Quick does not use QRhi");

#ifdef HAVE_QVULKANINSTANCE
        QVulkanInstance vulkanInstance; // must outlive the window
#endif
        QQuickRenderControl renderControl;
        QQuickWindow window(&renderControl);
        if (QQuickWindow::graphicsApi() == QSGRendererInterface::Vulkan) {
#ifdef HAVE_QVULKANINSTANCE
            vulkanInstance.setExtensions(QQuickGraphicsConfiguration::preferredInstanceExtensions());
            if (!vulkanInstance.create())
                QSKIP("Cannot create a Vulkan instance");
            window.setVulkanInstance(&vulkanInstance);
#else
            QSKIP("Qt is built without Vulkan or vulkan.h is missing");
#endif
        }

        QQmlEngine engine;
        QQmlComponent component(&engine, QUrl(QString::fromLatin1(s_qmlFile)));
        std::unique_ptr<QQuickItem> rootItem(qobject_cast<QQuickItem *>(component.create()));
        QVERIFY(rootItem);
        const QSize size(100, 100);
        window.contentItem()->setSize(size);
        window.setGeometry(0, 0, size.width(), size.height());
        rootItem->setParentItem(window.contentItem());

        if (!renderControl.initialize())
            QSKIP("QQuickRenderControl::initialize() failed");
        QRhi *rhi = renderControl.rhi();
        QVERIFY(rhi);

        // Destroyed before the window and the render control (reverse declaration order).
        const qreal dpr = window.effectiveDevicePixelRatio();
        const QSize pixelSize = size * dpr;
        std::unique_ptr<QRhiTexture> texture(rhi->newTexture(QRhiTexture::RGBA8, pixelSize, 1,
                                                             QRhiTexture::RenderTarget | QRhiTexture::UsedAsTransferSource));
        QVERIFY(texture->create());
        std::unique_ptr<QRhiRenderBuffer> depthStencil(rhi->newRenderBuffer(QRhiRenderBuffer::DepthStencil, pixelSize));
        QVERIFY(depthStencil->create());
        QRhiTextureRenderTargetDescription description { QRhiColorAttachment(texture.get()) };
        description.setDepthStencilBuffer(depthStencil.get());
        std::unique_ptr<QRhiTextureRenderTarget> renderTarget(rhi->newTextureRenderTarget(description));
        std::unique_ptr<QRhiRenderPassDescriptor> renderPass(renderTarget->newCompatibleRenderPassDescriptor());
        renderTarget->setRenderPassDescriptor(renderPass.get());
        QVERIFY(renderTarget->create());

        auto quickRenderTarget = QQuickRenderTarget::fromRhiRenderTarget(renderTarget.get());
        quickRenderTarget.setDevicePixelRatio(dpr);
        window.setRenderTarget(quickRenderTarget);

        auto renderFrame = [&] {
            renderControl.polishItems();
            renderControl.beginFrame();
            renderControl.sync();
            renderControl.render();
            renderControl.endFrame();
        };
        renderFrame();

        Probe::instance()->selectObject(&window);
        m_remoteView->setViewActive(true);

        const auto frame = waitForFrame(renderFrame, [&](const RemoteViewFrame &f) {
            return displayedImage(f).size() == pixelSize;
        });
        QVERIFY2(frame.isValid(), "no frame from the render control window");
        QVERIFY(hasQuadrants(displayedImage(frame)));

        // Let the inspector drop the window before it goes away.
        m_remoteView->setViewActive(false);
        rootItem.reset();
    }

    // Destroying the window right after a grab was rendered: on Vulkan the readback is then still
    // pending in QRhi. Must not crash or leak RHI resources (leaks fail through failOnWarning).
    void testDestroyWhileGrabbing()
    {
        if (!showRhiView())
            QSKIP("The view is not exposed or not rendered through QRhi");
        QVERIFY(waitForViewFrame([](const RemoteViewFrame &) { return true; }).isValid());

        auto server = qobject_cast<RemoteViewServer *>(m_remoteView);
        QVERIFY(server);
        QSignalSpy grabRequested(server, &RemoteViewServer::requestUpdate);
        QSignalSpy swapped(view(), &QQuickWindow::frameSwapped);

        m_remoteView->clientViewUpdated();
        view()->update();
        QVERIFY(grabRequested.wait());
        swapped.clear();
        QVERIFY(swapped.wait()); // the frame with the readback in it

        destroyView();
        QTest::qWait(200);
    }
};

QTEST_MAIN(QuickRhiGrabberTest)

#include "quickrhigrabbertest.moc"
