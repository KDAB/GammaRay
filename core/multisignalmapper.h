/*
  multisignalmapper.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_MULTISIGNALMAPPER_H
#define GAMMARAY_MULTISIGNALMAPPER_H

#include <QObject>
#include <QVariant>

namespace GammaRay {
class MultiSignalMapperPrivate;

/**
 * A signal mapper that can deal with multiple signals from the same sender.
 */
class MultiSignalMapper : public QObject
{
    Q_OBJECT
public:
    explicit MultiSignalMapper(QObject *parent = nullptr);
    ~MultiSignalMapper() override;

    void connectToSignal(QObject *sender, const QMetaMethod &signal);

signals:
    void signalEmitted(QObject *sender, int signalIndex, const QVector<QVariant> &arguments);

private:
    friend class MultiSignalMapperPrivate;
    MultiSignalMapperPrivate *const d;
};
}

#endif // GAMMARAY_MULTISIGNALMAPPER_H
