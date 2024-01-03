/*
  translatorinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef TRANSLATORINSPECTORINTERFACE_H
#define TRANSLATORINSPECTORINTERFACE_H

#include <QObject>

namespace GammaRay {
class TranslatorInspectorInterface : public QObject
{
    Q_OBJECT

public:
    explicit TranslatorInspectorInterface(const QString &name, QObject *parent);
    ~TranslatorInspectorInterface() override;

    const QString &name() const
    {
        return m_name;
    }

public slots:
    virtual void sendLanguageChangeEvent() = 0;
    virtual void resetTranslations() = 0;

private:
    QString m_name;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::TranslatorInspectorInterface,
                    "com.kdab.GammaRay.TranslatorInspectorInterface")
QT_END_NAMESPACE

#endif
