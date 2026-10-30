/*
  messagehandlertest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"

#include <common/objectbroker.h>
#include <common/tools/messagehandler/messagemodelroles.h>

#include <QAbstractItemModel>
#include <QTest>

using namespace GammaRay;

class MessageHandlerTest : public BaseProbeTest
{
    Q_OBJECT
private:
    static bool hasMessage(QAbstractItemModel *model, const QString &text)
    {
        for (int row = 0; row < model->rowCount(); ++row) {
            if (model->index(row, MessageModelColumn::Message).data().toString() == text)
                return true;
        }
        return false;
    }

private slots:
    void testMessagesBeforeProbeCreation()
    {
        // what an application logs between QCoreApplication's creation and its event loop, e.g.
        // while loading QML, comes before the probe and its message handler tool exist
        Paths::setRelativeRootPath(GAMMARAY_INVERSE_BIN_DIR);
        qputenv("GAMMARAY_ProbePath", Paths::probePath(GAMMARAY_PROBE_ABI).toUtf8());
        qputenv("GAMMARAY_ServerAddress", GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Hooks::installHooks();
        Probe::startupHookReceived();
        new ProbeCreator(ProbeCreator::Create);
        qWarning("message before the probe");

        QTRY_VERIFY(Probe::isInitialized());
        auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.MessageModel"));
        QVERIFY(model);
        qWarning("message after the probe");
        QTRY_VERIFY(hasMessage(model, QStringLiteral("message after the probe")));
        QVERIFY(hasMessage(model, QStringLiteral("message before the probe")));
    }
};

QTEST_MAIN(MessageHandlerTest)

#include "messagehandlertest.moc"
