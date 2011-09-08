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

#ifndef ENDOSCOPESETTINGS_H
#define ENDOSCOPESETTINGS_H

#include <analyzerbase/analyzersettings.h>

#include "endoscopeconstants.h"

namespace Endoscope {

/**
 * Endoscope settings shared for global and per-project.
 */
class EndoscopeBaseSettings : public Analyzer::AbstractAnalyzerSubConfig
{
    Q_OBJECT

public:
    EndoscopeBaseSettings() {}

    virtual QVariantMap toMap() const;
    virtual QVariantMap defaults() const;
    virtual bool fromMap(const QVariantMap &map);

    virtual QString id() const;
    virtual QString displayName() const;

    virtual QWidget* createConfigWidget(QWidget* parent);

signals:
    void changed(); // sent when multiple values have changed simulatenously (e.g. fromMap)

/**
 * Base endoscope settings
 */
public:
    QString endoscopeExecutable() const;
    Constants::InjectorType injector() const;

public slots:
    void setEndoscopeExecutable(const QString &);
    void setInjector(Constants::InjectorType);

signals:
    void endoscopeExecutableChanged(const QString &);
    void injectorChanged(Constants::InjectorType);

private:
    QString m_endoscopeExecutable;
    Constants::InjectorType m_injector;

};


/**
 * Global endoscope settings
 */
typedef EndoscopeBaseSettings EndoscopeGlobalSettings;

/**
 * Per-project endoscope settings.
 */
typedef EndoscopeBaseSettings EndoscopeProjectSettings;

} // namespace Endoscope

#endif // ENDOSCOPESETTINGS_H
