/*
  multisignalmapper.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "multisignalmapper.h"

#include <QDebug>
#include <QMetaMethod>
#include <QMetaObject>
#include <QVariant>

namespace GammaRay {
class MultiSignalMapperPrivate : public QObject
{
public:
    explicit MultiSignalMapperPrivate(MultiSignalMapper *parent)
        : QObject(parent)
        , q(parent)
    {
    }
    ~MultiSignalMapperPrivate() override = default;

    int qt_metacall(QMetaObject::Call call, int methodId, void **args) override
    {
        methodId = QObject::qt_metacall(call, methodId, args);
        if (methodId < 0)
            return methodId;

        if (call == QMetaObject::InvokeMetaMethod) {
            Q_ASSERT(sender());
            const QVector<QVariant> v = convertArguments(sender(), methodId, args);
            emit q->signalEmitted(sender(), methodId, v);
            return -1; // indicates we handled the call
        }
        return methodId;
    }

    static QVector<QVariant> convertArguments(QObject *sender, int signalIndex, void **args)
    {
        Q_ASSERT(sender);
        Q_ASSERT(signalIndex >= 0);

        const QMetaMethod signal = sender->metaObject()->method(signalIndex);
        Q_ASSERT(signal.methodType() == QMetaMethod::Signal);

        QVector<QVariant> v;
        const QList<QByteArray> paramTypes = signal.parameterTypes();
        for (int i = 0; i < paramTypes.size(); ++i) {
            int type = QMetaType::fromName(paramTypes[i]).id();
            if (type == QMetaType::Void || type == QMetaType::UnknownType) {
                qWarning() << Q_FUNC_INFO << "unknown metatype for signal argument type"
                           << paramTypes[i];
                continue;
            }
            v.push_back(QVariant(QMetaType(type), args[i + 1]));
        }

        return v;
    }

private:
    MultiSignalMapper *q;
};
}

using namespace GammaRay;

MultiSignalMapper::MultiSignalMapper(QObject *parent)
    : QObject(parent)
    , d(new MultiSignalMapperPrivate(this))
{
}

MultiSignalMapper::~MultiSignalMapper() = default;

void MultiSignalMapper::connectToSignal(QObject *sender, const QMetaMethod &signal)
{
    QMetaObject::connect(sender, signal.methodIndex(), d,
                         QObject::metaObject()->methodCount() + signal.methodIndex(), Qt::AutoConnection | Qt::UniqueConnection,
                         nullptr);
}
