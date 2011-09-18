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

#include "endoscopesettings.h"
#include "endoscopeconfigwidget.h"

#include <coreplugin/icore.h>
#include <utils/qtcassert.h>

#include <QtCore/QSettings>
#include <QtCore/QDebug>

using namespace Analyzer;

namespace Endoscope {
static const QLatin1String executableC("Analyzer.Endoscope.Executable");
static const QLatin1String injectorC("Analyzer.Endoscope.Injector");

QVariantMap EndoscopeBaseSettings::toMap() const
{
    QVariantMap map;

    map.insert(executableC, m_endoscopeExecutable);
    map.insert(injectorC, m_injector);

    return map;
}

QVariantMap EndoscopeBaseSettings::defaults() const
{
    QVariantMap map;

    map.insert(executableC, QLatin1String("endoscope"));
    map.insert(injectorC, Constants::DefaultInjector);

    return map;
}

bool EndoscopeBaseSettings::fromMap(const QVariantMap &map)
{
    setIfPresent(map, executableC, &m_endoscopeExecutable);
    int injector = 0;
    setIfPresent(map, injectorC, &injector);
    QTC_CHECK(injector >= 0 && injector < Constants::INJECTOR_COUNT);
    m_injector = static_cast<Constants::InjectorType>(injector);

    emit changed();
    return true;
}

QString EndoscopeBaseSettings::id() const
{
    return "Analyzer.Endoscope.Settings";
}

QString EndoscopeBaseSettings::displayName() const
{
    return tr("Endoscope");
}

QString EndoscopeBaseSettings::endoscopeExecutable() const
{
    return m_endoscopeExecutable;
}

void EndoscopeBaseSettings::setEndoscopeExecutable(const QString &exe)
{
    m_endoscopeExecutable = exe;
}

Constants::InjectorType EndoscopeBaseSettings::injector() const
{
    return m_injector;
}

void EndoscopeBaseSettings::setInjector(Constants::InjectorType injector)
{
    m_injector = injector;
    QTC_CHECK(m_injector == injector);
}

QWidget *EndoscopeBaseSettings::createConfigWidget(QWidget *parent)
{
    return new EndoscopeConfigWidget(this, parent);
}

}
