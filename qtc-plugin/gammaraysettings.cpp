/**************************************************************************
**
** This file is part of Qt Creator Instrumentation Tools
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Author: Milian Wolff, KDAB (milian.wolff@kdab.com)
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#include "gammaraysettings.h"
#include "gammarayconfigwidget.h"

#include <coreplugin/icore.h>
#include <utils/qtcassert.h>

#include <QtCore/QSettings>
#include <QtCore/QDebug>

using namespace Analyzer;

namespace GammaRay {

static const QLatin1String executableC("Analyzer.GammaRay.Executable");
static const QLatin1String injectorC("Analyzer.GammaRay.Injector");

QVariantMap GammaRayBaseSettings::toMap() const
{
    QVariantMap map;

    map.insert(executableC, m_gammarayExecutable);
    map.insert(injectorC, m_injector);

    return map;
}

QVariantMap GammaRayBaseSettings::defaults() const
{
    QVariantMap map;

    map.insert(executableC, QLatin1String("gammaray"));
    map.insert(injectorC, Constants::DefaultInjector);

    return map;
}

bool GammaRayBaseSettings::fromMap(const QVariantMap &map)
{
    setIfPresent(map, executableC, &m_gammarayExecutable);
    int injector = 0;
    setIfPresent(map, injectorC, &injector);
    QTC_CHECK(injector >= 0 && injector < Constants::INJECTOR_COUNT);
    m_injector = static_cast<Constants::InjectorType>(injector);

    emit changed();
    return true;
}

QString GammaRayBaseSettings::id() const
{
    return "Analyzer.GammaRay.Settings";
}

QString GammaRayBaseSettings::displayName() const
{
    return tr("GammaRay");
}

QString GammaRayBaseSettings::gammarayExecutable() const
{
    return m_gammarayExecutable;
}

void GammaRayBaseSettings::setGammaRayExecutable(const QString &exe)
{
    m_gammarayExecutable = exe;
}

Constants::InjectorType GammaRayBaseSettings::injector() const
{
    return m_injector;
}

void GammaRayBaseSettings::setInjector(Constants::InjectorType injector)
{
    m_injector = injector;
    QTC_CHECK(m_injector == injector);
}

QWidget *GammaRayBaseSettings::createConfigWidget(QWidget *parent)
{
    return new GammaRayConfigWidget(this, parent);
}

}
