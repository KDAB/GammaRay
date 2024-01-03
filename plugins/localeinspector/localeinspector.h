/*
  localeinspector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_LOCALEINSPECTOR_LOCALEINSPECTOR_H
#define GAMMARAY_LOCALEINSPECTOR_LOCALEINSPECTOR_H

#include <core/toolfactory.h>

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {

class TimezoneOffsetDataModel;

class LocaleInspector : public QObject
{
    Q_OBJECT
public:
    explicit LocaleInspector(Probe *probe, QObject *parent = nullptr);

private:
    void timezoneSelected(const QItemSelection &selection);

    TimezoneOffsetDataModel *m_offsetModel;
};

class LocaleInspectorFactory : public QObject, public StandardToolFactory<QObject, LocaleInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_localeinspector.json")
public:
    explicit LocaleInspectorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_LOCALEINSPECTOR_H
