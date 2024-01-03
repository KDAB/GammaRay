/*
  resourcebrowserinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_RESOURCEBROWSERINTERFACE_H
#define GAMMARAY_RESOURCEBROWSERINTERFACE_H

#include <QObject>

namespace GammaRay {
class ResourceBrowserInterface : public QObject
{
    Q_OBJECT
public:
    explicit ResourceBrowserInterface(QObject *parent = nullptr);
    ~ResourceBrowserInterface() override;

public slots:
    virtual void downloadResource(const QString &sourceFilePath, const QString &targetFilePath) = 0;
    virtual void selectResource(const QString &sourceFilePath, int line = -1, int column = -1) = 0;

signals:
    void resourceDeselected();
    void resourceSelected(const QByteArray &contents, int line, int column);

    void resourceDownloaded(const QString &targetFilePath, const QByteArray &contents);
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::ResourceBrowserInterface,
                    "com.kdab.GammaRay.ResourceBrowserInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_RESOURCEBROWSERINTERFACE_H
