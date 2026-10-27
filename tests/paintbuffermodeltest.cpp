/*
  paintbuffermodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <core/paintbuffer.h>
#include <core/paintbuffermodel.h>

#include <common/paintbuffermodelroles.h>

#include <QAbstractItemModelTester>
#include <QPainter>
#include <QImage>
#include <QPainterPath>
#include <QPixmap>
#include <QTransform>
#include <QTest>

using namespace GammaRay;

class PaintBufferModelTest : public QObject
{
    Q_OBJECT
private:
    // a bit of everything, the clip only applies between save() and restore()
    static PaintBuffer paintSomething()
    {
        PaintBuffer buffer;
        QPainter painter(&buffer);
        painter.fillRect(QRect(0, 0, 100, 100), Qt::white);
        painter.save();
        painter.setClipRect(QRect(10, 10, 20, 20));
        painter.setPen(Qt::red);
        painter.drawLine(0, 0, 50, 50);
        painter.restore();
        painter.translate(5, 5);
        painter.drawRect(QRect(1, 2, 3, 4));
        QPainterPath path;
        path.addEllipse(QRectF(20, 20, 30, 30));
        painter.drawPath(path);
        // only images and pixmaps have arguments
        QPixmap pixmap(8, 8);
        pixmap.fill(Qt::blue);
        painter.drawPixmap(QPoint(60, 60), pixmap);
        QImage image(4, 4, QImage::Format_ARGB32);
        image.fill(Qt::green);
        painter.drawImage(QRectF(70, 70, 8, 8), image, QRectF(0, 0, 4, 4));
        painter.end();
        return buffer;
    }

    static int findCommand(const QAbstractItemModel &model, const QString &name, int from = 0)
    {
        for (int row = from; row < model.rowCount(); ++row) {
            if (model.index(row, 0, {}).data().toString() == name)
                return row;
        }
        return -1;
    }

private slots:
    static void testStructure()
    {
        PaintBufferModel model;
        QAbstractItemModelTester tester(&model);
        model.setPaintBuffer(paintSomething());
        QVERIFY(model.rowCount() > 5);
        QCOMPARE(model.columnCount(), 3);

        bool hasArguments = false;
        for (int row = 0; row < model.rowCount(); ++row) {
            const auto command = model.index(row, 0, {});
            QVERIFY(!command.data().toString().isEmpty());
            QVERIFY(!command.parent().isValid());
            for (int arg = 0; arg < model.rowCount(command); ++arg) {
                hasArguments = true;
                const auto argument = model.index(arg, 0, command);
                QCOMPARE(argument.parent(), command);
                QVERIFY(!argument.data().toString().isEmpty());
            }
        }
        QVERIFY(hasArguments);
    }

    static void testData()
    {
        PaintBufferModel model;
        model.setPaintBuffer(paintSomething());

        const int transform = findCommand(model, QStringLiteral("SetTransform"));
        QVERIFY(transform >= 0);
        QCOMPARE(model.index(transform, 1, {}).data(Qt::EditRole).value<QTransform>(), QTransform::fromTranslate(5, 5));
        QCOMPARE(model.index(transform, 1, {}).data().toString(), QStringLiteral("[1 0 0, 0 1 0, 5 5 1]"));

        const int pixmap = findCommand(model, QStringLiteral("DrawPixmapPos"));
        QVERIFY(pixmap >= 0);
        const auto command = model.index(pixmap, 0, {});
        QCOMPARE(model.rowCount(command), 2);
        QCOMPARE(model.index(0, 1, command).data(PaintBufferModelRoles::ValueRole).toPointF(), QPointF(60, 60));
        QCOMPARE(model.index(1, 1, command).data(PaintBufferModelRoles::ValueRole).value<QPixmap>().size(), QSize(8, 8));
        QVERIFY(!model.index(0, 0, command).data().toString().isEmpty());

        const int image = findCommand(model, QStringLiteral("DrawImageRect"));
        QVERIFY(image >= 0);
        QCOMPARE(model.rowCount(model.index(image, 0, {})), 3);
    }

    // the clip that applies to a command, in the coordinates of the device
    static void testClipPath()
    {
        PaintBufferModel model;
        model.setPaintBuffer(paintSomething());

        const int clip = findCommand(model, QStringLiteral("ClipRect"));
        QVERIFY(clip >= 0);
        const int line = findCommand(model, QStringLiteral("DrawLineI"), clip);
        QVERIFY(line > clip);
        const auto clipPath = model.index(line, 0, {}).data(PaintBufferModelRoles::ClipPathRole).value<QPainterPath>();
        QCOMPARE(clipPath.boundingRect(), QRectF(10, 10, 20, 20));

        const int restore = findCommand(model, QStringLiteral("Restore"), line);
        QVERIFY(restore > line);
        QVERIFY(model.index(restore + 1, 0, {}).data(PaintBufferModelRoles::ClipPathRole).value<QPainterPath>().isEmpty());
    }

    static void testCosts()
    {
        PaintBufferModel model;
        QAbstractItemModelTester tester(&model);
        model.setPaintBuffer(paintSomething());
        QVector<double> costs(model.rowCount(), 1.0);
        costs[1] = 3.0;
        model.setCosts(costs);
        QCOMPARE(model.index(1, 2, {}).data().toDouble(), 3.0);
        QCOMPARE(model.index(0, 2, {}).data(PaintBufferModelRoles::MaxCostRole).toDouble(), 3.0);
    }

    static void testReplaceBuffer()
    {
        PaintBufferModel model;
        QAbstractItemModelTester tester(&model);
        model.setPaintBuffer(paintSomething());
        QVERIFY(model.rowCount() > 0);
        model.setPaintBuffer(PaintBuffer());
        QCOMPARE(model.rowCount(), 0);
        model.setPaintBuffer(paintSomething());
        QVERIFY(model.rowCount() > 0);
    }
};

QTEST_MAIN(PaintBufferModelTest)

#include "paintbuffermodeltest.moc"
